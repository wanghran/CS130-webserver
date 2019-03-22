#include "gtest/gtest.h"
#include "malformHandler.h"
#include <string>

using namespace std;

TEST(malformHandler_test, Bad_Request)
{
  NginxConfig config;
  Request request;
  malformHandler handler(config, "/");
  string ret = handler.HandleRequest(request)->give_reply();
  string shouldBe = "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Length: 0\r\n"
                    "Content-Transfer-Encoding: binary\r\n"
                    "Connection: close\r\n\r\n";
  EXPECT_EQ(ret, shouldBe);
}