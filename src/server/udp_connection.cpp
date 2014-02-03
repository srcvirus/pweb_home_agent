/*
 * udp_connection.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#include "udp_connection.h"
#include <boost/bind.hpp>

UDPConnection::UDPConnection(IOServicePool* ioServicePool, unsigned short localListenPort):
	ioServicePool(ioServicePool),
	localEndpoint(boost::asio::ip::udp::v4(), localListenPort),
	socket(ioServicePool->getPinnedIOService(), this->localEndpoint)
{
	;
}

void UDPConnection::listen()
{
	this->socket.async_receive_from(boost::asio::buffer(this->buffer), this->remoteEndpoint,
			boost::bind(&UDPConnection::handleDataReceived, this, this->buffer, boost::asio::placeholders::bytes_transferred()));
}

void UDPConnection::handleDataReceived(boost::array <char, MAX_UDP_BUFFER_SIZE>& buf, size_t bytesReceived)
{
	printf("Received %d bytes data from %s:%d\n", bytesReceived, this->remoteEndpoint.address().to_string().c_str(), this->remoteEndpoint.port());
	this->listen();
}
