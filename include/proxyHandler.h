#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include <string>
#include "common_handler.h"

class proxyHandler : public common_handler
{
  public:
    proxyHandler(const NginxConfig& config, const std::string& root_path);

    static proxyHandler* create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<reply> HandleRequest(const Request& request);

    private:
      NginxConfig config;
      std::string root_path;
};
#endif
