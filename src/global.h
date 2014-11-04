/*
 * global.h
 *
 *  Created on: Feb 2, 2014
 *      Author: sr2chowd
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <pthread.h>
#include <unistd.h>
#include <string>

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include <boost/shared_ptr.hpp>

using namespace std;

#define DO_LOG(__stream, __log_level__) __stream << log4cpp::Priority::__log_level__\
                                        << __FILE__ << ":" << __LINE__ << " Thread:" << std::hex << pthread_self() << "] "
#define LOG(__log_level__) DO_LOG(*logs::log.get(), __log_level__)

#define MAX_UDP_BUFFER_SIZE 65536
#define DNS_HEADER_LENGTH	12

#define N_CPU_THREADS	(sysconf(_SC_NPROCESSORS_ONLN))
#define DEFAULT_LISTEN_PORT	53
#define DEFAULT_HTTP_LISTEN_PORT 80
#define DEFAULT_HOST_NAME	"localhost"
#define DEFAULT_SUFFIX	"dht.pwebproject.net"
#define	DEFAULT_DESCRIPTION	"Home Agent"
#define USAGE_STRING	"Usage: pweb_home_agent [-a <home_agent_alias> | --alias=<home_agent_alias>] [optional args]\nOptional Args"

#define REST_API_SUGGESION_COUNT	5

namespace global
{
	const std::string GLOBAL_NAMESPACE_PREFIX = "pweb_global";
	const std::string LOCAL_NAMESPACE_PREFIX = "pweb_";
	const std::string IP_SOURCE_URL = "ifconfig.me";
};

namespace logs {
  extern boost::shared_ptr<log4cpp::Appender> fileAppender;
  extern boost::shared_ptr<log4cpp::Appender> consoleAppender;
  extern boost::shared_ptr<log4cpp::PatternLayout> logLayout;
  extern boost::shared_ptr<log4cpp::Category> log;
}
#endif  // GLOBAL_H_
