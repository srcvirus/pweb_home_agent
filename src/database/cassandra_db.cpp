#include "cassandra_db.h"

short unsigned int	CassandraDBDriver::DEFAULT_DB_PORT 		= 9042;
string				CassandraDBDriver::DEFAULT_DB_SERVER 	= "127.0.0.1";
bool 				CassandraDBDriver::instanceExists 		= false;

IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> >*	CassandraDBDriver::databaseDriverObject = NULL;

CassandraDBDriver::CassandraDBDriver():
		IDatabaseDriver(CassandraDBDriver::DEFAULT_DB_SERVER, CassandraDBDriver::DEFAULT_DB_PORT)
{
	;
}

CassandraDBDriver::CassandraDBDriver(const string& dbServerHost, unsigned short int dbServerPort = CassandraDBDriver::DEFAULT_DB_PORT):
		IDatabaseDriver(dbServerHost, dbServerPort)
{
	;
}

CassandraDBDriver::CassandraDBDriver(const char* dbServerHost, unsigned short int dbServerPort = CassandraDBDriver::DEFAULT_DB_PORT):
		IDatabaseDriver(dbServerHost, dbServerPort)
{
	;
}

int CassandraDBDriver::openConnection()
{
	cqlBuilder = cql::cql_cluster_t::builder();
	cqlBuilder->add_contact_point(boost::asio::ip::address::from_string(this->dbServerHostName), (unsigned short int)this->dbServerPort);
	cqlCluster = cqlBuilder->build();
	cqlSession = cqlCluster->connect();

	if(cqlSession)
	{
		connected = true;
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

//template <class T>
boost::shared_future <cql::cql_future_result_t>
CassandraDBDriver::executeQuery(string queryString)
{
	if(!this->connected)
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
	this->connected = false;
}

CassandraDBDriver::~CassandraDBDriver()
{
	this->closeConnection();
	CassandraDBDriver::instanceExists = false;
}

IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> >* CassandraDBDriver::getDatabaseDriverObject()
{
	if(!CassandraDBDriver::instanceExists)
	{
		CassandraDBDriver::databaseDriverObject = new CassandraDBDriver();
		CassandraDBDriver::instanceExists = false;
	}
	return CassandraDBDriver::databaseDriverObject;
}

IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> >* CassandraDBDriver::getDatabaseDriverObject(const string& dbServerHostName, unsigned short int dbServerPort)
{
	if(!CassandraDBDriver::instanceExists)
	{
		CassandraDBDriver::databaseDriverObject = new CassandraDBDriver(dbServerHostName, dbServerPort);
		CassandraDBDriver::instanceExists = false;
	}
	return CassandraDBDriver::databaseDriverObject;
}

IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> >* CassandraDBDriver::getDatabaseDriverObject(const char* dbServerHostName, unsigned short int dbServerPort)
{
	if(!CassandraDBDriver::instanceExists)
	{
		CassandraDBDriver::databaseDriverObject = new CassandraDBDriver(dbServerHostName, dbServerPort);
		CassandraDBDriver::instanceExists = false;
	}
	return CassandraDBDriver::databaseDriverObject;
}
