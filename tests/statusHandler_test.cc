#include <string>
#include "gtest/gtest.h"
#include "statusHandler.h"
#include "header.h"
#include "reply.h"
#include "config_parser.h"

TEST(statusHandler_response_test, data_response)
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
	request.uri = "/status";

	statusHandler status_handler(config, "/");
    std::string ret = status_handler.HandleRequest(request)->give_reply();
	std::string shouldBe("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 200\r\n"
                            "Content-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n"
                            "URL Requested\tNumber of Requests\r\nTotal number of requests: 0\r\n\r\n"
                            "Status Code Returned\tNumber of Responses\r\nTotal number of responses: 0\r\n\r\n"
                            "Request Handlers\tURL prefixes\r\n/echo\t/echo\r\n/status\t/status\r\n");
	EXPECT_EQ(ret, shouldBe);
}
