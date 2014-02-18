/*
 * cassandra_helper.h
 *
 *  Created on: Feb 10, 2014
 *      Author: mfbari
 */

#ifndef CONTROLLER_HELPER_H_
#define CONTROLLER_HELPER_H_

#include <boost/algorithm/string.hpp>

using namespace std;

class ControllerHelper
{

public:
	bool static isNullOREmptyString(const string& str)
	{
		if (boost::algorithm::trim_copy(str).empty())
			return true;
		return false;
	}
};



#endif /* CONTROLLER_HELPER_H_ */
