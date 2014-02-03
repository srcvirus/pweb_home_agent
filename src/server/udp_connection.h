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

#include "../global.h"
#include "../communication/io_service_pool.h"

class UDPConnection
{
	/* data buffer */
	boost::array <char, MAX_UDP_BUFFER_SIZE> buffer;

	/*End points of the connection*/
	boost::asio::ip::udp::endpoint remoteEndpoint;
	boost::asio::ip::udp::endpoint localEndpoint;

	/* The I/O Socket */
	boost::asio::ip::udp::socket socket;

	/* Pool of threads to handle the I/O and processing*/
	IOServicePool* ioServicePool;

public:
	UDPConnection(IOServicePool*, unsigned short);

	void listen();
	void handleDataReceived(boost::array <char, MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived);
	void handleDataSent();

	boost::asio::ip::udp::socket& getSocket(){ return this->socket; }
	boost::asio::ip::udp::endpoint& getLocalEndpoint(){ return this->localEndpoint; }
	boost::asio::ip::udp::endpoint& getRemoteEndpoint(){ return this->remoteEndpoint; }
};



#endif /* UDP_CONNECTION_H_ */
