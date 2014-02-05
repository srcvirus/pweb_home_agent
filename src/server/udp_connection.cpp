/*
 * udp_connection.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#include "udp_connection.h"

//temp
#include "../protocol/dns/datastructure/dns_message.h"

#include <boost/bind.hpp>

UDPConnection::UDPConnection(IOServicePool* ioServicePool, unsigned short localListenPort):
	ioServicePool(ioServicePool),
	localEndpoint(boost::asio::ip::udp::v4(), localListenPort),
	socket(ioServicePool->getPinnedIOService(), this->localEndpoint)
{
	this->socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
}

void UDPConnection::listen()
{
	this->socket.async_receive_from(boost::asio::buffer(this->buffer), this->remoteEndpoint,
			boost::bind(&UDPConnection::handleDataReceived, this, this->buffer, boost::asio::placeholders::bytes_transferred()));
}

void UDPConnection::handleDataSent()
{
	;
}

void UDPConnection::handleDataReceived(boost::array <char, MAX_UDP_BUFFER_SIZE>& buf, size_t bytesReceived)
{
	printf("Received %d bytes data from %s:%d\n", bytesReceived, this->remoteEndpoint.address().to_string().c_str(), this->remoteEndpoint.port());
	DNSMessage message(buf.c_array());
	message.parse();
	message.getDNSHeader().print();
	for(int i = 0; i < message.getDNSQuestions().size(); i++)
		message.getDNSQuestions()[i].print();

	if(message.getDNSQuestions().size() > 0)
	{
		DNSMessage reply;
		unsigned short int ancount = 1;
		unsigned short int zero = 0;

		bool qr = true;

		reply.setDNSHeader(message.getDNSHeader());
		reply.getDNSHeader().setANCount(ancount);
		reply.getDNSHeader().setNSCount(zero);
		reply.getDNSHeader().setARCount(zero);
		reply.getDNSHeader().setQDCount(zero);

		reply.getDNSHeader().setQR(qr);						printf("reply.getDNSHeader().getQR(): %d\n", reply.getDNSHeader().getQR());
		//reply.setDNSQuestion(message.getDNSQuestions());

		unsigned int ttl = 0;
		unsigned short rdLength = sizeof(int);
		boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string("127.0.0.1");;

		unsigned int ip = (unsigned int)addr.to_ulong();
		unsigned char data[rdLength];
		memcpy(data, &ip, sizeof(int));
		vector <char> d;

		for(int i = rdLength - 1; i >= 0; i--) d.push_back(data[i]);

		DNSResourceRecord ans;
		ans.setLabels(message.getDNSQuestions()[0].getLabels());
		ans.setClass(message.getDNSQuestions()[0].getClass());
		ans.setRdLength(rdLength);
		ans.setData(d);
		ans.setType(message.getDNSQuestions()[0].getType());
		ans.setTtl(ttl);

		reply.getDNSAnswers().push_back(ans);

		reply.allocateBuffer();
		reply.write();
		reply.print();

		boost::asio::ip::udp::socket s(this->ioServicePool->getIOService(), this->localEndpoint);
		s.async_send_to(boost::asio::buffer(reply.getBuffer(), reply.getSize()), this->remoteEndpoint,
				boost::bind(&UDPConnection::handleDataSent, this));
	}
	this->listen();
}
