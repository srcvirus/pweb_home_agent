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

using namespace boost;
using namespace std;

void DNSMessageHandler::handleDNSQueryRecive(boost::array <char, MAX_UDP_BUFFER_SIZE> buffer, size_t bytesReceived, boost::system::error_code error, UDPConnection* connection)
{
	int byteOffset = 0;
	DNSMessage dnsQueryMessage(buffer.c_array());
	dnsQueryMessage.parse();

	vector <DNSQuestion>& questions = dnsQueryMessage.getDNSQuestions();

	for(int i = 0; i < questions.size(); i++)
	{
		vector <string>& labels = questions[i].getLabels();

		if(labels.size() >= 3)
		{
			string& haAlias = labels[0];
			string& user = labels[1];
			string& device = labels[2];

			if(haAlias == connection->getAlias())
			{
				// lookup the user and device from the database
				// form a DNS reply message and send that to the requesting node
			}
			else
			{
				boost::shared_ptr <CassandraDBDriver> dbDriver = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
				HomeAgentIndexCassandraController haIndexController(dbDriver);
				boost::shared_ptr <HomeAgentIndex> haIndex = haIndexController.getHomeAgentIndex(haAlias);
				if(haIndex)
				{
					boost::asio::ip::address_v4 remoteHAIp = boost::asio::ip::address_v4::from_string(haIndex->getIp());
					boost::asio::ip::udp::endpoint remoteHAEndpoint(remoteHAIp, haIndex->getPort());
					this->forwardDNSMessage(dnsQueryMessage, remoteHAEndpoint, connection);
				}
			}
		}
	}
}

void DNSMessageHandler::forwardDNSMessage(DNSMessage& message, boost::asio::ip::udp::endpoint& remoteEndPoint, UDPConnection* connection)
{
	connection->getSocket().async_send_to(boost::asio::buffer(message.getBuffer(), message.getSize()), remoteEndPoint,
			boost::bind(&UDPConnection::handleDataSent, connection, boost::asio::placeholders::error()));
}




