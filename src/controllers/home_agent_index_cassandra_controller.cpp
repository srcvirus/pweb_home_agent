/*
 * home_agent_index_cassandra_controller.cpp
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#include "home_agent_index_cassandra_controller.h"

HomeAgentIndex* HomeAgentIndexCassandraController::getHomeAgentIndex(const string& name)
{
	HomeAgentIndex* retObject;
	string queryString = "select * from home_agent_index where name = '" + name + "';";

	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);

	cql::cql_result_t& rows = *(results.get().result); //databaseDriver->executeQuery(queryString);


	string haName, haIp;
	int haPort;

	rows.next();

	rows.get_string(HomeAgentIndex::COL_NAME, haName);
	rows.get_string(HomeAgentIndex::COL_IP, haIp);
	rows.get_int(HomeAgentIndex::COL_PORT, haPort);

	retObject = new HomeAgentIndex(haName, haIp, haPort);
	return retObject;
}



