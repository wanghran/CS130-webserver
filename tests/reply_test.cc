#include "reply.h"
#include "gtest/gtest.h"

TEST(reply_echo_reply_connection_close, echo_back)
{
  reply echo_reply;
  echo_reply.set_header(200);
  std::string body = "GET /echo HTTP/1.1\r\nHost: localhost:2019\r\n\r\n";
  echo_reply.set_content(body);
  echo_reply.set_length(body.length());
  echo_reply.set_mime("text/plain");
  EXPECT_EQ(echo_reply.give_reply(), "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 44\r\n"
                                     "Content-Transfer-Encoding: binary\r\n"
                                     "Connection: close\r\n\r\n"
                                     "GET /echo HTTP/1.1\r\n"
                                     "Host: localhost:2019\r\n\r\n");
}

TEST(reply_echo_reply_connect_alive, echo_back)
{
  reply echo_reply;
  echo_reply.set_header(200);
  std::string body = "GET /echo HTTP/1.1\r\nHost: localhost:2019\r\n\r\n";
  echo_reply.set_content(body);
  echo_reply.set_length(body.length());
  echo_reply.set_mime("text/plain");
  echo_reply.set_keep_alive(5, 1000);
  EXPECT_EQ(echo_reply.give_reply(), "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 44\r\n"
                                     "Content-Transfer-Encoding: binary\r\n"
                                     "Connection: Keep-Alive\r\n"
                                     "Keep-Alive: timeout=5, max=1000\r\n\r\n"
                                     "GET /echo HTTP/1.1\r\n"
                                     "Host: localhost:2019\r\n\r\n");
}

TEST(reply_404, not_found)
{
  reply invalid_reply;
  invalid_reply.set_header(404);
  invalid_reply.set_length(0);
  EXPECT_EQ(invalid_reply.give_reply(), "HTTP/1.1 404 Not Found\r\n"
                                        "Content-Length: 0\r\n"
                                        "Content-Transfer-Encoding: binary\r\n"
                                        "Connection: close\r\n\r\n");
}