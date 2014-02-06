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
#include "../protocol/dns/dns_message_handler.h"
#include "../database/cassandra_db.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../models/home_agent_index.h"
#include "udp_connection.h"

class HomeAgentServer
{
	std::string hostName, ip, homeAgentAlias;
	unsigned short listenPort;
	boost::shared_ptr <CassandraDBDriver> database;
	IOServicePool ioServicePool;
	UDPConnection udpConnection;
	DNSMessageHandler handler;

	string getMyIp()
	{
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(this->hostName, "");
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
		boost::asio::ip::tcp::endpoint endpoint = *it;
		return endpoint.address().to_string();
	}

public:

	HomeAgentServer(const std::string& homeAgentAlias, const std::string& homeAgentHost, unsigned short serverListenPort, size_t nIOThreads, unsigned long cpuPinMask = 0x0):
		hostName(homeAgentHost),
		homeAgentAlias(homeAgentAlias),
		listenPort(serverListenPort),
		ioServicePool(nIOThreads, cpuPinMask),
		udpConnection(&this->ioServicePool, serverListenPort, handler)
	{
		printf("[INFO] Initializing home agent server\n");

		/* Initialize the database driver */
		database = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
		database->openConnection();

		/* Add the home agent alias to Cassandra database */
		HomeAgentIndexCassandraController haIndexController(database);
		//this->ip = this->udpConnection.getLocalEndpoint().address().to_string();
		this->ip = this->getMyIp();
		boost::shared_ptr <HomeAgentIndex> haIndex (new HomeAgentIndex(this->homeAgentAlias, this->ip, serverListenPort));
		haIndexController.addHomeAgentIndex(haIndex);

		/* Add home agent alias to the connection end point */
		this->udpConnection.setAlias(this->homeAgentAlias);
	}

	void start()
	{
		/* Start the io_services */
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
