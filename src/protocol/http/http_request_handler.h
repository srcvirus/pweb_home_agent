/*
 * http_request_handler.h
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

#ifndef HTTP_REQUEST_HANDLER_H_
#define HTTP_REQUEST_HANDLER_H_

// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "query_string_parser.h"
#include "datastructure/http_reply.h"

#include "../rest_api_helper.h"

using namespace std;

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler: private boost::noncopyable
{
public:
	/// Construct with a directory containing files to be served.
	explicit request_handler(const string& homeAgentAlias);
	string homeAgentAlias;

	/// Handle a request and produce a reply.
	void handle_request(const request& req, reply& rep);

	/// Perform URL-decoding on a string. Returns false if the encoding was
	/// invalid.
	static bool url_decode(const std::string& in, std::string& out);

	static std::string& strtolower(std::string& str);
	boost::shared_ptr <RESTAPIHelper> restapi;
private:
	/// Build the response string
	void build_response(QueryStringParser& qsp, string& http_payload, reply::status_type& http_response);

};

#endif /* HTTP_REQUEST_HANDLER_H_ */
