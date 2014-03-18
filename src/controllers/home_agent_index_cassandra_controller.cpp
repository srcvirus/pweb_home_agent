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
	unsigned long tid = (unsigned long)pthread_self();

	string queryString = "select * from " + HomeAgentIndex::TABLE_NAME +  " where name = '" + name + "';";

	printf("[DEBUG] [Thread 0x%lx] %s\n", tid, queryString.c_str());

	boost::shared_ptr <HomeAgentIndex> retObject;
	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);

	if(results.get().error.is_err())
	{
		printf("[ERROR] %s\n", results.get().error.message.c_str());
		return retObject;
	}

	cql::cql_result_t& rows = *(results.get().result);

	if(rows.row_count() <= 0)
	{
		printf("[DEBUG] Zero rows returned\n");
		return retObject;
	}

	printf("[DEBUG] [Thread 0x%lx] %lu rows returned\n", tid, rows.row_count());

	string haName, haIp, haDescription;
	int haPort;

	rows.next();

	rows.get_string(HomeAgentIndex::COL_NAME, haName);
	rows.get_string(HomeAgentIndex::COL_IP, haIp);
	rows.get_int(HomeAgentIndex::COL_PORT, haPort);
	rows.get_string(HomeAgentIndex::COL_DESCRIPTION, haDescription);

	retObject = boost::shared_ptr <HomeAgentIndex> (new HomeAgentIndex(haName, haDescription, haIp, haPort));
	printf("[DEBUG] [Thread 0x%lx] ", tid); retObject->printShort();
	return retObject;
}

int HomeAgentIndexCassandraController::addHomeAgentIndex(boost::shared_ptr <HomeAgentIndex>& haIndex)
{
	std::string strPort;
	char str[10];
	sprintf(str, "%d", haIndex->getPort());
	strPort = str;

	string queryString = "insert into " + HomeAgentIndex::TABLE_NAME + " (" +
								HomeAgentIndex::COL_NAME + "," +
								HomeAgentIndex::COL_IP + "," +
								HomeAgentIndex::COL_PORT + "," +
								HomeAgentIndex::COL_DESCRIPTION + ") values " + "('" +
								haIndex->getName() + "', '" + haIndex->getIp() + "'," + strPort + ",'" + haIndex->getDescription() + "');";

	printf("[DEBUG] %s\n", queryString.c_str());

	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);
	if(results.get().error.is_err())
	{
		printf("[ERROR]\t%s\n", results.get().error.message.c_str());
		return results.get().error.code;
	}
	return 0;
}

vector < boost::shared_ptr <HomeAgentIndex> > HomeAgentIndexCassandraController::getAllHomeAgentIndex()
{
	unsigned long tid = (unsigned long)pthread_self();
	string queryString = "select * from " + HomeAgentIndex::TABLE_NAME +  ";";
	printf("[DEBUG] [Thread 0x%lx] %s\n", tid, queryString.c_str());

	vector < boost::shared_ptr <HomeAgentIndex> >  homeAgents;
	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);

	if(results.get().error.is_err())
	{
		printf("[ERROR] %s\n", results.get().error.message.c_str());
		return homeAgents;
	}

	cql::cql_result_t& rows = *(results.get().result);

	if(rows.row_count() <= 0)
	{
		printf("[DEBUG] Zero rows returned\n");
		return homeAgents;
	}

	printf("[DEBUG] [Thread 0x%lx] %lu rows returned\n", tid, rows.row_count());

	string haName, haIp, haDescription;
	int haPort;

	while(rows.next())
	{
		rows.get_string(HomeAgentIndex::COL_NAME, haName);
		rows.get_string(HomeAgentIndex::COL_IP, haIp);
		rows.get_int(HomeAgentIndex::COL_PORT, haPort);
		rows.get_string(HomeAgentIndex::COL_DESCRIPTION, haDescription);
		boost::shared_ptr <HomeAgentIndex> homeAgent = boost::shared_ptr <HomeAgentIndex> (new HomeAgentIndex(haName, haDescription, haIp, haPort));
		homeAgents.push_back(homeAgent);
		printf("[DEBUG] [Thread 0x%lx] ", tid); homeAgent->printShort();
	}
	return homeAgents;
}

