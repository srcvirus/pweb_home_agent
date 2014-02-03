/*
 * dns_resource_record.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef DNS_RESOURCE_RECORD_H_
#define DNS_RESOURCE_RECORD_H_

#include "typedefs.h"

using namespace std;

class DNSResourceRecord
{
	std::string name;
	QueryType	rType;
	QueryClass	rClass;
	unsigned int ttl;
	unsigned short rdLength;
	std::vector <char> rData;

public:
	std::string& getName() const
	{
		return name;
	}

	void setName(const std::string& name)
	{
		this->name = name;
	}

	QueryClass getClass() const
	{
		return rClass;
	}

	void setClass(QueryClass clazz)
	{
		rClass = clazz;
	}

	std::vector<char>& getData() const
	{
		return rData;
	}

	void setData(const std::vector<char>& data)
	{
		rData = data;
	}

	unsigned short getRdLength() const
	{
		return rdLength;
	}

	void setRdLength(unsigned short rdLength)
	{
		this->rdLength = rdLength;
	}

	QueryType getType() const
	{
		return rType;
	}

	void setType(QueryType type)
	{
		rType = type;
	}

	unsigned int getTtl() const
	{
		return ttl;
	}

	void setTtl(unsigned int ttl)
	{
		this->ttl = ttl;
	}


};



#endif /* DNS_RESOURCE_RECORD_H_ */
