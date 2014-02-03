/*
 * dns_message.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#include "../../protocol_helper.h"
#include "dns_message.h"
#include "../../../global.h"

long DNSMessage::parseHeaders(long offset)
{
	this->header.setBuffer(this->buffer);
	return offset + DNS_HEADER_LENGTH;
}

long DNSMessage::parseQuestions(long offset)
{
	int qCount = this->header.getQDCount();

	for(int i = 0; i < qCount; i++)
	{
		DNSQuestion question;
		unsigned short qType, qClass;

		while(true)
		{
			string nameComponent;		// name components are known as lebel in dns message format terminology
			offset = ProtocolHelper::extractStringFromByteBuffer(this->buffer, offset, nameComponent );
			if(nameComponent.length() <= 0) break;

			if(question.getName().length() > 0) question.getName() += ".";
			question.getName() += nameComponent;
			question.getLabels().push_back(nameComponent);
		}

		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &qType);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &qClass);

		qType = ntohs(qType);
		qClass = ntohs(qClass);

		question.setType((QueryType)qType);
		question.setClass((QueryClass)qClass);

		this->questions.push_back(question);
	}
}

long DNSMessage::parseResrouceRecord(long offset, vector <DNSResourceRecord>& rr, size_t n)
{
	for(int i = 0; i < n; i++)
	{
		DNSResourceRecord record;
		unsigned short rType, rClass;
		unsigned int ttl;
		unsigned short rdLength;
		std::vector <char> rData;

		while(true)
		{
			string nameComponent;		// name components are known as lebel in dns message format terminology
			offset = ProtocolHelper::extractStringFromByteBuffer(this->buffer, offset, nameComponent );
			if(nameComponent.length() <= 0) break;
			if(record.getName().length() > 0) record.getName() += ".";
			record.getName() += nameComponent;
		}

		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &rType);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &rClass);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &ttl);
		offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer, offset, &rdLength);

		record.setType((QueryType)rType);
		record.setClass((QueryClass)rClass);
		record.setTtl(ttl);
		record.setRdLength(rdLength);

		for(int j = 0; j < rdLength; j++)
			record.getData().push_back(this->buffer[offset++]);
	}
}

/**
 * char* buffer;

	DNSQueryHeader header;
	vector <DNSQuestion> questions;
	vector <DNSResourceRecord> answers, authority, additional;
 * @return
 */
size_t DNSMessage::getSize()
{
	size_t size = 0;
	size += DNSQueryHeader::getDNSHeaderLength();
	for(int i = 0; i < questions.size(); i++)
		size += questions[i].getSize();

	for(int i = 0; i < answers.size(); i++);
	for(int i = 0; i < answers.size(); i++);
	for(int i = 0; i < answers.size(); i++);
	for(int i = 0; i < answers.size(); i++);
}



