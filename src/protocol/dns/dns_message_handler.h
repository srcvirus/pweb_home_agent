/*
 * dns_message_handler.h
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#ifndef DNS_MESSAGE_HANDLER_H_
#define DNS_MESSAGE_HANDLER_H_

#include "../../global.h"
#include "../../server/udp_connection.h"

#include "datastructure/dns_message.h"
#include "datastructure/dns_query_header.h"
#include "datastructure/dns_question.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

class UDPConnection;

class DNSMessageHandler
{
public:
	void handleDNSQueryRecive(boost::array <char, MAX_UDP_BUFFER_SIZE> buffer, size_t bytesReceived, boost::system::error_code error, UDPConnection* connection);
	void handleDNSQuerySend(boost::system::error_code error);
	void forwardDNSMessage(DNSMessage& message, boost::asio::ip::udp::endpoint& remoteEndPoint, UDPConnection* connection);
};



#endif /* DNS_MESSAGE_HANDLER_H_ */
