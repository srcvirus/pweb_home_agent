/*
 * udp_connection.h
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#ifndef UDP_CONNECTION_H_
#define UDP_CONNECTION_H_

#include <map>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../../global.h"
#include "../../communication/io_service_pool.h"

#include "../../protocol/dns/dns_message_handler.h"
#include "../../protocol/dns/datastructure/dns_message.h"

#include "../../datastructure/lookup_table.h"

using namespace std;

class DNSMessageHandler;

class UDPConnection {
  /* data buffer */
  boost::array<char, MAX_UDP_BUFFER_SIZE> buffer;

  /* alias of the container home agent */
  string alias;

  /* suffix of the container home agent */
  string suffix;

  /*End points of the connection*/
  boost::asio::ip::udp::endpoint remoteEndpoint;
  boost::asio::ip::udp::endpoint localEndpoint;

  /* The I/O Socket */
  boost::asio::ip::udp::socket socket;

  /* Pool of threads to handle the I/O and processing*/
  boost::shared_ptr<IOServicePool> ioServicePool;

  /* The message handler */
  DNSMessageHandler &handler;

  /* Wrapper around this pointer */
  boost::shared_ptr<UDPConnection> thisConnection;

  /* pending requests */
  LookupTable<string, boost::asio::ip::udp::endpoint> pendingRequests;

public:
  UDPConnection(boost::shared_ptr<IOServicePool> &, unsigned short,
                DNSMessageHandler &handler, const string &alias,
                const string &suffix);

  void listen();
  void handleDataReceived(size_t bytesReceived);
  void handleDataSent();

  boost::asio::ip::udp::socket &getSocket() { return this->socket; }
  boost::asio::ip::udp::endpoint &getLocalEndpoint() {
    return this->localEndpoint;
  }
  boost::asio::ip::udp::endpoint &getRemoteEndpoint() {
    return this->remoteEndpoint;
  }

  string &getAlias() { return this->alias; }
  void setAlias(const string &alias) { this->alias = alias; }

  string &getSuffix() { return this->suffix; }
  void setSuffix(const string &suffix) { this->suffix = suffix; }

  LookupTable<string, boost::asio::ip::udp::endpoint> &getPendingRequests() {
    return this->pendingRequests;
  }
};

#endif /* UDP_CONNECTION_H_ */
