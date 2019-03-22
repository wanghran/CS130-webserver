#include "gtest/gtest.h"
#include "header.h"


TEST(header_test_OK_status, OK)
{
  header header_line;
  header_line.add_status(200);
  EXPECT_EQ(header_line.give_header(), "HTTP/1.1 200 OK\r\n\r\n");
}

TEST(header_test_notfound_status, not_found)
{
  header header_line;
  header_line.add_status(404);
  EXPECT_EQ(header_line.give_header(), "HTTP/1.1 404 Not Found\r\n\r\n");
}

TEST(header_test_connection_close, connection_close)
{
  header header_line;
  header_line.add_connection_close();
  EXPECT_EQ(header_line.give_header(), "Connection: close\r\n\r\n");
}

TEST(header_test_content_length, length_10)
{
  header header_line;
  header_line.add_content_length(10);
  EXPECT_EQ(header_line.give_header(), "Content-Length: 10\r\n\r\n");
}

TEST(header_test_content_type, plain_test)
{
  header header_line;
  header_line.add_content_type("text/plain");
  EXPECT_EQ(header_line.give_header(), "Content-Type: text/plain\r\n\r\n");
}

TEST(header_test_connection_keep_alive, keep_alive)
{
  header header_line;
  header_line.add_connect_alive(5, 1000);
  EXPECT_EQ(header_line.give_header(), "Connection: Keep-Alive\r\n"
                                       "Keep-Alive: timeout=5, max=1000\r\n\r\n");
}

TEST(header_test_binary_encoding, binary_encoding_enable)
{
  header header_line;
  header_line.add_binary_encoding();
  EXPECT_EQ(header_line.give_header(), "Content-Transfer-Encoding: binary\r\n\r\n");
}