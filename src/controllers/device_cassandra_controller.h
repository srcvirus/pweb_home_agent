/*
 * device_cassandra_controller.h
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#ifndef DEVICE_CASSANDRA_CONTROLLER_H_
#define DEVICE_CASSANDRA_CONTROLLER_H_

#include <string>

#include <boost/shared_ptr.hpp>
#include "../models/device.h"
#include "../database/cassandra_db.h"

using namespace std;

class DeviceCassandraController
{
	boost::shared_ptr <CassandraDBDriver> databaseDriver;

public:
	DeviceCassandraController():databaseDriver(boost::shared_ptr <CassandraDBDriver>()){;}
	DeviceCassandraController(boost::shared_ptr <CassandraDBDriver>& databaseDriver):databaseDriver(databaseDriver){;}

	string getDeviceIp(const string& deviceName, const string& userName);
	~DeviceCassandraController(){;}
};


#endif /* DEVICE_CASSANDRA_CONTROLLER_H_ */
