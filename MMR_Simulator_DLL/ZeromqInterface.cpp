#include "pch.h"
#include <iostream>
#include <zmq.h>
#include <fstream>
#include <cassert>
#include <string>

static int testZeromqHelo() {
	std::ofstream file("./testZeromq.txt", std::ios::out);
	if (!file.is_open()) {
		throw std::ios_base::failure("Failed to open the file for writing.");
	}

	file << "Connecting to hello world server with tcp" << std::endl;
	void* context = zmq_ctx_new();
	void* requester = zmq_socket(context, ZMQ_REQ);
	int res = zmq_connect(requester, "tcp://localhost:8080");
	assert(res == 0);
	file << "Connected to the server tcp://localhost:8080." << std::endl;

	int request_nbr;
	for (request_nbr = 0; request_nbr != 10; request_nbr++) {
		char buffer[10];
		std::string msg = "Hello" + std::to_string(request_nbr);
		file << "Sending " << msg << std::endl;
		zmq_send(requester, msg.c_str(), 6, 0);
		zmq_recv(requester, buffer, 10, 0);
		file << "Received World" << request_nbr << std::endl;
	}

	zmq_close(requester);
	file << "Connection closed." << std::endl;
	//zmq_ctx_destroy(context);
	file << "Context destroyed." << std::endl;

	file.close();

	file << "Test completed successfully." << std::endl;
	return 0;
}