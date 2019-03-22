#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include "common_handler.h"

class statusHandler : public common_handler
{
    public:
        statusHandler(const NginxConfig& config, const std::string& root_path);

        static statusHandler* create(const NginxConfig& config, const std::string& root_path);
        std::unique_ptr<reply> HandleRequest(const Request& request);

    private:
        NginxConfig config;
        std::string root_path;
};
#endif