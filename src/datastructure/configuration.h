/*
 * configuration.h
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string.h>

using namespace std;

class Configuration
{
	string alias;
	string hostname;
	unsigned short listenPort;
	unsigned char nThreads;

public:
	string& getAlias()
	{
		return alias;
	}

	void setAlias(const string& alias)
	{
		this->alias = alias;
	}

	string& getHostName()
	{
		return hostname;
	}

	void setHostName(const string& hostName)
	{
		this->hostname = hostName;
	}

	unsigned short& getListenPort()
	{
		return listenPort;
	}

	void setListenPort(unsigned short& listenPort)
	{
		this->listenPort = listenPort;
	}

	unsigned char& getThreads()
	{
		return nThreads;
	}

	void setThreads(unsigned char& threads)
	{
		nThreads = threads;
	}
};


#endif /* CONFIGURATION_H_ */
