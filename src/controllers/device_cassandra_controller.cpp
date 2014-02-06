/*
 * device_cassandra_controller.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#include "device_cassandra_controller.h"

string DeviceCassandraController::getDeviceIp(const string& deviceName, const string& userName)
{
	string retIp = "";
	bool searchable;

	unsigned long tid = (unsigned long)pthread_self();
	string queryString = "select " + Device::COL_IP + "," + Device::COL_SEARCHABLE + " from device where " + Device::COL_DEVICE_NAME + "='" + deviceName + "' and " + Device::COL_USER_NAME + "='" + userName + "';";
	printf("[DEBUG] [Thread 0x%lx] %s\n", tid, queryString.c_str());

	boost::shared_future <cql::cql_future_result_t> results = databaseDriver->executeQuery(queryString);

	if(results.get().error.is_err())
	{
		printf("[ERROR] %s\n", results.get().error.message.c_str());
		return retIp;
	}

	cql::cql_result_t& rows = *(results.get().result);

	if(rows.row_count() <= 0)
	{
		printf("[DEBUG] Zero rows returned\n");
		return retIp;
	}

	printf("[DEBUG] [Thread 0x%lx] %lu rows returned\n", tid, rows.row_count());

	rows.next();
	rows.get_bool(Device::COL_SEARCHABLE, searchable);
	if(searchable) rows.get_string(Device::COL_IP, retIp);

	return retIp;
}
