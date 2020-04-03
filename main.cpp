//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

//------------------------------------------------------------------------------
/*
	WebSocket chat server, multi-threaded

	This implements a multi-user chat room using WebSocket. The
	`io_context` runs on any number of threads, specified at
	the command line.

*/
//------------------------------------------------------------------------------

#include "listener.hpp"
#include "shared_state.hpp"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>
#include <chrono>

int
main(int argc, char* argv[])
{
	auto address = net::ip::make_address("127.0.0.1");
	unsigned short port = 8001;
	auto doc_root = ".";

	auto state = boost::make_shared<shared_state>(doc_root);

	auto network_thread = std::thread([&] {
		// The io_context is required for all I/O
		net::io_context ioc;

		// Create and launch a listening port
		boost::make_shared<listener>(
			ioc,
			tcp::endpoint{ address, port },
			state
		)->run();

		// Run the I/O service on the requested number of threads
		ioc.run();
	});

	auto last_update = std::chrono::steady_clock::now();
	for (;;) {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_update);
		if (elapsed.count() >= 1000) {
			last_update = std::chrono::steady_clock::now();
			state->send("hello");
		}
	}

	

	return EXIT_SUCCESS;
}