/*
 * device_cassandra_controller.h
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#ifndef DEVICE_CASSANDRA_CONTROLLER_H_
#define DEVICE_CASSANDRA_CONTROLLER_H_

#include <string>

#include "controller_helper.h"

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>

#include "../models/device.h"
#include "../database/cassandra_db.h"

#define SUGGESION_COUNT 5

using namespace std;

class DeviceCassandraController
{
	boost::shared_ptr <CassandraDBDriver> databaseDriver;
	
	string buildInsertStatement(boost::shared_ptr <Device>& device);
	void buildDeviceObjectFromQueryResult(cql::cql_result_t& row, boost::shared_ptr <Device>& device);
public:
	DeviceCassandraController():databaseDriver(boost::shared_ptr <CassandraDBDriver>()){;}
	DeviceCassandraController(boost::shared_ptr <CassandraDBDriver>& databaseDriver):databaseDriver(databaseDriver){;}

	string getDeviceIp(const string& deviceName, const string& userName);
	int addDevice(boost::shared_ptr<Device>& device);	
	vector < boost::shared_ptr <Device> > getUserDevices(const string& username, string& errorCode);
	boost::shared_ptr <Device> getDevice(const string& devicename, const string& username, string& errorCode);
	bool isDevicenameAvailable(const string& devicename, const string& username, string& errorCode);
	int deleteDevice(const string& devicename, const string& username);
	int updateDevice(const string& currentDevicename, const string& newDevicename, const string& username, const string& ip, const string& port, const string& public_folder, const string& private_folder);
	int updateDeviceIPPort(const string& devicename, const string& username, const string& ip, const string& port);
	int updateDeviceMetadata(const string& devicename, const string& username, const string& metadata);
	int updateDevice(const string& currentDevicename, const string& username, boost::unordered_map<string, string>& params);
	~DeviceCassandraController(){;}
};


#endif /* DEVICE_CASSANDRA_CONTROLLER_H_ */
