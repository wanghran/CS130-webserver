#include <stdio.h>
#include <string>
#include "echoHandler.h"

using namespace std;

echoHandler::echoHandler(const NginxConfig& config, const std::string& root_path) 
{
	this->config = config;
	this->root_path = root_path;
}

echoHandler* echoHandler::create(const NginxConfig& config, const string& root_path)
{
	echoHandler* echo_handler = new echoHandler(config, root_path);
	return echo_handler;
}

std::unique_ptr<reply> echoHandler::HandleRequest(const Request& request)
{
	std::unique_ptr<reply> response(new reply);
	string content;
	std::string s = request.method + " " + request.uri + " HTTP/" + std::to_string(request.versionMajor) + "." + std::to_string(request.versionMinor) + "\r\n";
	content.append(s);
	
	for (auto header: request.headers) {
		content.append(header.name + ": ");
		content.append(header.value);
		content.append("\r\n");
	}
	

	response->set_header(200);
	response->set_mime("text/plain");
	response->set_length(content.size());
	response->set_content(content);
	return response;
}