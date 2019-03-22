#include <string>
#include "invalidHandler.h"

using namespace std;

invalidHandler::invalidHandler(const NginxConfig& config, const string& root_path) 
{
    this->config = config;
	this->root_path = root_path;
}


invalidHandler* invalidHandler::create(const NginxConfig& config, const string& root_path)
{
    invalidHandler* invalid_handler = new invalidHandler(config, root_path);
    return invalid_handler;
}

std::unique_ptr<reply> invalidHandler::HandleRequest(const Request& request)
{
    std::unique_ptr<reply> response(new reply);
    response->set_header(404);
    response->set_length(0);
    return response;
}
