/*
 * database_driver.h
 *
 *  Created on: Jan 11, 2014
 *      Author: sr2chowd
 */

#ifndef DATABASE_DRIVER_H_
#define DATABASE_DRIVER_H_

#include <string>

using namespace std;

template <class QueryResultSetType>
class IDatabaseDriver
{
protected:

	string dbServerHostName;
	unsigned short int dbServerPort;
	bool connected;

	IDatabaseDriver();
	IDatabaseDriver(const string& dbServerHostName,	unsigned short int dbServerPort) :
			dbServerHostName(dbServerHostName),
			dbServerPort(dbServerPort),
			connected(false)
	{
		;
	}

	IDatabaseDriver(const char* dbServerHostName, unsigned short int dbServerPort) :
			dbServerHostName(dbServerHostName),
			dbServerPort(dbServerPort),
			connected(false)
	{
		;
	}

public:

	virtual int openConnection() = 0;
	virtual QueryResultSetType executeQuery(string queryString) = 0;
	virtual void closeConnection() = 0;

	virtual ~IDatabaseDriver(){;}

	/* Getters and setters */
	const string& getDbServerHostName() const
	{
		return dbServerHostName;
	}

	void setDbServerHostName(const string& dbServerHostName)
	{
		this->dbServerHostName = dbServerHostName;
	}

	unsigned short int getDbServerPort() const
	{
		return dbServerPort;
	}

	void setDbServerPort(unsigned short int dbServerPort)
	{
		this->dbServerPort = dbServerPort;
	}

	bool isConnected() const
	{
		return connected;
	}
};
#endif /* DATABASE_DRIVER_H_ */
