/*
 * home_agent_server.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: sr2chowd
 */

#include "home_agent_server.h"
#include <boost/bind.hpp>

void HomeAgentServer::startServer()
{
	boost::asio::ip::udp::endpoint remoteEndPoint;
	printf("Starting asynchronous receive on UDP port %u, from thread 0x%x, this = %p\n", this->serverListenPort, (unsigned int)pthread_self(), this);

	this->localUDPListenSocket.async_receive_from(boost::asio::buffer(this->receiveBuffer), remoteEndPoint,
			boost::bind(&HomeAgentServer::handleUDPDataReceive, this, remoteEndPoint, this->receiveBuffer, boost::asio::placeholders::bytes_transferred()));
}

void HomeAgentServer::handleUDPDataReceive(boost::asio::ip::udp::endpoint& remoteEndPoint, boost::array <char, HomeAgentServer::MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived)
{
	std::string receiveAddress = remoteEndPoint.address().to_v4().to_string();
	unsigned short receivePort = remoteEndPoint.port();
	printf("Received %lu bytes from %s:%u\n", bytesReceived, receiveAddress.c_str(), receivePort);
	printf("Received %lu bytes\n", bytesReceived);
	this->startServer();
}



