/*
 * dns_question.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef DNS_QUESTION_H_
#define DNS_QUESTION_H_

#include "typedefs.h"

class DNSQuestion
{
	std::string name;
	QueryType qType;
	QueryClass qClass;

public:
	const std::string& getName() const
	{
		return name;
	}

	void setName(const std::string& name)
	{
		this->name = name;
	}

	QueryClass getClass() const
	{
		return qClass;
	}

	void setClass(QueryClass clazz)
	{
		qClass = clazz;
	}

	QueryType getType() const
	{
		return qType;
	}

	void setType(QueryType type)
	{
		qType = type;
	}
};



#endif /* DNS_QUESTION_H_ */
