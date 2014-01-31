/*
 * HomeAgentServer.h
 *
 *  Created on: 2014-01-15
 *      Author: sr2chowd
 */

#ifndef HOMEAGENTSERVER_H_
#define HOMEAGENTSERVER_H_

#include "../communication/io_service_pool.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#define MAX_UDP_BUFFER_SIZE 65536

class HomeAgentServer
{
	boost::array <char, MAX_UDP_BUFFER_SIZE> receiveBuffer;

	std::string hostName;
	unsigned short int serverListenPort;

	IOServicePool ioServicePool;

	boost::asio::ip::udp::socket localUDPListenSocket;
	DNSMessageHandler dnsHandler;

public:

	HomeAgentServer(unsigned short serverListenPort, size_t nIOThreads, unsigned long cpuPinMask = 0x0):
		serverListenPort(serverListenPort),
		ioServicePool(nIOThreads, cpuPinMask),
		localUDPListenSocket(this->ioServicePool.getPinnedIOService(),
				boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), serverListenPort))
	{
		printf("Initializing home agent server\n");

		boost::asio::io_service& listeningService = localUDPListenSocket.get_io_service();
		listeningService.post(boost::bind(&HomeAgentServer::runServer, this));

		this->ioServicePool.startServices();
	}

	void runServer();
	void handleUDPDataReceive(boost::asio::ip::udp::endpoint& remoteEndPoint, boost::array <char, MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived);


};



#endif /* HOMEAGENTSERVER_H_ */
