/*
 * Copyright (C) Alex Nekipelov (alex@nekipelov.net)
 * License: MIT
 */
//Source: https://github.com/nekipelov/httpparser

#ifndef HTTPPARSER_REQUEST_H
#define HTTPPARSER_REQUEST_H

#include <string>
#include <vector>
#include <sstream>

/**
 * An implementation of request class
*/ 
class Request {
   public:

    struct HeaderItem
    {
        std::string name;
        std::string value;
    };

    std::string method;
    std::string uri;
    int versionMajor = 0;
    int versionMinor = 0;
    std::vector<HeaderItem> headers;
    std::vector<char> content;
    bool keepAlive = false;
    std::string file_name;
    std::string file_path;
    std::string proxy_port;
    std::string request_uri;
    std::string query;

    /**
     * create a stream to output request header and content
     * Do Not Use This Method in a CONST environment.
     * 
     * @param string of request header and content.
    */ 
    std::string inspect();
    Request();
};

#endif // HTTPPARSER_REQUEST_H
