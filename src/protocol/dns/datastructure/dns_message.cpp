/*
 * dns_message.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: sr2chowd
 */

#include "../../protocol_helper.h"
#include "dns_message.h"
#include "../../../global.h"

long DNSMessage::parseHeaders(long offset) {
  this->header.setBuffer(this->buffer + offset);
  return offset + this->header.parseFromBuffer();
}

long DNSMessage::parseQuestions(long offset) {
  unsigned short qCount = this->header.getQDCount();
  for (unsigned short i = 0; i < qCount; i++) {
    DNSQuestion question(this->buffer + offset);
    this->questions.push_back(question);
    offset += question.getSize();
  }
  return offset;
}

long DNSMessage::parse() {
  long offset = 0;
  offset = parseHeaders(offset);
  offset = parseQuestions(offset);
  offset = parseAnswers(offset);
  offset = parseAuthority(offset);
  offset = parseAdditional(offset);
  this->bufferSize = offset;
  return offset;
}

long DNSMessage::write() {
  long offset = 0;
  offset = writeHeaders(offset);
  offset = writeQuestions(offset);
  offset = writeAnswers(offset);
  offset = writeAuthority(offset);
  offset = writeAdditional(offset);
  return offset;
}
long DNSMessage::writeHeaders(long offset) {
  this->header.setBuffer(this->buffer);
  return offset + this->header.writeToBuffer();
}

long DNSMessage::writeQuestions(long offset) {
  size_t i;
  for (i = 0; i < questions.size(); i++) {
    questions[i].setBuffer(this->buffer + offset);
    offset += questions[i].writeToBuffer();
  }
  return offset;
}

long DNSMessage::writeAnswers(long offset) {
  size_t i;
  for (i = 0; i < answers.size(); i++) {
    answers[i].setBuffer(this->buffer + offset);
    offset += answers[i].writeToBuffer();
  }
  return offset;
}

long DNSMessage::writeAuthority(long offset) {
  size_t i;
  for (i = 0; i < authority.size(); i++) {
    authority[i].setBuffer(this->buffer + offset);
    offset += authority[i].writeToBuffer();
  }
  return offset;
}

long DNSMessage::writeAdditional(long offset) {
  size_t i;
  for (i = 0; i < additional.size(); i++) {
    additional[i].setBuffer(this->buffer + offset);
    offset += additional[i].writeToBuffer();
  }
  return offset;
}

long DNSMessage::parseAnswers(long offset) {
  unsigned short i;
  unsigned short anCount = this->header.getANCount();
  for (i = 0; i < anCount; i++) {
    DNSResourceRecord answer(this->buffer + offset);
    this->answers.push_back(answer);
    offset += answer.getSize();
  }
  return offset;
}

long DNSMessage::parseAuthority(long offset) {
  unsigned short i, authCount = this->header.getNSCount();
  for (i = 0; i < authCount; i++) {
    DNSResourceRecord auth(this->buffer + offset);
    this->authority.push_back(auth);
    offset += auth.getSize();
  }
  return offset;
}

long DNSMessage::parseAdditional(long offset) {
  unsigned short i, arCount = this->header.getARCount();
  for (i = 0; i < arCount; i++) {
    DNSResourceRecord additional(this->buffer + offset);
    this->additional.push_back(additional);
    offset += additional.getSize();
  }
  return offset;
}

size_t DNSMessage::getSize() {
  size_t i, size = 0;
  size += DNSQueryHeader::getDNSHeaderLength();
  for (i = 0; i < questions.size(); i++)
    size += questions[i].getSize();
  for (i = 0; i < answers.size(); i++)
    size += answers[i].getSize();
  for (i = 0; i < authority.size(); i++)
    size += authority[i].getSize();
  for (i = 0; i < additional.size(); i++)
    size += additional[i].getSize();
  return size;
}
