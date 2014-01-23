/*
 * dns_header.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef DNS_HEADER_H_
#define DNS_HEADER_H_

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include "../protocol_helper.h"
#include "typedefs.h"


/*
DNS Header format (source: http://tools.ietf.org/html/rfc1035)

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    ID              A 16 bit identifier assigned by the program that
                generates any kind of query.  This identifier is copied
                the corresponding reply and can be used by the requester
                to match up replies to outstanding queries.

QR              A one bit field that specifies whether this message is a
                query (0), or a response (1).

OPCODE          A four bit field that specifies kind of query in this
                message.  This value is set by the originator of a query
                and copied into the response.  The values are:

                0               a standard query (QUERY)

                1               an inverse query (IQUERY)

                2               a server status request (STATUS)

                3-15            reserved for future use

AA              Authoritative Answer - this bit is valid in responses,
                and specifies that the responding name server is an
                authority for the domain name in question section.

                Note that the contents of the answer section may have
                multiple owner names because of aliases.  The AA bit
                corresponds to the name which matches the query name, or
                the first owner name in the answer section.

TC              TrunCation - specifies that this message was truncated
                due to length greater than that permitted on the
                transmission channel.

RD              Recursion Desired - this bit may be set in a query and
                is copied into the response.  If RD is set, it directs
                the name server to pursue the query recursively.
                Recursive query support is optional.

RA              Recursion Available - this be is set or cleared in a
                response, and denotes whether recursive query support is
                available in the name server.

Z Reserved for future use. Must be zero in all queries

                and responses.

RCODE           Response code - this 4 bit field is set as part of
                responses.  The values have the following
                interpretation:

                0               No error condition

                1               Format error - The name server was
                                unable to interpret the query.

                2               Server failure - The name server was
                                unable to process this query due to a
                                problem with the name server.

                3               Name Error - Meaningful only for
                                responses from an authoritative name
                                server, this code signifies that the
                                domain name referenced in the query does
                                not exist.

                4               Not Implemented - The name server does
                                not support the requested kind of query.

                5               Refused - The name server refuses to
                                perform the specified operation for
                                policy reasons.  For example, a name
                                server may not wish to provide the
                                information to the particular requester,
                                or a name server may not wish to perform
                                a particular operation (e.g., zone
                                transfer) for particular data.

                6-15            Reserved for future use.

QDCOUNT         an unsigned 16 bit integer specifying the number of
                entries in the question section.

ANCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the answer section.

NSCOUNT         an unsigned 16 bit integer specifying the number of name
                server resource records in the authority records
                section.

ARCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the additional records section.


*/

class DNSQueryHeader
{
	static size_t const HEADER_LENGTH = 12;
	char *buf;
	unsigned short flag;
public:

	DNSQueryHeader(char* _buf)
	{
		clear();
		buf = _buf;
		readFlags();
	}

	void readFlags()
	{
		long byteOffset = 2;
		ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &this->flag);
	}

	void writeFlag()
	{
		const long byteOffset = 2;
		ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, htons(flag));
	}

	void clear(){ buf = NULL; }


	unsigned short getId()
	{
		unsigned short id;
		const long byteOffset = 0;
		ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &id);
		return ntohs(id);
	}

	void setId(unsigned short id)
	{
		const long byteOffset = 0;
		id = htons(id);
		ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, id);
	}

	bool getQR()
	{
		long byteOffset = 0;
		int bitOffset = 7;
		char* ptr = (char*)&flag;
		return ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
	}

	void setQR(const bool qr)
	{
		long byteOffset = 0;
		int bitOffset = 7;
		char* ptr = (char*)&flag;
		if(qr)
			ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
		else
			ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
	}

	OpCode getOpcode() const
	{
		OpCode op;
		unsigned char val = ((unsigned char)flag & 0x78 ) >> 3;

		switch(val)
		{
		case 0:
			return O_QUERY;
		case 1:
			return O_IQUERY;
		case 2:
			return O_STATUS;
		case 3:
			return O_RESERVED;
		}
	}

	void setOpcode(const OpCode op)
	{
		flag |= (unsigned short)(op << 3);
	}

	bool getAA()
	{
		long byteOffset = 0;
		int bitOffset = 2;
		char* ptr = (char*)&flag;
		return ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
	}

	void setAA(const bool aa)
	{
		long byteOffset = 0;
		int bitOffset = 2;
		char* ptr = (char*)&flag;
		if(aa) ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
		else ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
	}

	bool getTC()
	{
		long byteOffset = 0;
		int bitOffset = 1;
		char* ptr = (char*)&flag;
		return ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
	}

	void setTC( bool const tc )
	{
		long byteOffset = 0;
		int bitOffset = 1;
		char* ptr = (char*)&flag;
		if(tc) ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
		else ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
	}

	bool getRD()
	{
		long byteOffset = 0;
		int bitOffset = 0;
		char* ptr = (char*)&flag;
		return ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
	}

	void setRD( bool const rd )
	{
		long byteOffset = 0;
		int bitOffset = 0;
		char* ptr = (char*)&flag;
		if(rd) ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
		else ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
	}

	bool getRA()
	{
		long byteOffset = 1;
		int bitOffset = 7;
		char* ptr = (char*)&flag;
		ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
	}

	void setRA( bool const ra )
	{
		const long byteOffset = 1;
		const int bitOffset = 7;
		char* ptr = (char*)&flag;
		if(ra) ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
		else ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
	}

	unsigned char getZ()
	{
		const long byteOffset = 3;
		const unsigned short mask = 0x7000;
		return (unsigned char)((flag & mask) >> (1 + sizeof(char)));
	}

	ReturnCode getRetCode() const
	{
		const unsigned short mask = 0x0f00;
		unsigned char val = (flag & mask) >> (4 + sizeof(char));

		switch ( val )
		{
		case 0:
			return R_SUCCESS;
	    case 1:
	    	return R_FORMAT_ERROR;
	    case 2:
	    	return R_SERVER_FAILURE;
	    case 3:
	    	return R_NAME_ERROR;
	    case 4:
	    	return R_NOT_IMPLEMENTED;
	    case 5:
	    	return R_REFUSED;
	    default:
	    	return R_RESERVED;
	    }
    }

	void setRetCode( ReturnCode code )
	{
		flag |= (unsigned char)(code << 4);
	}



	unsigned short getQDCount()
	{
		unsigned short ret;
		const long byteOffset = 4;
		ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &ret);
		return ntohs(ret);
	}

	void setQDCount(size_t qdcount)
	{
		const long byteOffset = 4;
		ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, htons((unsigned short)qdcount));
	}

	unsigned short getANCount()
	{
		unsigned short ret;
		const long byteOffset = 6;
		ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &ret);
		return ntohs(ret);
	}

	void setANCount(size_t ancount)
	{
		const long byteOffset = 6;
		ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, htons((unsigned short)ancount));
	}

	unsigned short getNSCount()
	{
		unsigned short ret;
		const long byteOffset = 8;
		ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &ret);
		return ntohs(ret);
	}

	void setNSCount(size_t nscount)
	{
		const long byteOffset = 8;
		ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, htons((unsigned short)nscount));
	}

	unsigned short getARCount()
	{
		unsigned short ret;
		const long byteOffset = 10;
		ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &ret);
		return ntohs(ret);
	}

	void setARCount(size_t arcount)
	{
		const long byteOffset = 10;
		ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, htons((unsigned short)arcount));
	}

	unsigned short getFlags()
	{
		return this->flag;
	}

	static size_t getDNSHeaderLength()
	{
		return DNSQueryHeader::HEADER_LENGTH;
	}

	void print()
	{
		printf("{\n");
		{
			printf("\tID: 0x%hx\n", this->getId());
			printf("\tFlags: 0x%hx\n", this->getFlags());
			printf("\tOpcode: 0x%x\n", this->getOpcode());
			printf("\tMessage type: 0x%x\n", this->getQR());
			printf("\tQuery count: 0x%x\n", this->getQDCount());
		}
		printf("}\n");
	}
};



#endif /* DNS_HEADER_H_ */
