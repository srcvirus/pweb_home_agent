/*
 * dns_message_handler.h
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#ifndef DNS_MESSAGE_HANDLER_H_
#define DNS_MESSAGE_HANDLER_H_

#include "../../global.h"
#include "../../server/home_agent_server/udp_connection.h"

#include "../protocol_helper.h"

#include "datastructure/typedefs.h"
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
	void handleDNSQueryRecive(boost::array <char, MAX_UDP_BUFFER_SIZE> buffer, size_t bytesReceived, boost::shared_ptr <UDPConnection> connection);
	void handleDNSQuerySend(boost::system::error_code error);
	void forwardDNSMessage(DNSMessage& message, boost::asio::ip::udp::endpoint& remoteEndPoint, boost::shared_ptr <UDPConnection> connection);
	bool isValidRequest(DNSMessage& message, const string& suffix);
	void composeFailReply(DNSMessage& query, DNSMessage& reply);
	void composeSuccessReply(DNSMessage& query, DNSMessage& reply, string& ansToQuestion);
};



#endif /* DNS_MESSAGE_HANDLER_H_ */
