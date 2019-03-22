#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include <fstream>
#include <string>
#include "common_handler.h"


/**
 * This is an implementation of static handler
*/ 
class staticHandler : public common_handler
{
  public:
    
    enum fileType
    {
      txt = 1,
      jpg = 2,
      zip = 3,
      html = 4,
      unsupported = 0
    };

    staticHandler(const NginxConfig& config, const std::string& root_path);

    static staticHandler* create(const NginxConfig& config, const std::string& root_path);

    std::unique_ptr<reply> HandleRequest(const Request& request);

    std::string get_file_path();

    fileType getFileType();
    

    private:
      std::string file;
      NginxConfig config;
      std::string root_path;
};
#endif