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
#include "../../protocol_helper.h"
#include "typedefs.h"
#include "../../../global.h"

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

class DNSQueryHeader {
  char *buf;
  unsigned char flag[2];

  unsigned short id;

  bool qr;
  OpCode opcode;
  bool aa, tc, rd, ra;
  unsigned char z;
  ReturnCode retcode;

  unsigned short qdCount, anCount, nsCount, arCount;

public:

  DNSQueryHeader() { this->buf = NULL; }
  DNSQueryHeader(char *_buf) {
    clear();
    buf = _buf;
  }

  DNSQueryHeader &operator=(const DNSQueryHeader &header) {
    this->flag[0] = header.flag[0];
    this->flag[1] = header.flag[1];

    this->id = header.id;
    this->qr = header.qr;
    this->opcode = header.opcode;
    this->aa = header.aa;
    this->tc = header.tc;
    this->rd = header.rd;
    this->ra = header.ra;
    this->z = header.z;
    this->retcode = header.retcode;
    this->qdCount = header.qdCount;
    this->anCount = header.anCount;
    this->nsCount = header.nsCount;
    this->arCount = header.arCount;
    return *this;
  }

  void setBuffer(char *buffer) {
    clear();
    this->buf = buffer;
  }

  void readFlags() {
    long byteOffset = 2;
    this->flag[0] = buf[byteOffset++];
    this->flag[1] = buf[byteOffset++];

    readQR();
    readOpcode();
    readTC();
    readAA();
    readRD();
    readRA();
    readZ();
    readRetCode();
  }

  void writeFlag() {
    long byteOffset = 2;
    writeQR();
    writeOpcode();
    writeTC();
    writeAA();
    writeRD();
    writeRA();
    writeRetCode();
    byteOffset =
        ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, flag[0]);
    byteOffset =
        ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset, flag[1]);
  }

  void clear() { buf = NULL; }

  unsigned short getId() { return this->id; }

  void setId(unsigned short &id) { this->id = id; }

  void readId() {
    const long byteOffset = 0;
    ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset, &id);
    this->id = ntohs(this->id);
  }

  void writeId() {
    const long byteOffset = 0;
    ProtocolHelper::placeAtomicDataIntoByteBuffer(buf, byteOffset,
                                                  htons(this->id));
  }

  bool getQR() { return this->qr; }
  void setQR(bool &qr) { this->qr = qr; }

  // 0x80
  void readQR() {
    long byteOffset = 0;
    int bitOffset = 7;
    unsigned char *ptr = flag;
    this->qr = ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
  }

  void writeQR() {
    long byteOffset = 0;
    int bitOffset = 7;
    unsigned char *ptr = flag;
    if (this->qr)
      ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
    else
      ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
  }

  OpCode getOpcode() { return this->opcode; }
  void setOpcode(OpCode &code) { this->opcode = code; }

  void readOpcode() {
    unsigned char val = (flag[0] & 0x78) >> 3; // 0x78
    switch (val) {
    case 0:
      this->opcode = O_QUERY;
      break;
    case 1:
      this->opcode = O_IQUERY;
      break;
    case 2:
      this->opcode = O_STATUS;
      break;
    case 3:
      this->opcode = O_RESERVED;
      break;
    }
  }

  void writeOpcode() { flag[0] |= (unsigned short)(opcode << 3); }

  bool getAA() { return this->aa; }
  void setAA(bool &aa) { this->aa = aa; }

  // 0x04
  void readAA() {
    long byteOffset = 0;
    int bitOffset = 2;
    unsigned char *ptr = flag;
    this->aa = ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
  }

  void writeAA() {
    long byteOffset = 0;
    int bitOffset = 2;
    unsigned char *ptr = flag;
    if (this->aa)
      ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
    else
      ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
  }

  bool getTC() { return this->tc; }
  void setTC(bool &tc) { this->tc = tc; }

  // 0x02
  void readTC() {
    long byteOffset = 0;
    int bitOffset = 1;
    unsigned char *ptr = flag;
    this->tc = ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
  }

  void writeTC() {
    long byteOffset = 0;
    int bitOffset = 1;
    unsigned char *ptr = flag;
    if (this->tc)
      ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
    else
      ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
  }

  bool getRD() { return this->rd; }
  void setRD(bool &rd) { this->rd = rd; }

  // 0x01
  bool readRD() {
    long byteOffset = 0;
    int bitOffset = 0;
    unsigned char *ptr = flag;
    this->rd = ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
  }

  void writeRD() {
    long byteOffset = 0;
    int bitOffset = 0;
    unsigned char *ptr = flag;
    if (this->rd)
      ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
    else
      ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
  }

  bool getRA() { return this->ra; }
  void setRA(bool &ra) { this->ra = ra; }

  // 0x80
  void readRA() {
    long byteOffset = 1;
    int bitOffset = 7;
    unsigned char *ptr = flag;
    this->ra = ProtocolHelper::getBit(ptr, byteOffset, bitOffset);
  }

  void writeRA() {
    const long byteOffset = 1;
    const int bitOffset = 7;
    unsigned char *ptr = flag;
    if (this->ra)
      ProtocolHelper::setBit(ptr, byteOffset, bitOffset);
    else
      ProtocolHelper::resetBit(ptr, byteOffset, bitOffset);
  }

  unsigned char getZ() { return this->z; }
  void setZ(unsigned char &z) { this->z = z; }

  void readZ() {
    const unsigned short mask = 0x70;
    this->z = (unsigned char)((flag[1] & mask) >> 4);
  }

  ReturnCode getRetCode() { return this->retcode; }
  void setRetCode(ReturnCode &retcode) { this->retcode = retcode; }

  void readRetCode() {
    const unsigned short mask = 0xF0;
    unsigned char val = (flag[1] & mask) >> 4;

    switch (val) {
    case 0:
      this->retcode = R_SUCCESS;
      break;
    case 1:
      this->retcode = R_FORMAT_ERROR;
      break;
    case 2:
      this->retcode = R_SERVER_FAILURE;
      break;
    case 3:
      this->retcode = R_NAME_ERROR;
      break;
    case 4:
      this->retcode = R_NOT_IMPLEMENTED;
      break;
    case 5:
      this->retcode = R_REFUSED;
      break;
    default:
      this->retcode = R_RESERVED;
      break;
    }
  }

  void writeRetCode() { flag[1] |= (unsigned char)(this->retcode << 4); }

  unsigned short getQDCount() { return this->qdCount; }
  void setQDCount(unsigned short &qdCount) { this->qdCount = qdCount; }

  void readQDCount() {
    const long byteOffset = 4;
    ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset,
                                                    &this->qdCount);
    this->qdCount = ntohs(this->qdCount);
  }

  void writeQDCount() {
    const long byteOffset = 4;
    ProtocolHelper::placeAtomicDataIntoByteBuffer(
        buf, byteOffset, htons((unsigned short)this->qdCount));
  }

  unsigned short getANCount() { return this->anCount; }
  void setANCount(unsigned short &anCount) { this->anCount = anCount; }

  void readANCount() {
    const long byteOffset = 6;
    ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset,
                                                    &this->anCount);
    this->anCount = ntohs(this->anCount);
  }

  void writeANCount() {
    const long byteOffset = 6;
    ProtocolHelper::placeAtomicDataIntoByteBuffer(
        buf, byteOffset, htons((unsigned short)this->anCount));
  }

  unsigned short getNSCount() { return this->nsCount; }
  void setNSCount(unsigned short &nsCount) { this->nsCount = nsCount; }

  void readNSCount() {
    const long byteOffset = 8;
    ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset,
                                                    &this->nsCount);
    this->nsCount = ntohs(this->nsCount);
  }

  void writeNSCount() {
    const long byteOffset = 8;
    ProtocolHelper::placeAtomicDataIntoByteBuffer(
        buf, byteOffset, htons((unsigned short)this->nsCount));
  }

  unsigned short getARCount() { return this->arCount; }
  void setARCount(unsigned short &arCount) { this->arCount = arCount; }

  void readARCount() {
    const long byteOffset = 10;
    ProtocolHelper::extractAtomicDataFromByteBuffer(buf, byteOffset,
                                                    &this->arCount);
    this->arCount = htons(this->arCount);
  }

  void writeARCount() {
    const long byteOffset = 10;
    ProtocolHelper::placeAtomicDataIntoByteBuffer(
        buf, byteOffset, htons((unsigned short)this->arCount));
  }

  unsigned char *getFlags() { return this->flag; }

  static size_t getDNSHeaderLength() { return DNS_HEADER_LENGTH; }

  long parseFromBuffer() {
    readId();
    readFlags();
    readQDCount();
    readANCount();
    readNSCount();
    readARCount();
    return DNS_HEADER_LENGTH;
  }

  long writeToBuffer() {
    writeId();
    writeFlag();
    writeQDCount();
    writeANCount();
    writeNSCount();
    writeARCount();
  }

  void print() {
    printf("{\n");
    {
      this->readFlags();
      printf("\tID: 0x%hx\n", this->getId());
      printf("\tFlags: 0x%2hx%2hx\n", this->getFlags()[0], this->getFlags()[1]);
      printf("\tOpcode: 0x%x\n", this->getOpcode());
      printf("\tMessage type: 0x%x\n", this->getQR());
      printf("\tQuery count: 0x%x\n", this->getQDCount());
    }
    printf("}\n");
  }
};

#endif /* DNS_HEADER_H_ */
