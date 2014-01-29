/*
 * dns_message_handler.h
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#ifndef DNS_MESSAGE_HANDLER_H_
#define DNS_MESSAGE_HANDLER_H_

#include "../server/home_agent_server.h"

class DNSMessageHandler
{
public:

	static void handleDNSQuery(boost::array <char, 65536>& buffer, size_t bytesReceived);
};



#endif /* DNS_MESSAGE_HANDLER_H_ */
