/*
 * dns_query.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef DNS_QUERY_H_
#define DNS_QUERY_H_

#include <vector>
#include <stdio.h>
#include "../../protocol_helper.h"
#include "dns_question.h"
#include "dns_query_header.h"
#include "dns_resource_record.h"

using namespace std;

/**
 * Explanation of DNS Message
 * http://www.zytrax.com/books/dns/ch15/
 */

class DNSMessage
{
	char* buffer;

	DNSQueryHeader header;
	vector <DNSQuestion> questions;
	vector <DNSResourceRecord> answers, authority, additional;

public:
	DNSMessage()
	{
		this->buffer = NULL;
		header.clear();
		questions.clear();
		answers.clear();
		authority.clear();
		additional.clear();
	}

	DNSMessage(char* _buf)
	{
		this->buffer = _buf;
		header.clear();
		questions.clear();
		answers.clear();
		authority.clear();
		additional.clear();
	}

	char* getBuffer(){ return this->buffer; }

	size_t getSize();

	DNSQueryHeader& getDNSHeader()
	{
		return this->header;
	}

	void setDNSHeader(const DNSQueryHeader& header){ this->header = header; }

	vector <DNSQuestion>& getDNSQuestions()
	{
		return this->questions;
	}

	void setDNSQuestion(const vector <DNSQuestion>& questions){ this->questions = questions; }

	vector <DNSResourceRecord>& getDNSAnswers(){ return this->answers; }
	vector <DNSResourceRecord>& getDNSAuthority(){ return this->authority; }
	vector <DNSResourceRecord>& getDNSAdditional(){ return this->additional; }

	long parse();
	long write();

	long parseHeaders(long offset);
	long parseQuestions(long offset);
	long parseAnswers(long offset);
	long parseAuthority(long offset);
	long parseAdditional(long offset);

	long writeHeaders(long offset);
	long writeQuestions(long offset);
	long writeAnswers(long offset);
	long writeAuthority(long offset);
	long writeAdditional(long offset);

	void allocateBuffer()
	{
		size_t bufferSize = this->getSize();
		//if(this->buffer != NULL) delete[] this->buffer;
		this->buffer = new char[bufferSize];
	}

	void deallocateBuffer()
	{
		if(this->buffer != NULL)
			delete[] buffer;
		this->buffer = NULL;
	}

	~DNSMessage()
	{
		//this->deallocateBuffer();
	}

	DNSMessage& operator=(const DNSMessage& msg)
	{
		this->header = msg.header;
		this->questions = msg.questions;
		this->answers = msg.answers;
		this->authority = msg.authority;
		this->additional = msg.additional;
		return *this;
	}

	void print()
	{
		printf("------------------------------------------\n");
		header.print();
		printf("nQuestions: %d\n", questions.size());
		for(int i = 0; i < questions.size(); i++)
			questions[i].print();

		printf("nAnswers: %d\n", answers.size());
		for(int i = 0; i < answers.size(); i++)
			answers[i].print();

		printf("nAuthority: %d\n", authority.size());
		for(int i = 0; i < authority.size(); i++)
			authority[i].print();

		printf("nAdditional: %d\n", additional.size());
		for(int i = 0; i < additional.size(); i++)
			additional[i].print();
		printf("------------------------------------------\n");
	}
};


#endif /* DNS_QUERY_H_ */
