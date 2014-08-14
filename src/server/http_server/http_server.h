/*
 * http_server.h
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "tcp_connection.h"
#include "../../communication/io_service_pool.h"
#include "../../protocol/http/http_request_handler.h"

/// The top-level class of the HTTP server.
class http_server : private boost::noncopyable {
public:
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit http_server(const std::string &address,
                       const std::string &home_agent_alias,
                       const unsigned short &port,
                       boost::shared_ptr<IOServicePool> &io_service_pool_);

  /// Initiate an asynchronous accept operation.
  void start_accept();

private:

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code &e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// The pool of io_service objects used to perform asynchronous operations.
  boost::shared_ptr<IOServicePool> io_service_pool_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  /// The handler for all incoming requests.
  request_handler request_handler_;

  std::string home_agent_alias;
};

#endif /* HTTP_SERVER_H_ */
