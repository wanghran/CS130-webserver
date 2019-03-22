#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "DataProcessor.h"
#include "httprequestparser.h"
#include "request.h"


TEST(DataProcessorCopyDataTest, CopyZeroByte) {
  DataProcessor dataProcessor;
  EXPECT_EQ(0, dataProcessor.all_data_.size());
  dataProcessor.copyData(0);
  EXPECT_EQ(0, dataProcessor.all_data_.size());
}

TEST(DataProcessorCopyDataTest, CopyNonzeroByte) {
  DataProcessor dataProcessor;
  EXPECT_EQ(0, dataProcessor.all_data_.size());
  strcpy(dataProcessor.data_, "Hello");
  dataProcessor.copyData(5);
  EXPECT_EQ(5, dataProcessor.all_data_.size());
  EXPECT_EQ("Hello", std::string(dataProcessor.all_data_.begin(), dataProcessor.all_data_.end()));
}

TEST(DataProcessorParseHttpRequestTest, ParseEmptyString) {
    DataProcessor dataProcessor;
    Request req;
    EXPECT_EQ(httpparser::HttpRequestParser::ParsingIncompleted, dataProcessor.parseHttpRequest(&req));
}

TEST(DataProcessorParseHttpRequestTest, ParseInvalidString) {
    std::string s = "GET\r\n";
    Request req;
    DataProcessor dataProcessor;
    std::copy(s.begin(), s.end(), std::back_inserter(dataProcessor.all_data_));
    EXPECT_EQ("GET\r\n", std::string(dataProcessor.all_data_.begin(), dataProcessor.all_data_.end()));
    EXPECT_EQ(httpparser::HttpRequestParser::ParsingError, dataProcessor.parseHttpRequest(&req));
}

TEST(DataProcessorParseHttpRequestTest, ParseIncompletedString) {
    std::string s = "GET / HTTP/1.1\r\n";
    Request req;
    DataProcessor dataProcessor;
    std::copy(s.begin(), s.end(), std::back_inserter(dataProcessor.all_data_));
    EXPECT_EQ("GET / HTTP/1.1\r\n", std::string(dataProcessor.all_data_.begin(), dataProcessor.all_data_.end()));
    EXPECT_EQ(httpparser::HttpRequestParser::ParsingIncompleted, dataProcessor.parseHttpRequest(&req));
}

TEST(DataProcessorParseHttpRequestTest, ParseCompletedString) {
    std::string s = "GET / HTTP/1.1\r\n\r\n";
    Request req;
    DataProcessor dataProcessor;
    std::copy(s.begin(), s.end(), std::back_inserter(dataProcessor.all_data_));
    EXPECT_EQ("GET / HTTP/1.1\r\n\r\n", std::string(dataProcessor.all_data_.begin(), dataProcessor.all_data_.end()));
    EXPECT_EQ(httpparser::HttpRequestParser::ParsingCompleted, dataProcessor.parseHttpRequest(&req));
}

TEST(DataProcessorClean, NonZeroInput) {
    DataProcessor dataProcessor;
    strcpy(dataProcessor.data_, "Hello");
    dataProcessor.copyData(5);
    dataProcessor.cleanData();
    EXPECT_EQ(dataProcessor.all_data_.size(), 0);
}

TEST(DataProcessorConstructUrlTest, ConstructUrl) {
    DataProcessor d;
    Request req;
    std::string result;
    std::string host = "localhost";
    req.uri = "/static/1.txt";
    result = d.construct_url(&req, host);
    EXPECT_EQ("http://localhost/static/1.txt", result);
}

TEST(DataProcessorCheckRequestTest, CheckRequest) {
    DataProcessor d;
    Request req;
    std::string host;
    std::vector<Request::HeaderItem> v;
    //Host", "http://localhost"
    struct Request::HeaderItem h = Request::HeaderItem();
    h.name = "Host";
    h.value = "http://localhost";
    v.push_back(h);
    req.headers = v;
    bool status = d.check_request(&req,&host);
    EXPECT_TRUE(status);
}