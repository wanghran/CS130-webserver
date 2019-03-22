#ifndef HEALTH_HANDLER_H
#define HEALTH_HANDLER_H

#include "common_handler.h"

class healthHandler : public common_handler
{
  private:
    NginxConfig config;
    std::string root_path;

  public:
    healthHandler(const NginxConfig &config, const std::string &root_path);
    static healthHandler *create(const NginxConfig &config, const std::string &root_path);
    std::unique_ptr<reply> HandleRequest(const Request &request);

};
#endif