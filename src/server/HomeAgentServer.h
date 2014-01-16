/*
 * HomeAgentServer.h
 *
 *  Created on: 2014-01-15
 *      Author: sr2chowd
 */

#ifndef HOMEAGENTSERVER_H_
#define HOMEAGENTSERVER_H_

class HomeAgentServer
{
	std::string hostName;
	unsigned short int serverListenPort;

	boost::asio::ip::udp::endpoint serverListenEndpoint;
	boost::asio::signal_set_service	registeredSignals;

public:

	void signalHandler();
};



#endif /* HOMEAGENTSERVER_H_ */
