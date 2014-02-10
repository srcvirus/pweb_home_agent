/*
 * pweb_home_agent.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#include <string>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "datastructure/configuration.h"
#include "server/home_agent_server/home_agent_server.h"
#include "server/http_server/http_server.h"
#include "global.h"

using namespace std;


boost::program_options::options_description config(USAGE_STRING);
boost::program_options::variables_map		programOptionMap;

Configuration programConfig;
boost::shared_ptr <IOServicePool> ioServicePool;
boost::shared_ptr <HomeAgentServer> haServer;
boost::shared_ptr <http_server> httpServer;

void populateConfigOptions(int, char*[]);

int main(int argc, char* argv[])
{
	bool optionsOk = true;

	try
	{
		populateConfigOptions(argc, argv);
		if(programOptionMap.count("help") > 0)
		{
			cout << config;
			return 0;
		}
		boost::program_options::notify(programOptionMap);
		ioServicePool = boost::shared_ptr <IOServicePool> (new IOServicePool(programConfig.getThreads(), 0x03));

		haServer = boost::shared_ptr <HomeAgentServer> (new HomeAgentServer (programConfig.getAlias(),
								 programConfig.getSuffix(),
								 programConfig.getHostName(),
								 programConfig.getListenPort(),
								 ioServicePool));

		httpServer = boost::shared_ptr <http_server> (new http_server (programConfig.getHostName(),
							   	   	   	   	   	   	   	   	   	   	   programConfig.getHttpListenPort(),
							   	   	   	   	   	   	   	   	   	   	   ioServicePool));
		haServer->start();
		httpServer->start_accept();
		ioServicePool->startServices();
	}
	catch(std::exception& ex)
	{
		cout << ex.what() << endl;
		cout << config;
	}

	return 0;
}

void populateConfigOptions(int argc, char* argv[])
{
	config.add_options()
			("help,h", "Print help message and exit")
			("alias,a", boost::program_options::value <string>(&programConfig.getAlias())->required(), "Home Agent alias")
			("port,p", boost::program_options::value <unsigned short>(&programConfig.getListenPort())->default_value(DEFAULT_LISTEN_PORT), "UDP port to listen for DNS queries")
			("http_port,P", boost::program_options::value <unsigned short>(&programConfig.getHttpListenPort())->default_value(DEFAULT_HTTP_LISTEN_PORT), "Port for listening REST API calls")
			("host,H", boost::program_options::value <string>(&programConfig.getHostName())->default_value(DEFAULT_HOST_NAME), "Host name of the home agent server")
			("suffix,s", boost::program_options::value <string> (&programConfig.getSuffix())->default_value(DEFAULT_SUFFIX), "Common suffix of the device names")
			("threads,t", boost::program_options::value <unsigned short>(&programConfig.getThreads())->default_value(N_CPU_THREADS), "Number of threads");

	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(config).run(), programOptionMap);
}


