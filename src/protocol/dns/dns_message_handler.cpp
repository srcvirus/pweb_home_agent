/*
 * dns_message_handler.cpp
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#include "datastructure/dns_query.h"
#include "datastructure/dns_query_header.h"
#include "dns_message_handler.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../models/home_agent_index.h"
#include "../database/cassandra_db.h"

#include "../global.h"

#include <boost/array.hpp>

using namespace boost;
using namespace std;

void DNSMessageHandler::handleDNSQuery(boost::array <char, MAX_UDP_BUFFER_SIZE>& buffer, size_t bytesReceived)
{
	printf("Received %lu bytes\n", bytesReceived);
	int byteOffset = 0;

	DNSMessage dnsQuery(buffer.c_array());

	vector <DNSQuestion>& questions = dnsQuery.getDNSQuestions();

	for(int i = 0; i < questions.size(); i++)
	{
		vector <string>& labels = questions[i].getLabels();

		if(labels.size() >= 3)
		{
			string& haAlias = labels[0];
			string& user = labels[1];
			string& device = labels[2];

			//if(haAlias == this->containerServer->getHomeAgentAlias())
			{
				// lookup the user and device from the database
				// form a DNS reply message and send that to the requesting node
			}
			//else
			{
				boost::shared_ptr <CassandraDBDriver> dbDriver = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
				HomeAgentIndexCassandraController haIndexController(dbDriver);
				boost::shared_ptr <HomeAgentIndex> haIndex = haIndexController.getHomeAgentIndex(haAlias);
				if(haIndex)
				{
					// forward the dns message to the found home agent.
				}
			}
		}
	}

	/*byteOffset += DNSQueryHeader::getDNSHeaderLength();

	int curByte = 0;

	for( ;curByte < bytesReceived; )
	{
		for(int j = 0; curByte < bytesReceived && j < 8; j++)
		{
			printf("0x%x ", buffer.data()[curByte++]);
		}
		putchar('\n');
	}*/
}




