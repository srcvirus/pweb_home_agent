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
	string suffix;
	unsigned short listenPort;
	unsigned short nThreads;

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

	unsigned short& getThreads()
	{
		return nThreads;
	}

	void setThreads(unsigned short& threads)
	{
		nThreads = threads;
	}

	string& getSuffix()
	{
		return suffix;
	}

	void setSuffix(const string& suffix)
	{
		this->suffix = suffix;
	}
};


#endif /* CONFIGURATION_H_ */
