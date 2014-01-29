/*
 * dns_message_handler.cpp
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#include "datastructure/dns_query.h"
#include "datastructure/dns_query_header.h"
#include "dns_message_handler.h"

#include <boost/array.hpp>

void DNSMessageHandler::handleDNSQuery(boost::array <char, 65536>& buffer, size_t bytesReceived)
{
	printf("Received %lu bytes\n", bytesReceived);
	int byteOffset = 0;
//	DNSQueryHeader qrHeader(buffer.c_array());
	DNSQuery dnsQuery(buffer.c_array());
	byteOffset += DNSQueryHeader::getDNSHeaderLength();

	int curByte = 0;

	for( ;curByte < bytesReceived; )
	{
		for(int j = 0; curByte < bytesReceived && j < 8; j++)
		{
			printf("0x%x ", buffer.data()[curByte++]);
		}
		putchar('\n');
	}
}




