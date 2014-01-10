#ifndef _CASSANDRA_DB_H_
#define _CASSANDRA_DB_H_

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

using namespace std;

class CassandraDBDriver
{
	string		dbServer;
	int			dbPort;
	bool		isConnected;

	boost::shared_ptr <cql::cql_builder_t>	cqlBuilder;
	boost::shared_ptr <cql::cql_cluster_t>	cqlCluster;
	boost::shared_ptr <cql::cql_session_t>	cqlSession;

public:

	static int		DEFAULT_DB_PORT;
	static string	DEFAULT_DB_SERVER;

	CassandraDBDriver();
	CassandraDBDriver(const string&, int dbPort);
	CassandraDBDriver(const char *, int dbPort);

	int												openConnection();
	int												selectKeySpace(string keyspace);
	boost::shared_future <cql::cql_future_result_t>	executeQuery(string queryString);

	void	closeConnection();

	/* getters and setters */

	const string& getDbServer() const {
		return dbServer;
	}

	void setDbServer(const string& dbServer) {
		this->dbServer = dbServer;
	}


	~CassandraDBDriver();
};

#endif
