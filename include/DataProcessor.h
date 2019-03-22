#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include<vector>
#include "httprequestparser.h"
#include "request.h"

/**
 * This is a utility class used to handle http request.
 * HTTP requests are parsed by this class.
*/ 
class DataProcessor
{
public:
    DataProcessor();


    /**
     * Copy all transfered data in this session to all_data. 
     * 
     * @param size of input handled in this session.
    */ 
    void copyData(size_t bytesTransferred);
    
    /**
     * Parse a http request 
     * 
     * @param size of input handled in this session.
     * @return the status of parsed result. It could be Error, Completed, or Incompleted. 
    */ 
    httpparser::HttpRequestParser::ParseResult parseHttpRequest(Request* req);

    /**
     * clear all stored data.
    */ 
    void cleanData();

    /**
     * Check if the received request contains a Host section in header.
     * 
     * @param Request object to be checked.
     * @param Name of section in header to be checked.
     * @return Ture if Host exists, otherwise False.
    */ 
    bool check_request(Request* req, std::string* host);
    
    /**
     * Reconstruct url if path and host are seperated.
     * 
     * @param Request object to be constructed.
     * @param Host.
     * @return a recontructed url.
    */ 
    std::string construct_url(Request* req, std::string host);

    enum { maxLength = 1024 };
    
    char data_[maxLength];
    
    std::vector<char> all_data_;
};

#endif