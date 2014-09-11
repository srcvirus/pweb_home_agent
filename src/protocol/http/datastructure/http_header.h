/*
 * http_header.h
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

#ifndef HTTP_HEADER_H_
#define HTTP_HEADER_H_

#include <string>

using namespace std;

struct header {
  std::string name;
  std::string value;
};

#endif /* HTTP_HEADER_H_ */
