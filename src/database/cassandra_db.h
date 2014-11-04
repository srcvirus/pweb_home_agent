#ifndef _CASSANDRA_DB_H_
#define _CASSANDRA_DB_H_

#include "../global.h"
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>


#include <cql/cql.hpp>
#include <cql/cql_error.hpp>
#include <cql/cql_event.hpp>
#include <cql/cql_connection.hpp>
#include <cql/cql_connection_factory.hpp>
#include <cql/cql_session.hpp>
#include <cql/cql_cluster.hpp>
#include <cql/cql_builder.hpp>
#include <cql/cql_execute.hpp>
#include <cql/cql_result.hpp>

#include "../interfaces/database_driver.h"

using namespace std;

class CassandraDBDriver
    : public IDatabaseDriver<boost::shared_future<cql::cql_future_result_t> > {
   boost::shared_ptr<cql::cql_builder_t> cqlBuilder;
   boost::shared_ptr<cql::cql_cluster_t> cqlCluster;
   boost::shared_ptr<cql::cql_session_t> cqlSession;

protected:

  CassandraDBDriver();
  CassandraDBDriver(const string &, unsigned short int dbPort);
  CassandraDBDriver(const char *, unsigned short int dbPort);

  /* *
   * Member variables to implement Singleton pattern
   */
  static bool instanceExists;
  static boost::shared_ptr<IDatabaseDriver<
      boost::shared_future<cql::cql_future_result_t> > > databaseDriverObject;

public:

  static unsigned short int DEFAULT_DB_PORT;
  static string DEFAULT_DB_SERVER;

  /* Virtual functions inherited from the abstract class */
  virtual int openConnection();
  virtual boost::shared_future<cql::cql_future_result_t>
      executeQuery(const string &queryString);
  virtual void closeConnection();

  /**
   * The following functions make sure that CassandraDBDriver is a Singleton
   * class
   */
  static boost::shared_ptr<
      IDatabaseDriver<boost::shared_future<cql::cql_future_result_t> > >
      getDatabaseDriverObject();
  static boost::shared_ptr<
      IDatabaseDriver<boost::shared_future<cql::cql_future_result_t> > >
      getDatabaseDriverObject(const string &dbServerHostName,
                              unsigned short int dbServerPort);
  static boost::shared_ptr<
      IDatabaseDriver<boost::shared_future<cql::cql_future_result_t> > >
      getDatabaseDriverObject(const char *dbServerHostName,
                              unsigned short int dbServerPort);

  /* Cassandra specific: selects keyspace for further database operation */
  int selectKeySpace(const string &keyspace);
  virtual ~CassandraDBDriver();
};

#endif
