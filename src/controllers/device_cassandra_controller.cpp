/*
 * device_cassandra_controller.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#include "device_cassandra_controller.h"
#include "../global.h"

string DeviceCassandraController::buildInsertStatement(
    boost::shared_ptr<Device> &device) {
  string queryString =
      "insert into " + Device::TABLE_NAME + " (" + Device::COL_USER_NAME +
      ", " + Device::COL_DEVICE_NAME + ", " + Device::COL_TYPE + ", " +
      Device::COL_IP + ", " + Device::COL_DIR_IP + ", " + Device::COL_PORT +
      ", " + Device::COL_DIR_PORT + ", " + Device::COL_PUBLIC_FOLDER + ", " +
      Device::COL_PRIVATE_FOLDER + ", " + Device::COL_LAST_SEEN + ", " +
      Device::COL_OS + ", " + Device::COL_DESCRIPTION + ", " +
      Device::COL_CONTENT_META + ", " + Device::COL_SEARCHABLE + ", " +
      Device::COL_CONTENT_TIME_STAMP + ") VALUES ('" + device->getUsername() +
      "', '" + device->getDevicename() + "', '" + device->getType() + "', '" +
      device->getIp() + "', '" + device->getDirIp() + "', '" +
      boost::lexical_cast<string>(device->getPort()) + "', '" +
      boost::lexical_cast<string>(device->getDirPort()) + "', '" +
      device->getPublicFolder() + "', '" + device->getPrivateFolder() + "', " +
      boost::lexical_cast<string>(device->getLastSeen()) + ", '" +
      device->getOs() + "', '" + device->getDescription() + "', '" +
      device->getContentMeta() + "', " +
      ((device->isSearchable()) ? "true" : "false") + ", " +
      boost::lexical_cast<string>(device->getContentTimestamp()) + ");";

  return queryString;
}

void DeviceCassandraController::buildDeviceObjectFromQueryResult(
    cql::cql_result_t &row, boost::shared_ptr<Device> &device) {
  string dname, uname, type, ip, dirIp, publicFolder, privateFolder, os,
      description, contentMeta;
  unsigned short port, dirPort;
  string strPort, strDirPort;
  int lastSeen, contentTimestamp;
  bool searchable;

  row.get_string(Device::COL_USER_NAME, uname);
  row.get_string(Device::COL_DEVICE_NAME, dname);
  row.get_string(Device::COL_TYPE, type);
  row.get_string(Device::COL_IP, ip);
  row.get_string(Device::COL_DIR_IP, dirIp);
  row.get_string(Device::COL_PORT, strPort);
  row.get_string(Device::COL_DIR_PORT, strDirPort);
  row.get_string(Device::COL_PUBLIC_FOLDER, publicFolder);
  row.get_string(Device::COL_PRIVATE_FOLDER, privateFolder);
  row.get_int(Device::COL_LAST_SEEN, lastSeen);
  row.get_string(Device::COL_OS, os);
  row.get_string(Device::COL_DESCRIPTION, description);
  row.get_string(Device::COL_CONTENT_META, contentMeta);
  row.get_bool(Device::COL_SEARCHABLE, searchable);
  row.get_int(Device::COL_CONTENT_TIME_STAMP, contentTimestamp);

  if (ControllerHelper::isNullOREmptyString(strPort))
    port = -1;
  else
    port = boost::lexical_cast<unsigned short>(strPort);
  if (ControllerHelper::isNullOREmptyString(strDirPort))
    dirPort = -1;
  else
    dirPort = boost::lexical_cast<unsigned short>(strDirPort);

  device = boost::shared_ptr<Device>(new Device(
      uname, dname, type, ip, port, dirIp, dirPort, publicFolder, privateFolder,
      lastSeen, os, description, contentMeta, searchable, contentTimestamp));
}

string DeviceCassandraController::getDeviceIp(const string &deviceName,
                                              const string &userName) {
  string retIp = "";
  bool searchable;

  unsigned long tid = (unsigned long)pthread_self();
  string queryString =
      "select " + Device::COL_IP + "," + Device::COL_SEARCHABLE + " from " +
      Device::TABLE_NAME + " where " + Device::COL_DEVICE_NAME + "='" +
      deviceName + "' and " + Device::COL_USER_NAME + "='" + userName + "';";
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << "[Thread " << std::hex << tid << "] "
               << results.get().error.message.c_str();
    return retIp;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    return retIp;
  }
  rows.next();
  rows.get_bool(Device::COL_SEARCHABLE, searchable);
  if (searchable)
    rows.get_string(Device::COL_IP, retIp);
  return retIp;
}

/**
 * Adds a new Device
 * @param device An intance of class Device
 * @return 0 on success, otherwise the database error code
 */
int DeviceCassandraController::addDevice(boost::shared_ptr<Device> &device) {
  string queryString = buildInsertStatement(device);
  LOG(DEBUG) << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return results.get().error.code;
  }
  return 0;
}

boost::shared_ptr<Device> DeviceCassandraController::getDevice(
    const string &devicename, const string &username, string &errorCode) {
  unsigned long tid = (unsigned long)pthread_self();

  string queryString = "select * from " + Device::TABLE_NAME + " where " +
                       Device::COL_USER_NAME + " = '" + username + "' and " +
                       Device::COL_DEVICE_NAME + " = '" + devicename + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_ptr<Device> device;
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    errorCode = boost::lexical_cast<string>(results.get().error.code);
    return device;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(WARN) << "Zero rows returned";
    errorCode = "-1";
    return device;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << rows.row_count()
             << "rows returned.";
  rows.next();
  buildDeviceObjectFromQueryResult(rows, device);
  return device;
}

vector<boost::shared_ptr<Device> >
DeviceCassandraController::getUserDevices(const string &username,
                                          string &errorCode) {
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + Device::TABLE_NAME + " where " +
                       Device::COL_USER_NAME + " = '" + username + "';";

  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  vector<boost::shared_ptr<Device> > deviceVector;
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    errorCode = boost::lexical_cast<string>(results.get().error.code);
    return deviceVector;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(DEBUG) << "Zero rows returned";
    return deviceVector;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << rows.row_count()
             << "rows returned.";
  while (rows.next()) {
    boost::shared_ptr<Device> device;
    buildDeviceObjectFromQueryResult(rows, device);
    deviceVector.push_back(device);
  }
  return deviceVector;
}

bool DeviceCassandraController::isDevicenameAvailable(const string &devicename,
                                                      const string &username,
                                                      string &errorCode) {
  errorCode = "";
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + Device::TABLE_NAME + " where " +
                       Device::COL_USER_NAME + " = '" + username + "' and " +
                       Device::COL_DEVICE_NAME + " = '" + devicename + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    errorCode = boost::lexical_cast<string>(results.get().error.code);
    return false;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    return true;
  }
  return false;
}

int DeviceCassandraController::deleteDevice(const string &devicename,
                                            const string &username) {
  string errorCode = "";
  bool isAvailable = isDevicenameAvailable(devicename, username, errorCode);
  if (!errorCode.empty()) {
    return boost::lexical_cast<int>(errorCode);
  }
  if (isAvailable) {
    return -1;
  }
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "delete from " + Device::TABLE_NAME + " where " +
                       Device::COL_USER_NAME + " = '" + username + "' and " +
                       Device::COL_DEVICE_NAME + " = '" + devicename + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return results.get().error.code;
  }
  return 0;
}

int DeviceCassandraController::updateDeviceIPPort(const string &devicename,
                                                  const string &username,
                                                  const string &ip,
                                                  const string &port) {
  string errorCode = "";
  bool isAvailable = isDevicenameAvailable(devicename, username, errorCode);
  if (!errorCode.empty()) {
    return boost::lexical_cast<int>(errorCode);
  }
  if (isAvailable) {
    return -1;
  }
  unsigned long tid = (unsigned long)pthread_self();
  string queryString =
      "update " + Device::TABLE_NAME + " set " + Device::COL_IP + " = '" + ip +
      "', " + Device::COL_PORT + " = '" + port + "' where " +
      Device::COL_USER_NAME + " = '" + username + "' and " +
      Device::COL_DEVICE_NAME + " = '" + devicename + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return results.get().error.code;
  }

  return 0;
}

int DeviceCassandraController::updateDeviceMetadata(const string &devicename,
                                                    const string &username,
                                                    const string &metadata) {
  string errorCode = "";
  bool isAvailable = isDevicenameAvailable(devicename, username, errorCode);
  if (!errorCode.empty()) {
    return boost::lexical_cast<int>(errorCode);
  }
  if (isAvailable) {
    return -1;
  }

  unsigned long tid = (unsigned long)pthread_self();
  string queryString =
      "update " + Device::TABLE_NAME + " set " + Device::COL_CONTENT_META +
      " = '" + metadata + "' where " + Device::COL_USER_NAME + " = '" +
      username + "' and " + Device::COL_DEVICE_NAME + " = '" + devicename +
      "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] " << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return results.get().error.code;
  }
  return 0;
}

/**
 * Update devicename, ip, port, public folder, and private folder
 * @param currentDevicename
 * @param newDevicename
 * @param username
 * @param ip
 * @param port
 * @param pulicFolder
 * @param privateFolder
 * @return 0 on success, -1 when currentDevice does not exist, otherwise DB
 * error code
 */
int DeviceCassandraController::updateDevice(
    const string &currentDevicename, const string &newDevicename,
    const string &username, const string &ip, const string &port,
    const string &publicFolder, const string &privateFolder) {
  string errorCode = "";
  int result;
  // Get the current device.
  boost::shared_ptr<Device> device =
      getDevice(currentDevicename, username, errorCode);
  if (!errorCode.empty()) {
    return boost::lexical_cast<int>(errorCode);
  }

  // Delete the current device from db.
  result = deleteDevice(currentDevicename, username);
  if (result != 0) {
    return result;
  }

  // Update data in the current device.
  device->setDevicename(newDevicename);
  device->setIp(ip);
  unsigned short portValue = -1;
  if (!ControllerHelper::isNullOREmptyString(port))
    portValue = boost::lexical_cast<unsigned short>(port);
  device->setPort(portValue);
  device->setPublicFolder(publicFolder);
  device->setPrivateFolder(privateFolder);

  // Add the new device.
  result = addDevice(device);
  return result;
}

int DeviceCassandraController::updateDevice(
    const string &devicename, const string &username,
    boost::unordered_map<string, string> &params) {
  if (params.size() <= 0)
    return 0;

  string errorCode = "";
  int result;

  // Get the current device.
  boost::shared_ptr<Device> device = getDevice(devicename, username, errorCode);
  if (!errorCode.empty()) {
    return boost::lexical_cast<int>(errorCode);
  }

  // Check if devicename needs to be changed.
  if (params.count("new_devicename")) {
    // Delete the current device from db.
    result = deleteDevice(devicename, username);
    if (result != 0) {
      return result;
    }
  }

  // Update data in the current device.
  if (params.count("new_devicename")) {
    device->setDevicename(params["new_devicename"]);
  }
  if (params.count("type")) {
    device->setType(params["type"]);
  }
  if (params.count("metadata")) {
    device->setContentMeta(params["metadata"]);
    device->setContentTimestamp(time(NULL));
  }
  if (params.count("dir_ip")) {
    device->setDirIp(params["dir_ip"]);
  }
  if (params.count("dir_port")) {
    unsigned short portValue = -1;
    if (!ControllerHelper::isNullOREmptyString(params["dir_port"]))
      portValue = boost::lexical_cast<unsigned short>(params["dir_port"]);
    device->setDirPort(portValue);
  }
  if (params.count("ip")) {
    device->setIp(params["ip"]);
  }
  if (params.count("port")) {
    unsigned short portValue = -1;
    if (!ControllerHelper::isNullOREmptyString(params["port"]))
      portValue = boost::lexical_cast<unsigned short>(params["port"]);
    device->setPort(portValue);
  }
  if (params.count("os")) {
    device->setOs(params["os"]);
  }
  if (params.count("description")) {
    device->setDescription(params["description"]);
  }
  if (params.count("public_folder")) {
    device->setPublicFolder(params["public_folder"]);
  }
  if (params.count("private_folder")) {
    device->setPrivateFolder(params["private_folder"]);
  }
  if (params.count("is_indexed")) {
    bool is_indexed = false;
    if (boost::iequals(params["is_indexed"], "true"))
      is_indexed = true;
    device->setSearchable(is_indexed);
  }

  // Update lastseen.
  device->setLastSeen(time(NULL));

  // Add the new device.
  result = addDevice(device);
  return result;
}
