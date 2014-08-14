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

#include "../../global.h"

#include "../../communication/io_service_pool.h"
#include "../../protocol/dns/dns_message_handler.h"
#include "../../database/cassandra_db.h"
#include "../../controllers/home_agent_index_cassandra_controller.h"
#include "../../models/home_agent_index.h"
#include "../../models/device.h"
#include "../../models/user.h"
#include "udp_connection.h"

class HomeAgentServer {
  std::string hostName, ip, homeAgentAlias, suffix, homeAgentDescription;
  unsigned short listenPort;
  boost::shared_ptr<CassandraDBDriver> database;
  boost::shared_ptr<IOServicePool> ioServicePool;
  boost::shared_ptr<UDPConnection> udpConnection;
  DNSMessageHandler handler;

  string getMyIp() {
    std::string command = "curl " + global::IP_SOURCE_URL;
    FILE *pHandler = popen(command.c_str(), "r");
    char strIp[20];
    string ip = "";

    if (!pHandler)
      return ip;
    fscanf(pHandler, "%s", strIp);
    ip += strIp;

    return ip;
  }

  void initModelNames() {
    HomeAgentIndex::TABLE_NAME =
        global::GLOBAL_NAMESPACE_PREFIX + "." + HomeAgentIndex::TABLE_NAME;
    User::TABLE_NAME = global::LOCAL_NAMESPACE_PREFIX + this->homeAgentAlias +
                       "." + User::TABLE_NAME;
    Device::TABLE_NAME = global::LOCAL_NAMESPACE_PREFIX + this->homeAgentAlias +
                         "." + Device::TABLE_NAME;
  }

public:

  HomeAgentServer(const std::string &homeAgentAlias, const std::string &ip,
                  const std::string &suffix, const std::string &homeAgentHost,
                  const std::string &homeAgentDescription,
                  unsigned short serverListenPort,
                  boost::shared_ptr<IOServicePool> &ioPool)
      : hostName(homeAgentHost), ip(ip), homeAgentAlias(homeAgentAlias),
        suffix(suffix), homeAgentDescription(homeAgentDescription),
        listenPort(serverListenPort), ioServicePool(ioPool),
        udpConnection(new UDPConnection(ioServicePool, serverListenPort,
                                        handler, homeAgentAlias, suffix)) {
    printf("[INFO] Initializing home agent server\n");

    /* Initialize the database driver */
    database = boost::dynamic_pointer_cast<CassandraDBDriver>(
        CassandraDBDriver::getDatabaseDriverObject());
    database->openConnection();

    this->initModelNames();

    /* Add the home agent alias to Cassandra database */
    HomeAgentIndexCassandraController haIndexController(database);
    // this->ip = this->udpConnection.getLocalEndpoint().address().to_string();
    // this->ip = this->getMyIp();
    boost::shared_ptr<HomeAgentIndex> haIndex(
        new HomeAgentIndex(this->homeAgentAlias, this->homeAgentDescription,
                           this->ip, serverListenPort));
    haIndexController.addHomeAgentIndex(haIndex);

    /* Add home agent alias to the connection end point */
    this->udpConnection->setAlias(this->homeAgentAlias);
  }

  void start() {
    /* Start the io_services */
    this->udpConnection->listen();
  }

  std::string &getHomeAgentAlias() { return this->homeAgentAlias; }

  void setHomeAgentAlias(const std::string &homeAgentAlias) {
    this->homeAgentAlias = homeAgentAlias;
  }

  boost::shared_ptr<UDPConnection> &getUDPConnection() {
    return this->udpConnection;
  }
};

#endif /* HOMEAGENTSERVER_H_ */
