/*
 * dns_question.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef DNS_QUESTION_H_
#define DNS_QUESTION_H_

#include "typedefs.h"
#include "../../protocol_helper.h"

#include <string>
#include <vector>

#include <netinet/in.h>

using namespace std;

class DNSQuestion {
  char *buffer;

  string name;
  vector<string> labels;

  QueryType qType;
  QueryClass qClass;

public:

  DNSQuestion() { this->buffer = NULL; }

  DNSQuestion(char *_buf) {
    this->buffer = _buf;
    this->parseFromBuffer();
  }

  void setBuffer(char *_buf) { this->buffer = _buf; }

  std::string &getName() { return name; }

  void setName(const std::string &name) { this->name = name; }

  std::vector<std::string> &getLabels() { return this->labels; }

  void setLabels(const std::vector<std::string> &labels) {
    this->labels = labels;
  }
  QueryClass getClass() { return qClass; }

  void setClass(QueryClass clazz) { qClass = clazz; }

  QueryType getType() { return qType; }

  void setType(QueryType type) { qType = type; }

  size_t getSize() {
    size_t size = 0;

    // size of all the labels + 1 byte for each of their length
    for (int i = 0; i < labels.size(); i++)
      size += (labels[i].size() + 1) * sizeof(char);

    size++; // 1 byte to indicated 0 length of the end of labels string

    // query type and query class are of type unsigned short
    size += 2 * sizeof(unsigned short);

    return size;
  }

  long parseFromBuffer() {
    long offset = 0;
    while (true) {
      string nameComponent; // name components are known as lebel in dns message
                            // format terminology
      offset = ProtocolHelper::extractStringFromByteBuffer(this->buffer, offset,
                                                           nameComponent);
      if (nameComponent.length() <= 0)
        break;

      if (this->name.length() > 0)
        this->name += ".";
      this->name += nameComponent;
      this->labels.push_back(nameComponent);
    }

    unsigned short qType, qClass;

    offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer,
                                                             offset, &qType);
    offset = ProtocolHelper::extractAtomicDataFromByteBuffer(this->buffer,
                                                             offset, &qClass);

    this->qType = (QueryType)ntohs(qType);
    this->qClass = (QueryClass)ntohs(qClass);

    return offset;
  }

  long writeToBuffer() {
    long offset = 0;
    for (int i = 0; i < this->labels.size(); i++) {
      offset = ProtocolHelper::placeStringIntoByteBuffer(this->buffer, offset,
                                                         labels[i]);
    }
    string endOfLables = "";
    offset = ProtocolHelper::placeStringIntoByteBuffer(this->buffer, offset,
                                                       endOfLables);

    offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(
        this->buffer, offset, htons((unsigned short)this->qType));
    offset = ProtocolHelper::placeAtomicDataIntoByteBuffer(
        this->buffer, offset, htons((unsigned short)this->qClass));

    return offset;
  }

  DNSQuestion &operator=(const DNSQuestion &q) {
    this->labels = q.labels;
    this->name = q.name;
    this->qClass = q.qClass;
    this->qType = q.qType;
    return *this;
  }

  void print() {
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
