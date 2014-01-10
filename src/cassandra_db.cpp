#include "cassandra_db.h"

int		CassandraDBDriver::DEFAULT_DB_PORT = 9042;
string	CassandraDBDriver::DEFAULT_DB_SERVER = "127.0.0.1";

CassandraDBDriver::CassandraDBDriver():	dbServer(CassandraDBDriver::DEFAULT_DB_SERVER),
							dbPort(CassandraDBDriver::DEFAULT_DB_PORT),
							isConnected(false)
{
	;
}

CassandraDBDriver::CassandraDBDriver(const string& host, int dbPort = CassandraDBDriver::DEFAULT_DB_PORT): dbServer(host),
																						 dbPort(dbPort),
																						 isConnected(false)
{
	;
}

CassandraDBDriver::CassandraDBDriver(const char* host, int dbPort = CassandraDBDriver::DEFAULT_DB_PORT): dbServer(host),
																					   dbPort(dbPort),
																					   isConnected(false)
{
	;
}

int CassandraDBDriver::openConnection()
{
	cqlBuilder = cql::cql_cluster_t::builder();
	cqlBuilder->add_contact_point(boost::asio::ip::address::from_string(this->dbServer), (unsigned short int)dbPort);
	cqlCluster = cqlBuilder->build();
	cqlSession = cqlCluster->connect();

	if(cqlSession)
	{
		isConnected = true;
		return 0;
	}
	return -1;
}

int CassandraDBDriver::selectKeySpace(string keyspace)
{
	string queryString = "use " + keyspace + ";";
	boost::shared_future <cql::cql_future_result_t> cqlResult = this->executeQuery(queryString);
	if(cqlResult.get().error.is_err())
		return -1;
	return 0;
}

boost::shared_future <cql::cql_future_result_t>
CassandraDBDriver::executeQuery(string queryString)
{
	if(!this->isConnected)
		this->openConnection();

	boost::shared_ptr <cql::cql_query_t> cqlQuery( new cql::cql_query_t(queryString, cql::CQL_CONSISTENCY_ONE) );
	boost::shared_future <cql::cql_future_result_t> cqlResult = this->cqlSession->query(cqlQuery);
	cqlResult.wait();
	return cqlResult;
}

void CassandraDBDriver::closeConnection()
{
	this->cqlSession->close();
	this->cqlCluster->shutdown();
	this->isConnected = false;
}

CassandraDBDriver::~CassandraDBDriver()
{
	this->closeConnection();
}
