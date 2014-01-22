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

#include <boost/shared_ptr.hpp>

class HomeAgentIndexCassandraController
{
	boost::shared_ptr <CassandraDBDriver> databaseDriver;

public:

	HomeAgentIndexCassandraController():databaseDriver(boost::shared_ptr <CassandraDBDriver>()){;}
	HomeAgentIndexCassandraController(boost::shared_ptr <CassandraDBDriver>& databaseDriver):databaseDriver(databaseDriver){;}

	boost::shared_ptr <HomeAgentIndex> getHomeAgentIndex(const string& name);
	~HomeAgentIndexCassandraController(){;}
};



#endif /* HOME_AGENT_INDEX_CASSANDRA_CONTROLLER_H_ */
