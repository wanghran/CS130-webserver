#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include <string>
#include "common_handler.h"

class echoHandler : public common_handler
{
  public:
    echoHandler(const NginxConfig& config, const std::string& root_path);

    static echoHandler* create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<reply> HandleRequest(const Request& request);

    private:
      NginxConfig config;
      std::string root_path;
};
#endif