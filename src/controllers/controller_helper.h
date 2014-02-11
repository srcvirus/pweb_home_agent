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
	void static ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}


public:
	bool static isNullOREmptyString(const string& str)
	{
		if (boost::algorithm::trim_copy(str).empty())
			return true;
		return false;
	}

	void static escapeSingleQuote(string& value){
		ReplaceStringInPlace(value, "'","''");
	}
};



#endif /* CONTROLLER_HELPER_H_ */
