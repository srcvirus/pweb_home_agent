/*
 * http_server.cpp
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "http_server.h"
#include "../../protocol/protocol_helper.h"
#include <boost/bind.hpp>

http_server::http_server(const std::string& address, const std::string& home_agent_alias, const unsigned short& port,
		boost::shared_ptr <IOServicePool>& io_service_pool_) :
		io_service_pool_(io_service_pool_), acceptor_(io_service_pool_->getDedicatedIOService()),
		new_connection_(), request_handler_(),
		home_agent_alias(home_agent_alias)
{
	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
	boost::asio::ip::tcp::resolver::query query(address, ProtocolHelper::intToString(port));
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();
}


void http_server::start_accept()
{
	new_connection_.reset(
			new connection(io_service_pool_->getIOService(),
					request_handler_));
	acceptor_.async_accept(new_connection_->socket(),
			boost::bind(&http_server::handle_accept, this,
					boost::asio::placeholders::error));
}

void http_server::handle_accept(const boost::system::error_code& e)
{
	if (!e)
	{
		new_connection_->start();
	}

	start_accept();
}

void http_server::handle_stop()
{
		;
}
