#include <string>
#include "statusHandler.h"

statusHandler::statusHandler(const NginxConfig& config, const std::string& root_path) 
{
	this->config = config;
	this->root_path = root_path;
}

statusHandler* statusHandler::create(const NginxConfig& config, const std::string& root_path)
{
    statusHandler* status_handler = new statusHandler(config, root_path);
	return status_handler;
}

std::unique_ptr<reply> statusHandler::HandleRequest(const Request& request)
{
    std::unique_ptr<reply> response(new reply);
    std::string content;
	
    int total_num_url_requested = 0;
    content.append("URL Requested\tNumber of Requests\r\n");
    for (auto const& x : config.num_url_requested)
    {
        total_num_url_requested += x.second;
        content.append(x.first + "\t" + std::to_string(x.second) + "\r\n");
    }
    content.append("Total number of requests: " + std::to_string(total_num_url_requested) + "\r\n\r\n");

    int total_num_code_returned = 0;
    content.append("Status Code Returned\tNumber of Responses\r\n");
    for (auto const& x : config.num_code_returned)
    {
        total_num_code_returned += x.second;
        content.append(x.first + "\t" + std::to_string(x.second) + "\r\n");
    }
    content.append("Total number of responses: " + std::to_string(total_num_code_returned) + "\r\n\r\n");

    content.append("Request Handlers\tURL prefixes\r\n");
    for(int i = 0; i < config.static_path_.size(); i++) {
        content.append(config.static_path_[i] + "\t" + config.client_access_static_[i] + "\r\n");
    }
    content.append("/echo\t/echo\r\n");
    content.append("/status\t/status\r\n");

	response->set_header(200);
	response->set_mime("text/plain");
	response->set_length(content.size());
	response->set_content(content);
	return response;
}