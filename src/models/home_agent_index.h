/*
 * home_agent_index.h
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#ifndef HOME_AGENT_INDEX_H_
#define HOME_AGENT_INDEX_H_

#include <string>

using namespace std;

class HomeAgentIndex
{
	string name;
	string ip;
	string port;

public:

	static const string COL_NAME;
	static const string COL_IP;
	static const string COL_PORT;

	HomeAgentIndex(){;}
	HomeAgentIndex(string name, string ip, string port):name(name), ip(ip), port(port){;}

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

	const string& getPort() const
	{
		return port;
	}

	void setPort(const string& port)
	{
		this->port = port;
	}
};

const string HomeAgentIndex::COL_NAME = "name";
const string HomeAgentIndex::COL_IP = "ip";
const string HomeAgentIndex::COL_PORT = "port";

#endif /* HOME_AGENT_INDEX_H_ */
