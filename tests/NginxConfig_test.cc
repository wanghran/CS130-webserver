#include "gtest/gtest.h"
#include "config_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

TEST(NginxConfigTest, ToString) {
  NginxConfigParser parse;
  NginxConfig out_config;
  std::string ss = "foo;";
  //std::string correct = "TOKEN_TYPE_NORMAL: foo\nTOKEN_TYPE_STATEMENT_END: ;\nTOKEN_TYPE_ENDOFLINE: \nTOKEN_TYPE_EOF: \nfoo;\n";
  //write_files(ss);
  parse.Parse("../config/test2", &out_config);
  ss += '\n';
  EXPECT_EQ(ss, out_config.ToString().c_str());
  //write_files(out_config.ToString().c_str());
}

TEST(NginxConfigTest2, ToString) {
  NginxConfigParser parse;
  NginxConfig out_config;
  std::ifstream config_file;
  config_file.open("../config/nginx_config_test1");
  std::ostringstream sin;
  sin<<config_file.rdbuf();
  std::string ss = sin.str();
  config_file.close();
  config_file.clear();
  ss += '\n';
  parse.Parse("../config/nginx_config_test1", &out_config);

  EXPECT_EQ(ss, out_config.ToString().c_str());
}