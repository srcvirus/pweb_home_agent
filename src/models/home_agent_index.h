/*
 * home_agent_index.h
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#ifndef HOME_AGENT_INDEX_H_
#define HOME_AGENT_INDEX_H_

#include <string>
#include <stdio.h>

using namespace std;

class HomeAgentIndex
{
	string name;
	string ip;
	int port;

public:

	static const string COL_NAME;
	static const string COL_IP;
	static const string COL_PORT;

	HomeAgentIndex(){;}
	HomeAgentIndex(string name, string ip, int port):name(name), ip(ip), port(port){;}

	const string& getIp() const
	{
		return ip;
	}

	void setIp(const string& ip)
	{
		this->ip = ip;
	}

	const string& getName() const
	{
		return name;
	}

	void setName(const string& name)
	{
		this->name = name;
	}

	int getPort() const
	{
		return port;
	}

	void setPort(int port)
	{
		this->port = port;
	}

	void print()
	{
		printf("{\n");
		printf("\t%s : %s\n", HomeAgentIndex::COL_NAME.c_str(), this->name.c_str());
		printf("\t%s : %s\n", HomeAgentIndex::COL_IP.c_str(), this->ip.c_str());
		printf("\t%s : %d\n", HomeAgentIndex::COL_PORT.c_str(), this->port);
		printf("}\n");
	}


};

#endif /* HOME_AGENT_INDEX_H_ */
