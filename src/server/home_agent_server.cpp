/*
 * home_agent_server.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: sr2chowd
 */

#include "home_agent_server.h"
#include <boost/bind.hpp>

#include "../protocol/datastructure/dns_query_header.h"
#include "../protocol/dns_message_handler.h"

void HomeAgentServer::startServer()
{
	boost::asio::ip::udp::endpoint remoteEndPoint;
	printf("Starting asynchronous receive on UDP port %u, from thread 0x%x, this = %p\n", this->serverListenPort, (unsigned int)pthread_self(), this);

	this->localUDPListenSocket.async_receive_from(boost::asio::buffer(this->receiveBuffer), remoteEndPoint,
			boost::bind(&HomeAgentServer::handleUDPDataReceive, this, remoteEndPoint, this->receiveBuffer, boost::asio::placeholders::bytes_transferred()));
}

void HomeAgentServer::handleUDPDataReceive(boost::asio::ip::udp::endpoint& remoteEndPoint, boost::array <char, MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived)
{
	this->ioServicePool.getIOService().post(boost::bind(DNSMessageHandler::handleDNSQuery, buffer, bytesReceived));
	this->startServer();
}



