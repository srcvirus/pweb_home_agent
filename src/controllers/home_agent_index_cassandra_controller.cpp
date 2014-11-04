/*
 * home_agent_index_cassandra_controller.cpp
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#include "home_agent_index_cassandra_controller.h"
#include "../global.h"
#include <sstream>

using namespace std;

boost::shared_ptr<HomeAgentIndex>
HomeAgentIndexCassandraController::getHomeAgentIndex(const string &name) {
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + HomeAgentIndex::TABLE_NAME +
                       " where name = '" + name + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_ptr<HomeAgentIndex> retObject;
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return retObject;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(DEBUG) << "Zero rows returned";
    return retObject;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << rows.row_count()
             << " rows returned";

  string haName, haIp, haDescription;
  int haPort;
  rows.next();
  rows.get_string(HomeAgentIndex::COL_NAME, haName);
  rows.get_string(HomeAgentIndex::COL_IP, haIp);
  rows.get_int(HomeAgentIndex::COL_PORT, haPort);
  rows.get_string(HomeAgentIndex::COL_DESCRIPTION, haDescription);
  retObject = boost::shared_ptr<HomeAgentIndex>(
      new HomeAgentIndex(haName, haDescription, haIp, haPort));
  retObject->printShort();
  return retObject;
}

int HomeAgentIndexCassandraController::addHomeAgentIndex(
    boost::shared_ptr<HomeAgentIndex> &haIndex) {
  std::string strPort;
  char str[10];
  unsigned long tid = (unsigned long)pthread_self();
  sprintf(str, "%d", haIndex->getPort());
  strPort = str;
  string queryString =
      "insert into " + HomeAgentIndex::TABLE_NAME + " (" +
      HomeAgentIndex::COL_NAME + "," + HomeAgentIndex::COL_IP + "," +
      HomeAgentIndex::COL_PORT + "," + HomeAgentIndex::COL_DESCRIPTION +
      ") values " + "('" + haIndex->getName() + "', '" + haIndex->getIp() +
      "'," + strPort + ",'" + haIndex->getDescription() + "');";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return results.get().error.code;
  }
  LOG(INFO) << "Successfully added " << haIndex->getName() << " to cassandra";
  return 0;
}

vector<boost::shared_ptr<HomeAgentIndex> >
HomeAgentIndexCassandraController::getAllHomeAgentIndex() {
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + HomeAgentIndex::TABLE_NAME + ";";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  vector<boost::shared_ptr<HomeAgentIndex> > homeAgents;
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return homeAgents;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(DEBUG) << "Zero rows returned";
    return homeAgents;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << rows.row_count()
             << " rows returned";

  string haName, haIp, haDescription;
  int haPort;
  while (rows.next()) {
    rows.get_string(HomeAgentIndex::COL_NAME, haName);
    rows.get_string(HomeAgentIndex::COL_IP, haIp);
    rows.get_int(HomeAgentIndex::COL_PORT, haPort);
    rows.get_string(HomeAgentIndex::COL_DESCRIPTION, haDescription);
    boost::shared_ptr<HomeAgentIndex> homeAgent =
        boost::shared_ptr<HomeAgentIndex>(
            new HomeAgentIndex(haName, haDescription, haIp, haPort));
    homeAgents.push_back(homeAgent);
    homeAgent->printShort();
  }
  return homeAgents;
}
