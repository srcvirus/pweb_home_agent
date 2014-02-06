/*
 * dns_resource_record.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef DNS_RESOURCE_RECORD_H_
#define DNS_RESOURCE_RECORD_H_

#include "typedefs.h"

#include <netinet/in.h>
#include <string>
#include <vector>

using namespace std;

class DNSResourceRecord
{
	char* buffer;

	string name;
	vector <string> labels;

	QueryType	rType;
	QueryClass	rClass;

	unsigned int ttl;
	unsigned short rdLength;

	vector <char> rData;

public:

	DNSResourceRecord()
	{
		this->buffer = NULL;
		labels.clear();
		rData.clear();
		this->rdLength = 0;
	}

	DNSResourceRecord(char* _buf)
	{
		this->buffer = _buf;
		labels.clear();
		rData.clear();
		this->rdLength = 0;
		this->parseFromBuffer();
	}

	DNSResourceRecord& operator=(const DNSResourceRecord& record)
	{
		this->name = record.name;
		this->labels = record.labels;
		this->ttl = record.ttl;
		this->rdLength = record.rdLength;
		this->rData = record.rData;
		return *this;
	}

	void setBuffer(char* _buf)
	{
		this->buffer = _buf;
	}

	string& getName()
	{
		return name;
	}

	void setName(const std::string& name)
	{
		this->name = name;
	}

	vector <string>& getLabels()
	{
		return labels;
	}

	void setLabels(const vector <string>& labels)
	{
		this->labels = labels;
	}

	QueryClass getClass()
	{
		return rClass;
	}

	void setClass(QueryClass clazz)
	{
		rClass = clazz;
	}

	std::vector<char>& getData()
	{
		return rData;
	}

	void setData(const std::vector<char>& data)
	{
		rData = data;
	}

	unsigned short getRdLength()
	{
		return rdLength;
	}

	void setRdLength(unsigned short rdLength)
	{
		this->rdLength = rdLength;
	}

	QueryType getType()
	{
		return rType;
	}

	void setType(QueryType type)
	{
		rType = type;
	}

	unsigned int getTtl()
	{
		return ttl;
	}

	void setTtl(unsigned int ttl)
	{
		this->ttl = ttl;
	}

	long parseFromBuffer()
	{
		long offset = 0;
		while(true)
		{
			string nameComponent;		// name components are known as lebel in dns message format terminology
			offset = ProtocolHelper::extractStringFromByteBuffer(this->buffer, offset, nameComponent );
			if(nameComponent.length() <= 0) break;
			if(this->name.length() > 0) this->name += ".";
			this->name += nameComponent;
		}

		unsigned short rType, rClass, rdLength;
		unsigned int ttl;

		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &rType);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &rClass);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &ttl);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &rdLength);

		this->rType = (QueryType)ntohs(rType);
		this->rClass = (QueryClass)ntohs(rClass);
		this->ttl = ttl;
		this->rdLength = ntohs(rdLength);

		return offset;
	}

	long writeToBuffer()
	{
		long offset = 0;
		for(int i = 0; i < this->labels.size(); i++)
		{
			offset = ProtocolHelper::placeStringIntoByteBuffer(this->buffer, offset, labels[i]);
		}
		string endOfLabels = "";
		offset = ProtocolHelper::placeStringIntoByteBuffer(this->buffer, offset, endOfLabels);

		offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(this->buffer, offset, htons((unsigned short)this->rType));
		offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(this->buffer, offset, htons((unsigned short)this->rClass));
		offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(this->buffer, offset, ttl);
		offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(this->buffer, offset, htons(this->rdLength));
		for(int i = 0; i < this->rdLength; i++)
			offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(this->buffer, offset, this->rData[i]);

		return offset;
	}

	size_t getSize()
	{
		size_t size = 0;

		//size of all the labels + 1 byte for each of their length
		for(int i = 0; i < labels.size(); i++)
			size += (labels[i].size() + 1) * sizeof(char);

		size++; //1 byte to indicated 0 length of the end of labels string

		// ret type and ret class are of type unsigned short
		size += 2 * sizeof(unsigned short);

		// size of ttl
		size += sizeof(int);

		// size of return data length field
		size += sizeof(unsigned short);

		// size of the length of return data
		size += rdLength * sizeof(char);
		return size;
	}

	void print()
	{
		printf("{\n");
		printf("\tname: %s\n", this->name.c_str());
		printf("\tRet Type: %d\n", this->rType);
		printf("\tRet Class: %d\n", this->rClass);
		printf("\tRet Data Length: %d\n", this->rdLength);
		printf("\tRet Data:");
		for(int i = 0; i < rdLength; i++)
			printf(" 0x%x", this->rData[i]);
		printf("\n}\n");
	}
};



#endif /* DNS_RESOURCE_RECORD_H_ */
