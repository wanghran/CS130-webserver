#ifndef MALFORM_HANDLER_H
#define MALFORM_HANDLER_H

#include "common_handler.h"

class malformHandler:public common_handler
{
  private:
    NginxConfig config;
    std::string root_path;

  public:
    malformHandler(const NginxConfig &config, const std::string &root_path);
    static malformHandler* create(const NginxConfig &config, const std::string &root_path);
    std::unique_ptr<reply> HandleRequest(const Request &request);
};
#endif