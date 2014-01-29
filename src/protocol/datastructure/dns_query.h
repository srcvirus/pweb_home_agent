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
#include "../protocol_helper.h"
#include "dns_question.h"
#include "dns_query_header.h"

using namespace std;

class DNSQuery
{
	DNSQueryHeader header;
	vector <DNSQuestion> questions;

public:
	DNSQuery(char* _buf):header(DNSQueryHeader(_buf))
	{
		header.print();

		int qCount = header.getQDCount();
		long offset = DNSQueryHeader::getDNSHeaderLength();

		for(int i = 0; i < qCount; i++)
		{
			DNSQuestion question;
			unsigned short qType, qClass;
			while(true)
			{
				string nameComponent;		// name components are known as lebel in dns message format terminology
				offset = ProtocolHelper::extractStringFromByteBuffer(_buf, offset, nameComponent );
				if(nameComponent.length() <= 0) break;

				if(question.getName().length() > 0) question.getName() += ".";
				question.getName() += nameComponent;
			}

			offset = ProtocolHelper::extractAtomicDataFromByteBuffer(_buf, offset, &qType);
			offset = ProtocolHelper::extractAtomicDataFromByteBuffer(_buf, offset, &qClass);
			qType = ntohs(qType);
			qClass = ntohs(qClass);

			question.setType((QueryType)qType);
			question.setClass((QueryClass)qClass);

			this->questions.push_back(question);
		}

		for(int i = 0; i < qCount; i++)
			questions[i].print();
	}
};


#endif /* DNS_QUERY_H_ */
