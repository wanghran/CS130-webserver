#ifndef INVALID_HANDLER_H
#define INVALID_HANDLER_H
#include <string>
#include "common_handler.h"

class invalidHandler : public common_handler
{
  public:
    invalidHandler(const NginxConfig& config, const std::string& root_path);

    static invalidHandler* create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<reply> HandleRequest(const Request& request);
  
  private:
    NginxConfig config;
    std::string root_path;
};
#endif