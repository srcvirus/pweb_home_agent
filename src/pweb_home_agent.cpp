/*
 * pweb_home_agent.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "datastructure/configuration.h"
#include "server/home_agent_server.h"
#include "global.h"

using namespace std;


boost::program_options::options_description config(USAGE_STRING);
boost::program_options::variables_map		programOptionMap;

Configuration programConfig;

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

		HomeAgentServer haServer(programConfig.getAlias(),
								 programConfig.getSuffix(),
								 programConfig.getHostName(),
								 programConfig.getListenPort(),
								 programConfig.getThreads(),
								 0x01);
		haServer.start();
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
			("host,H", boost::program_options::value <string>(&programConfig.getHostName())->default_value(DEFAULT_HOST_NAME), "Host name of the home agent server")
			("suffix,s", boost::program_options::value <string> (&programConfig.getSuffix())->default_value(DEFAULT_SUFFIX), "Common suffix of the device names")
			("threads,t", boost::program_options::value <unsigned short>(&programConfig.getThreads())->default_value(N_CPU_THREADS), "Number of threads");

	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(config).run(), programOptionMap);
}


