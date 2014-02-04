/*
 * dns_message_handler.h
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#ifndef DNS_MESSAGE_HANDLER_H_
#define DNS_MESSAGE_HANDLER_H_

#include "../../global.h"
#include <boost/array.hpp>

class DNSMessageHandler
{
	//HomeAgentServer* containerServer;
public:
	void handleDNSQuery(boost::array <char, MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived);
	//void setContainerServer(HomeAgentServer* haServer){ this->containerServer = haServer; }
	//HomeAgentServer* getContainerSerer(){return this->containerServer;}
};



#endif /* DNS_MESSAGE_HANDLER_H_ */
