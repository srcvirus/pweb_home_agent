#ifndef QUERY_STRING_PARSER_H_
#define QUERY_STRING_PARSER_H_

#include "../../datastructure/lookup_table.h"
#include "http_request_handler.h"

#include <vector>
#include <sstream>

using namespace std;

class request_handler;

class QueryStringParser {
  LookupTable<string, string> *key_value_store;

public:
  QueryStringParser() { key_value_store = new LookupTable<string, string>(); }

  string &strtolower(std::string &str) {
    for (int i = 0; i < (int)str.length(); i++)
      str[i] = tolower(str[i]);

    return str;
  }

  void parse(const string query_string) {
    vector<string> kvps = split(query_string, '&');
    for (vector<string>::iterator it = kvps.begin(); it != kvps.end(); ++it) {
      vector<string> kv = split(*it, '=');
      if (kv.size() == 2) {
        key_value_store->add(strtolower(kv[0]), kv[1]);
      } else if (kv.size() == 1) {
        string value = "";
        key_value_store->add(strtolower(kv[0]), value);
      } else {
        string value = "";
        for (int i = 1; i < kv.size(); ++i) {
          if (i > 1)
            value += "=";
          value += kv[i];
        }
        key_value_store->add(strtolower(kv[0]), value);
      }
      // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
      // printf(">>>>>>>>>%s, %s --> %s, %s<<<<<<<<<<<\n", kv[0].c_str(),
      // kv[1].c_str(), urlDecode(kv[0]).c_str(), urlDecode(kv[1]).c_str());
      // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    }
  }

  bool get_value(string key, string &value) {
    bool result = key_value_store->lookup(key, value);
    escapeSingleQuote(key);
    escapeSingleQuote(value);
    return result;
  }

  vector<string> &split(const string &s, char delim, vector<string> &elems) {
    // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    // printf("Received: %s\n", s.c_str());
    stringstream ss(s);
    ss << std::noskipws;
    string item;
    while (getline(ss, item, delim)) {
      elems.push_back(item);
      // printf("item: %s\n", item.c_str());
      item.clear();
    }
    // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    return elems;
  }

  vector<string> split(const string &s, char delim) {
    vector<string> elems;
    return split(s, delim, elems);
  }

  int size() { return key_value_store->size(); }

  void replaceStringInPlace(std::string &subject, const std::string &search,
                            const std::string &replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
    }
  }

  void escapeSingleQuote(string &value) {
    replaceStringInPlace(value, "'", "''");
  }
};

#endif
