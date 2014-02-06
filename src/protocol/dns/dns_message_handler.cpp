/*
 * dns_message_handler.cpp
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#include "dns_message_handler.h"
#include "../../controllers/home_agent_index_cassandra_controller.h"
#include "../../models/home_agent_index.h"
#include "../../database/cassandra_db.h"

#include "../../global.h"

#include <boost/array.hpp>

#include "../../server/udp_connection.h"

#include <pthread.h>

using namespace boost;
using namespace std;

void DNSMessageHandler::handleDNSQueryRecive(boost::array <char, MAX_UDP_BUFFER_SIZE> buffer, size_t bytesReceived, boost::shared_ptr <UDPConnection>& connection)
{
	unsigned long tid = (unsigned long)pthread_self();
	printf("[DEBUG] Processing DNS query from thread 0x%lx\n", tid);

	int byteOffset = 0;
	DNSMessage dnsQueryMessage(buffer.c_array());
	dnsQueryMessage.parse();

	boost::asio::ip::udp::endpoint remoteEndpoint = connection->getRemoteEndpoint();

	if(dnsQueryMessage.getDNSHeader().getQR())
	{
		;
	}
	else
	{
		vector <DNSQuestion>& questions = dnsQueryMessage.getDNSQuestions();

		for(int i = 0; i < questions.size(); i++)
		{
			vector <string>& labels = questions[i].getLabels();

			if(labels.size() >= 3)
			{
				string& haAlias = labels[2];
				string& user = labels[1];
				string& device = labels[0];

				printf("[DEBUG] [Thread 0x%lx] haAlias = %s, user = %s, device = %s\n", tid, haAlias.c_str(), user.c_str(), device.c_str());
				if(haAlias == connection->getAlias())
				{
					// This is the target home agent
					printf("[DEBUG] [Thread 0x%lx] Current Home agent == Query Home agent (%s == %s)\n", tid, connection->getAlias().c_str(), haAlias.c_str());
					// lookup the user and device from the database
					// form a DNS reply message and send that to the requesting node
				}
				else
				{
					// This is not the target home agent
					// Lookup home agent's alias in cassandra
					// to forward to query to the target home agent

					boost::shared_ptr <CassandraDBDriver> dbDriver = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
					HomeAgentIndexCassandraController haIndexController(dbDriver);
					boost::shared_ptr <HomeAgentIndex> haIndex = haIndexController.getHomeAgentIndex(haAlias);
					if(haIndex)
					{
						boost::asio::ip::address_v4 remoteHAIp = boost::asio::ip::address_v4::from_string(haIndex->getIp());
						remoteEndpoint = boost::asio::ip::udp::endpoint(remoteHAIp, haIndex->getPort());
						printf("[DEBUG] [Thread 0x%lx] Found home agent %s, (%s:%d)\n", tid, haIndex->getName().c_str(), haIndex->getIp().c_str(), haIndex->getPort());
						this->forwardDNSMessage(dnsQueryMessage, remoteEndpoint, connection);
					}
				}
			}
		}
	}
}

void DNSMessageHandler::forwardDNSMessage(DNSMessage& message, boost::asio::ip::udp::endpoint& remoteEndpoint, boost::shared_ptr <UDPConnection>& connection)
{
	printf("[DEBUG] [Thread 0x%lx] Forwarding %lu bytes to %s:%u\n", (unsigned long)pthread_self(), message.getSize(), remoteEndpoint.address().to_string().c_str(), remoteEndpoint.port());
	connection->getSocket().async_send_to(boost::asio::buffer(message.getBuffer(), message.getSize()), remoteEndpoint,
			boost::bind(&UDPConnection::handleDataSent, connection));
}



