#include "gtest/gtest.h"
#include "healthHandler.h"
#include <string>

using namespace std;

TEST(healthHandlerTest, OK_Response)
{
  Request request;
  NginxConfig config;
  healthHandler handler(config, "/");
  unique_ptr<reply> response = handler.HandleRequest(request);
  string shouldBe = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 2\r\n"
                    "Content-Transfer-Encoding: binary\r\n"
                    "Connection: close\r\n\r\n"
                    "OK";
  EXPECT_EQ(response->give_reply(), shouldBe);
}