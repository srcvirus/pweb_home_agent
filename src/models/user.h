/*
 * user.h
 *
 *  Created on: 2014-01-31
 *      Author: sr2chowd
 */

#ifndef USER_H_
#define USER_H_

#include <string>

using namespace std;

class User
{
	string username, password;
	string email;
	string fullname;
	string location;
	string affiliation;

public:

	static const string COL_USER_NAME, COL_PASSWORD;
	static const string COL_EMAIL;
	static const string COL_FULL_NAME;


};


#endif /* USER_H_ */
