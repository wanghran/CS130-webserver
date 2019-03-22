#include <algorithm>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "DataProcessor.h"
#include "httprequestparser.h"

DataProcessor::DataProcessor() {}

void DataProcessor::copyData(size_t bytesTransferred)
{
    all_data_.resize(all_data_.size() + bytesTransferred);
    std::copy(data_, data_ + bytesTransferred, all_data_.begin() + all_data_.size() - bytesTransferred);
}
    
httpparser::HttpRequestParser::ParseResult DataProcessor::parseHttpRequest(Request* req)
{
    httpparser::HttpRequestParser req_parser;
    httpparser::HttpRequestParser::ParseResult status = req_parser.parse(*req, &all_data_.front(), &all_data_.front() + all_data_.size());
    return status;
}

void DataProcessor::cleanData()
{
    all_data_.clear();
}

bool DataProcessor::check_request(Request* req, std::string* host)
{
    std::string host_ = "Host";
    bool find_host = false;
    for(std::vector<Request::HeaderItem>::const_iterator it = req->headers.begin();
            it != req->headers.end(); ++it)
    {
        if(host_.compare(it->name)==0)
        {
            find_host = true;
            *host = it->value;
        }
    }
    return find_host;
}

std::string DataProcessor::construct_url(Request* req, std::string host)
{
    return "http://"+ host+ req->uri;
}