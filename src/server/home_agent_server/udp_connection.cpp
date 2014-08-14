/*
 * udp_connection.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#include "udp_connection.h"

// temp
#include "../../protocol/dns/datastructure/dns_message.h"
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>

UDPConnection::UDPConnection(boost::shared_ptr<IOServicePool> &ioServicePool,
                             unsigned short localListenPort,
                             DNSMessageHandler &handler, const string &alias,
                             const string &suffix)
    : ioServicePool(ioServicePool),
      localEndpoint(boost::asio::ip::udp::v4(), localListenPort),
      socket(ioServicePool->getDedicatedIOService(), this->localEndpoint),
      handler(handler), thisConnection(this), alias(alias), suffix(suffix) {
  memset(this->buffer.elems, 0, sizeof(this->buffer.elems));
  this->socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
  pendingRequests.clear();
}

void UDPConnection::listen() {
  unsigned long tid = (unsigned long)pthread_self();
  printf("[DEBUG] [Thread 0x%lx] Listening on %s:%u\n", tid,
         this->localEndpoint.address().to_string().c_str(),
         this->localEndpoint.port());
  this->socket.async_receive_from(
      boost::asio::buffer(this->buffer), this->remoteEndpoint,
      boost::bind(&UDPConnection::handleDataReceived, this,
                  boost::asio::placeholders::bytes_transferred()));
}

void UDPConnection::handleDataSent() { ; }

void UDPConnection::handleDataReceived(size_t bytesReceived) {
  unsigned long tid = (unsigned long)pthread_self();
  printf("[DEBUG] [Thread 0x%lx] Received %lu bytes from %s:%u\n", tid,
         bytesReceived, this->remoteEndpoint.address().to_string().c_str(),
         this->remoteEndpoint.port());
  this->ioServicePool->getIOService().post(
      boost::bind(&DNSMessageHandler::handleDNSQueryRecive, (this->handler),
                  this->buffer, bytesReceived, thisConnection));
  this->listen();
}
