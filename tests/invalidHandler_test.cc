#include "gtest/gtest.h"
#include "invalidHandler.h"
#include "header.h"
#include "reply.h"
#include "config_parser.h"
#include <string>

using namespace std;

TEST(invalidHandler_response, unfound_file)
{
    Request request;
	NginxConfig config;
    invalidHandler invalid_handler(config, "/");
    string response = invalid_handler.HandleRequest(request)->give_reply();
    string shouldBe("HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n");
    EXPECT_EQ(response, shouldBe);
}