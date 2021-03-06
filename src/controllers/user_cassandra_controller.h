/*
 * user_cassandra_controller.h
 *
 *  Created on: Feb 7, 2014
 *      Author: mfbari
 */

#ifndef USER_CASSANDRA_CONTROLLER_H_
#define USER_CASSANDRA_CONTROLLER_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "../models/user.h"
#include "../database/cassandra_db.h"

using namespace std;

class UserCassandraController {
  boost::shared_ptr<CassandraDBDriver> databaseDriver;

public:
  UserCassandraController()
      : databaseDriver(boost::shared_ptr<CassandraDBDriver>()) {
    ;
  }
  UserCassandraController(boost::shared_ptr<CassandraDBDriver> &databaseDriver)
      : databaseDriver(databaseDriver) {
    ;
  }

  boost::shared_ptr<User> getUser(const string &username);
  vector<boost::shared_ptr<User> > getAllUser();
  int addUser(boost::shared_ptr<User> &user);
  bool isUsernameAvailable(const string &username);
  bool isUsernameAvailable(const string &username, string &errorCode);
  bool authenticateUser(const string &username, const string &password,
                        string &errorCode);
  int updateUser(const string &username,
                 boost::unordered_map<string, string> &params);
  ~UserCassandraController() { ; }
};

#endif /* USER_CASSANDRA_CONTROLLER_H_ */
