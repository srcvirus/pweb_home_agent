/*
 * user.h
 *
 *  Created on: 2014-01-31
 *      Author: sr2chowd
 */

#ifndef USER_H_
#define USER_H_

#include <string>
#include <cstdio>

using namespace std;

class User {
  string username, password;
  string email;
  string fullname;
  string location;
  string affiliation;

public:

  static const string COL_USER_NAME, COL_PASSWORD;
  static const string COL_EMAIL;
  static const string COL_FULL_NAME;
  static const string COL_LOCATION;
  static const string COL_AFFILIATION;
  static string TABLE_NAME;

  User(const string &username, const string &password, const string &email,
       const string &fullname, const string &location,
       const string &affiliation)
      : username(username), password(password), email(email),
        fullname(fullname), location(location), affiliation(affiliation) {
    ;
  }

  string &getAffiliation() { return affiliation; }

  void setAffiliation(const string &affiliation) {
    this->affiliation = affiliation;
  }

  string &getEmail() { return email; }

  void setEmail(const string &email) { this->email = email; }

  string &getFullname() { return fullname; }

  void setFullname(const string &fullname) { this->fullname = fullname; }

  string &getLocation() { return location; }

  void setLocation(const string &location) { this->location = location; }

  string &getPassword() { return password; }

  void setPassword(const string &password) { this->password = password; }

  string &getUsername() { return username; }

  void setUsername(const string &username) { this->username = username; }

  void print() {
    printf("{\n");
    printf("\t%s : %s\n", User::COL_USER_NAME.c_str(), this->username.c_str());
    printf("\t%s : %s\n", User::COL_EMAIL.c_str(), this->email.c_str());
    printf("\t%s : %s\n", User::COL_FULL_NAME.c_str(), this->fullname.c_str());
    printf("\t%s : %s\n", User::COL_LOCATION.c_str(), this->location.c_str());
    printf("\t%s : %s\n", User::COL_AFFILIATION.c_str(),
           this->affiliation.c_str());
    printf("}\n");
  }

  void printShort() {
    printf("{%s : %s, %s : %s, %s : %s, %s : %s, %s : %s}\n",
           User::COL_USER_NAME.c_str(), this->username.c_str(),
           User::COL_EMAIL.c_str(), this->email.c_str(),
           User::COL_FULL_NAME.c_str(), this->fullname.c_str(),
           User::COL_LOCATION.c_str(), this->location.c_str(),
           User::COL_AFFILIATION.c_str(), this->affiliation.c_str());
  }
};

#endif /* USER_H_ */
