/*
 * udp_connection.h
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#ifndef UDP_CONNECTION_H_
#define UDP_CONNECTION_H_

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../global.h"
#include "../communication/io_service_pool.h"

#include "../protocol/dns/dns_message_handler.h"

class DNSMessageHandler;

class UDPConnection
{
	/* data buffer */
	boost::array <char, MAX_UDP_BUFFER_SIZE> buffer;

	/* alias of the container home agent */
	string alias;

	/*End points of the connection*/
	boost::asio::ip::udp::endpoint remoteEndpoint;
	boost::asio::ip::udp::endpoint localEndpoint;

	/* The I/O Socket */
	boost::asio::ip::udp::socket socket;

	/* Pool of threads to handle the I/O and processing*/
	IOServicePool* ioServicePool;

	/* The message handler */
	DNSMessageHandler& handler;

public:
	UDPConnection(IOServicePool*, unsigned short, DNSMessageHandler& handler);

	void listen();
	void handleDataReceived(boost::array <char, MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived, boost::system::error_code& error);
	void handleDataSent(boost::system::error_code& error);

	boost::asio::ip::udp::socket& getSocket(){ return this->socket; }
	boost::asio::ip::udp::endpoint& getLocalEndpoint(){ return this->localEndpoint; }
	boost::asio::ip::udp::endpoint& getRemoteEndpoint(){ return this->remoteEndpoint; }

	string& getAlias(){ return this->alias; }
	void setAlias(const string& alias){ this->alias = alias; }
};



#endif /* UDP_CONNECTION_H_ */
