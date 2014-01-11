/*
 * home_agent_index_cassandra_controller.h
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#ifndef HOME_AGENT_INDEX_CASSANDRA_CONTROLLER_H_
#define HOME_AGENT_INDEX_CASSANDRA_CONTROLLER_H_

#include "../database/cassandra_db.h"
#include "../models/home_agent_index.h"

class HomeAgentIndexCassandraController
{
	CassandraDBDriver* databaseDriver;

public:

	HomeAgentIndexCassandraController():databaseDriver(NULL){;}
	HomeAgentIndexCassandraController(CassandraDBDriver* databaseDriver):databaseDriver(databaseDriver){;}

	HomeAgentIndex* getHomeAgentIndex(const string& name);
};



#endif /* HOME_AGENT_INDEX_CASSANDRA_CONTROLLER_H_ */
