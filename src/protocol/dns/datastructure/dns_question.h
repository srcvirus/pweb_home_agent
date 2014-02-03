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
	vector <std::string> labels;

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

	vector <std::string>& getLabels()
	{
		return this->labels;
	}

	void setLabels(const vector <std::string>& labels)
	{
		this->labels = labels;
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

	size_t getSize()
	{
		size_t size = 0;

		//size of all the labels + 1 byte for each of their length
		for(int i = 0; i < labels.size(); i++)
			size += (labels[i].length() + 1);

		// query type and query class are of type unsigned short
		size += 2 * sizeof(unsigned short);

		return size;
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
