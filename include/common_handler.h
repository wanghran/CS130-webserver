#ifndef COMMON_HANDLER_H
#define COMMON_HANDLER_H

#include <string>
#include <boost/move/unique_ptr.hpp>
#include "config_parser.h"
#include "request.h"
#include "reply.h"

/**
 * This is a common handler which should be inherited by all handler class
*/ 
class common_handler
{
    public:
      static common_handler* create(const NginxConfig& config, const std::string& root_path);
      virtual std::unique_ptr<reply> HandleRequest(const Request& request) = 0;
    
    protected:
      NginxConfig config;
      std::string root_path;

};
#endif