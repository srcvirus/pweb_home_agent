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
	std::string& getName()
	{
		return name;
	}

	void setName(const std::string& name)
	{
		this->name = name;
	}

	QueryClass getClass()
	{
		return qClass;
	}

	void setClass(QueryClass clazz)
	{
		qClass = clazz;
	}

	QueryType getType()
	{
		return qType;
	}

	void setType(QueryType type)
	{
		qType = type;
	}

	void print()
	{
		printf("{\n");
		{
			printf("\tname: %s\n", this->name.c_str());
			printf("\tquery_type: %d\n", this->qType);
			printf("\tquery_class: %d\n", this->qClass);
		}
		printf("}\n");
	}
};



#endif /* DNS_QUESTION_H_ */
