#include "gtest/gtest.h"
#include "config_parser.h"
#include <iostream>
#include <fstream>

TEST(NginxConfigParserSemicolonTest, MissingSemicolon) {
  NginxConfigParser parse;
  NginxConfig out_config;
  //std::string ss = "foo bar\n aaa;\n";
  //write_files(ss);
  bool success = parse.Parse("../config/test1", &out_config);
  EXPECT_FALSE(success);
}

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parse;
  NginxConfig out_config;
  bool success = parse.Parse("../config/config", &out_config);
  EXPECT_TRUE(success);
}

TEST(NginxConfigParserNoInputTest, EmptyInput) {
  NginxConfigParser parse;
  NginxConfig out_config;
  bool success = parse.Parse("", &out_config);
  EXPECT_FALSE(success);
}

TEST(NginxConfigParserStreamTest, Stream) {
  NginxConfigParser parse;
  NginxConfig out_config;
  std::ifstream config_file;
  config_file.open("../config/test2");
  EXPECT_TRUE(config_file.good());

  const bool success = parse.Parse(dynamic_cast<std::istream*>(&config_file), &out_config);;
  EXPECT_TRUE(success);
}

TEST(NginxConfigParserTest2, ComplexTest) {
  NginxConfigParser parse;
  NginxConfig out_config;
  bool success = parse.Parse("../config/complex_test", &out_config);
  EXPECT_TRUE(success);
}
