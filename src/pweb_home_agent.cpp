/*
 * pweb_home_agent.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#include "global.h"

#include <string>
#include <time.h>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "datastructure/configuration.h"
#include "server/home_agent_server/home_agent_server.h"
#include "server/http_server/http_server.h"

using namespace std;

boost::program_options::options_description config(USAGE_STRING);
boost::program_options::variables_map programOptionMap;
Configuration programConfig;

boost::shared_ptr<IOServicePool> ioServicePool;
boost::shared_ptr<HomeAgentServer> haServer;
boost::shared_ptr<http_server> httpServer;

namespace logs {
boost::shared_ptr<log4cpp::Appender> fileAppender;
boost::shared_ptr<log4cpp::Appender> consoleAppender;
boost::shared_ptr<log4cpp::PatternLayout> logLayout;
boost::shared_ptr<log4cpp::Category> log;
}

void configureLog();
void populateConfigOptions(int, char * []);

int main(int argc, char *argv[]) {
  bool optionsOk = true;

  try {
    populateConfigOptions(argc, argv);
    if (programOptionMap.count("help") > 0) {
      cout << config;
      return 0;
    }
    configureLog();
    boost::program_options::notify(programOptionMap);
    ioServicePool = boost::shared_ptr<IOServicePool>(
        new IOServicePool(programConfig.getThreads(), 0x00));

    LOG(INFO) << " alias: " << programConfig.getAlias()
               << ", ip: " << programConfig.getIp()
               << ", host name: " << programConfig.getHostName()
               << ", description: " << programConfig.getDescription()
               << ", listen-port: " << programConfig.getListenPort();
    haServer = boost::shared_ptr<HomeAgentServer>(new HomeAgentServer(
        programConfig.getAlias(), programConfig.getIp(),
        programConfig.getSuffix(), programConfig.getHostName(),
        programConfig.getDescription(), programConfig.getListenPort(),
        ioServicePool));

    httpServer = boost::shared_ptr<http_server>(
        new http_server(programConfig.getHostName(), programConfig.getAlias(),
                        programConfig.getHttpListenPort(), ioServicePool));
    haServer->start();
    httpServer->start_accept();
    ioServicePool->startServices();
  }
  catch (std::exception & ex) {
    LOG(ERROR) << ex.what() << "\n" 
               << config;
  }

  return 0;
}

void configureLog() {
  using logs::log;
  using logs::fileAppender;
  using logs::consoleAppender;
  using logs::logLayout;
  log = boost::shared_ptr<log4cpp::Category>(&log4cpp::Category::getRoot());
  string log_file_name = "/var/log/home-agent-log.";
  time_t current_time;
  time(&current_time);
  tm* time_info = localtime(&current_time);
  char str_time[128];
  strftime(str_time, 128, "%Y%m%d%H%M%S", time_info);
  log_file_name += str_time;
  fileAppender = boost::shared_ptr<log4cpp::Appender>(
      new log4cpp::FileAppender(
        "pweb_logger", "/var/log/home-agent-log", false));
  consoleAppender = boost::shared_ptr<log4cpp::Appender>(
      new log4cpp::OstreamAppender("console", &std::cout));
  logLayout =
      boost::shared_ptr<log4cpp::PatternLayout>(new log4cpp::PatternLayout());
  logLayout->setConversionPattern("[%-5p %d{%a %b-%d-%Y %H:%M:%S.%l}: %m%n");
  fileAppender->setLayout(logLayout.get());
  consoleAppender->setLayout(logLayout.get());
  log->addAppender(fileAppender.get());
  log->addAppender(consoleAppender.get());
  log->setPriority(log4cpp::Priority::DEBUG);
}

void populateConfigOptions(int argc, char *argv[]) {
  config.add_options()("help,h", "Print help message and exit")(
      "alias,a", boost::program_options::value<string>(
                     &programConfig.getAlias())->required(),
      "Home Agent alias")("ip,I", boost::program_options::value<string>(
                                      &programConfig.getIp())->required(),
                          "IP address of Home Agent")(
      "port,p",
      boost::program_options::value<unsigned short>(
          &programConfig.getListenPort())->default_value(DEFAULT_LISTEN_PORT),
      "UDP port to listen for DNS queries")(
      "http_port,P", boost::program_options::value<unsigned short>(
                         &programConfig.getHttpListenPort())
                         ->default_value(DEFAULT_HTTP_LISTEN_PORT),
      "Port for listening REST API calls")(
      "host,H",
      boost::program_options::value<string>(&programConfig.getHostName())
          ->default_value(DEFAULT_HOST_NAME),
      "Host name of the home agent server")(
      "suffix,s",
      boost::program_options::value<string>(&programConfig.getSuffix())
          ->default_value(DEFAULT_SUFFIX),
      "Common suffix of the device names")(
      "description,d",
      boost::program_options::value<string>(&programConfig.getDescription())
          ->default_value(DEFAULT_DESCRIPTION),
      "Description of home agent")(
      "threads,t",
      boost::program_options::value<unsigned short>(&programConfig.getThreads())
          ->default_value(N_CPU_THREADS),
      "Number of threads");

  boost::program_options::store(boost::program_options::command_line_parser(
                                    argc, argv).options(config).run(),
                                programOptionMap);
}
