/*
 * user_cassandra_controller.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: mfbari
 */

#include "user_cassandra_controller.h"
#include "../global.h"

/**
 * Get user by username.
 * @param username The username of the User you are looking for
 * @return A boost::shared_ptr <User> object on success, otherwise null
 */
boost::shared_ptr<User>
UserCassandraController::getUser(const string &username) {
  unsigned long tid = static_cast<unsigned long>(pthread_self());
  string queryString = "select * from " + User::TABLE_NAME + " where " +
                       User::COL_USER_NAME + " = '" + username + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << queryString.c_str();
  boost::shared_ptr<User> retObject;
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return retObject;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(WARN) << "Zero rows returned.";
    return retObject;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << rows.row_count() << " rows returned.";

  string uname, password, email, fullname, location, affiliation;
  rows.next();
  rows.get_string(User::COL_USER_NAME, uname);
  rows.get_string(User::COL_PASSWORD, password);
  rows.get_string(User::COL_EMAIL, email);
  rows.get_string(User::COL_FULL_NAME, fullname);
  rows.get_string(User::COL_LOCATION, location);
  rows.get_string(User::COL_AFFILIATION, affiliation);
  retObject = boost::shared_ptr<User>(
      new User(uname, password, email, fullname, location, affiliation));
  retObject->printShort();
  return retObject;
}

/**
 * Add new User
 * @param user An intance of class User
 * @return 0 on success, otherwise the database error code
 */
int UserCassandraController::addUser(boost::shared_ptr<User> &user) {
  unsigned long tid = static_cast<unsigned long>(pthread_self());
  string queryString =
      "insert into " + User::TABLE_NAME + " (" + User::COL_USER_NAME + ", " +
      User::COL_PASSWORD + ", " + User::COL_EMAIL + ", " + User::COL_FULL_NAME +
      ", " + User::COL_LOCATION + ", " + User::COL_AFFILIATION + ") VALUES ('" +
      user->getUsername() + "', '" + user->getPassword() + "', '" +
      user->getEmail() + "', '" + user->getFullname() + "', '" +
      user->getLocation() + "', '" + user->getAffiliation() + "');";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return results.get().error.code;
  }
  return 0;
}

vector<boost::shared_ptr<User> > UserCassandraController::getAllUser() {
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + User::TABLE_NAME + ";";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << queryString.c_str();
  vector<boost::shared_ptr<User> > userVector;
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    return userVector;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(WARN) << "Zero rows returned.";
    return userVector;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << rows.row_count() << " rows returned.";

  string uname, password, email, fullname, location, affiliation;
  while (rows.next()) {
    rows.get_string(User::COL_USER_NAME, uname);
    rows.get_string(User::COL_PASSWORD, password);
    rows.get_string(User::COL_EMAIL, email);
    rows.get_string(User::COL_FULL_NAME, fullname);
    rows.get_string(User::COL_LOCATION, location);
    rows.get_string(User::COL_AFFILIATION, affiliation);
    boost::shared_ptr<User> user = boost::shared_ptr<User>(
        new User(uname, password, email, fullname, location, affiliation));
    userVector.push_back(user);
  }
  return userVector;
}

bool UserCassandraController::isUsernameAvailable(const string &username) {
  string errorCode = "";
  return isUsernameAvailable(username, errorCode);
}

bool UserCassandraController::isUsernameAvailable(const string &username,
                                                  string &errorCode) {
  errorCode = "";
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + User::TABLE_NAME + " where " +
                       User::COL_USER_NAME + " = '" + username + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << queryString.c_str();
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

bool UserCassandraController::authenticateUser(const string &username,
                                               const string &password,
                                               string &errorCode) {
  errorCode = "";
  unsigned long tid = (unsigned long)pthread_self();
  string queryString = "select * from " + User::TABLE_NAME + " where " +
                       User::COL_USER_NAME + " = '" + username + "' and " +
                       User::COL_PASSWORD + " ='" + password + "';";
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << queryString.c_str();
  boost::shared_future<cql::cql_future_result_t> results =
      databaseDriver->executeQuery(queryString);
  if (results.get().error.is_err()) {
    LOG(ERROR) << results.get().error.message.c_str();
    errorCode = boost::lexical_cast<string>(results.get().error.code);
    return false;
  }
  cql::cql_result_t &rows = *(results.get().result);
  if (rows.row_count() <= 0) {
    LOG(WARN) << "Zero rows returned.";
    return false;
  }
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << rows.row_count() << " rows returned.";
  return true;
}

int UserCassandraController::updateUser(
    const string &username, boost::unordered_map<string, string> &params) {
  if (params.size() <= 0)
    return 0;
  string errorCode = "";
  int result;

  // Get the current user.
  boost::shared_ptr<User> user = getUser(username);
  if (!errorCode.empty()) {
    return boost::lexical_cast<int>(errorCode);
  }

  // Update data in the current user.
  if (params.count("new_password")) {
    user->setPassword(params["new_password"]);
  }
  if (params.count("email")) {
    user->setEmail(params["email"]);
  }
  if (params.count("fullname")) {
    user->setFullname(params["fullname"]);
  }
  if (params.count("location")) {
    user->setLocation(params["location"]);
  }
  if (params.count("affiliation")) {
    user->setAffiliation(params["affiliation"]);
  }

  // Add the new user.
  result = addUser(user);

  return result;
}
