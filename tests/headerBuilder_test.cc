#include "gtest/gtest.h"
#include "header_builder.h"

TEST(header_build_ok_status, code_200)
{
  header* header_file = header_builder()
                        .add_status(200)
                        .build();

  EXPECT_EQ(header_file->give_header(), "HTTP/1.1 200 OK\r\n\r\n");
}

TEST(header_build_404_status, code_404)
{
  header* header_file = header_builder()
                        .add_status(404)
                        .build();
  EXPECT_EQ(header_file->give_header(), "HTTP/1.1 404 Not Found\r\n\r\n");
}