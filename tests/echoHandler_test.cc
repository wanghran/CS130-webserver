#include "gtest/gtest.h"
#include "echoHandler.h"
#include "header.h"
#include "reply.h"
#include "config_parser.h"
#include <iostream>

using namespace std;

TEST(echoHandler_response_test, data_response)
{
	Request request;
	Request::Request::HeaderItem header_item;
	NginxConfig config;
	header_item.name = "Host";
	header_item.value = "localhost:2019";
	request.headers.push_back(header_item);
	request.versionMajor = 1;
	request.versionMinor = 1;
	request.method = "GET";
	request.uri = "/echo";

	echoHandler echo_handler(config, "/");
	string ret = echo_handler.HandleRequest(request)->give_reply();
	string shouldBe("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent"
					"-Length: 42\r\nContent-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n"
					"GET /echo HTTP/1.1\r\nHost: localhost:2019\r\n");
	EXPECT_EQ(ret, shouldBe);
}