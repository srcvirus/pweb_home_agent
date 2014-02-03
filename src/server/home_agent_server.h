/*
 * HomeAgentServer.h
 *
 *  Created on: 2014-01-15
 *      Author: sr2chowd
 */

#ifndef HOMEAGENTSERVER_H_
#define HOMEAGENTSERVER_H_

/* Boost Headers */
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include "../global.h"

#include "../communication/io_service_pool.h"
#include "../protocol/dns_message_handler.h"
#include "../database/cassandra_db.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../models/home_agent_index.h"
#include "udp_connection.h"

class HomeAgentServer
{
	std::string hostName, ip, homeAgentAlias;
	boost::shared_ptr <CassandraDBDriver> database;
	IOServicePool ioServicePool;
	UDPConnection udpConnection;

public:

	HomeAgentServer(const std::string& homeAgentAlias, unsigned short serverListenPort, size_t nIOThreads, unsigned long cpuPinMask = 0x0):
		ioServicePool(nIOThreads, cpuPinMask),
		udpConnection(&this->ioServicePool, serverListenPort)
	{
		printf("Initializing home agent server\n");
		this->homeAgentAlias = homeAgentAlias;

		/* Initialize the database driver */
		database = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
		database->openConnection();

		/* Add the home agent alias to Cassandra database */
		HomeAgentIndexCassandraController haIndexController(database);
		this->ip = this->udpConnection.getLocalEndpoint().address().to_string();
		boost::shared_ptr <HomeAgentIndex> haIndex (new HomeAgentIndex(this->homeAgentAlias, this->ip, serverListenPort));
		haIndexController.addHomeAgentIndex(haIndex);

		this->udpConnection.listen();
		this->ioServicePool.startServices();
	}

	std::string& getHomeAgentAlias()
	{
		return this->homeAgentAlias;
	}

	void setHomeAgentAlias(const std::string& homeAgentAlias)
	{
		this->homeAgentAlias = homeAgentAlias;
	}

	UDPConnection& getUDPConnection()
	{
		return this->udpConnection;
	}
};



#endif /* HOMEAGENTSERVER_H_ */
