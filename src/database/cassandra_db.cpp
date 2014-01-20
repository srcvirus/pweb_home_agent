#include "cassandra_db.h"

short unsigned int	CassandraDBDriver::DEFAULT_DB_PORT 	= 9042;
string CassandraDBDriver::DEFAULT_DB_SERVER = "127.0.0.1";
bool CassandraDBDriver::instanceExists = false;

boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > > CassandraDBDriver::databaseDriverObject;

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
	pthread_mutex_lock(&this->dbLock);
	cqlBuilder = cql::cql_cluster_t::builder();
	cqlBuilder->add_contact_point(boost::asio::ip::address::from_string(this->dbServerHostName), (unsigned short int)this->dbServerPort);
	cqlCluster = cqlBuilder->build();
	cqlSession = cqlCluster->connect();
	pthread_mutex_unlock(&this->dbLock);

	if(cqlSession)
	{
		connected = true;
		return 0;
	}
	return -1;
}

int CassandraDBDriver::selectKeySpace(const string& keyspace)
{
	string queryString = "use " + keyspace + ";";
	boost::shared_future <cql::cql_future_result_t> cqlResult = this->executeQuery(queryString);
	if(cqlResult.get().error.is_err())
		return -1;
	return 0;
}


boost::shared_future <cql::cql_future_result_t>
CassandraDBDriver::executeQuery(const string& queryString)
{
	pthread_mutex_lock(&this->dbLock);
	if(!connected)
		this->openConnection();
	boost::shared_ptr <cql::cql_query_t> cqlQuery( new cql::cql_query_t(queryString, cql::CQL_CONSISTENCY_ALL) );
	boost::shared_future <cql::cql_future_result_t> cqlResult = this->cqlSession->query(cqlQuery);
	cqlResult.wait();
	pthread_mutex_unlock(&this->dbLock);

	return cqlResult;
}

void CassandraDBDriver::closeConnection()
{
	if(this->cqlSession) this->cqlSession->close();
	if(this->cqlCluster) this->cqlCluster->shutdown();
	this->connected = false;
}

CassandraDBDriver::~CassandraDBDriver()
{
	this->closeConnection();
	CassandraDBDriver::instanceExists = false;
	pthread_mutex_destroy(&this->dbLock);
}

boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > >
CassandraDBDriver::getDatabaseDriverObject()
{
	if(!CassandraDBDriver::instanceExists)
	{
		CassandraDBDriver::databaseDriverObject =
				boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > >(new CassandraDBDriver());
		CassandraDBDriver::instanceExists = false;
	}
	return CassandraDBDriver::databaseDriverObject;
}

boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > >
CassandraDBDriver::getDatabaseDriverObject(const string& dbServerHostName, unsigned short int dbServerPort)
{
	if(!CassandraDBDriver::instanceExists)
	{
		CassandraDBDriver::databaseDriverObject =
				boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > >(new CassandraDBDriver(dbServerHostName, dbServerPort));
		CassandraDBDriver::instanceExists = false;
	}
	return CassandraDBDriver::databaseDriverObject;
}

boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > >
CassandraDBDriver::getDatabaseDriverObject(const char* dbServerHostName, unsigned short int dbServerPort)
{
	if(!CassandraDBDriver::instanceExists)
	{
		CassandraDBDriver::databaseDriverObject =
				boost::shared_ptr < IDatabaseDriver < boost::shared_future <cql::cql_future_result_t> > >(new CassandraDBDriver(dbServerHostName, dbServerPort));
		CassandraDBDriver::instanceExists = false;
	}
	return CassandraDBDriver::databaseDriverObject;
}
