/*
 * home_agent_index_cassandra_controller.cpp
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#include "home_agent_index_cassandra_controller.h"
#include <sstream>

using namespace std;

boost::shared_ptr <HomeAgentIndex> HomeAgentIndexCassandraController::getHomeAgentIndex(const string& name)
{
	string queryString = "select * from " + HomeAgentIndex::TABLE_NAME +  " where name = '" + name + "';";
	boost::shared_ptr <HomeAgentIndex> retObject;

	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);

	if(results.get().error.is_err())
	{
		printf("[ERROR]\t%s\n", results.get().error.message.c_str());
		return retObject;
	}

	cql::cql_result_t& rows = *(results.get().result); //databaseDriver->executeQuery(queryString);

	if(rows.row_count() <= 0)
		return retObject;

	string haName, haIp;
	int haPort;

	rows.next();

	rows.get_string(HomeAgentIndex::COL_NAME, haName);
	rows.get_string(HomeAgentIndex::COL_IP, haIp);
	rows.get_int(HomeAgentIndex::COL_PORT, haPort);

	retObject = boost::shared_ptr <HomeAgentIndex> (new HomeAgentIndex(haName, haIp, haPort));
	return retObject;
}

int HomeAgentIndexCassandraController::addHomeAgentIndex(boost::shared_ptr <HomeAgentIndex>& haIndex)
{
	std::string strPort;
	std::ostringstream ss(strPort);
	ss << haIndex->getPort();

	string queryString = "insert into " + HomeAgentIndex::TABLE_NAME + " (" +
								HomeAgentIndex::COL_NAME + "," +
								HomeAgentIndex::COL_IP + "," +
								HomeAgentIndex::COL_PORT + ") values " + "(" +
								haIndex->getName() + ", " + haIndex->getIp() + "," + strPort + ");";

	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);
	if(results.get().error.is_err())
	{
		printf("[ERROR]\t%s\n", results.get().error.message.c_str());
		results.get().error.code;
	}
	return 0;
}
