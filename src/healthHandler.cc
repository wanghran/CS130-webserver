#include "healthHandler.h"
#include <string>

using namespace std;

healthHandler::healthHandler(const NginxConfig &config, const string &root_path)
{
  this->config = config;
  this->root_path = root_path;
}

healthHandler* healthHandler::create(const NginxConfig &config, const string &root_path)
{
  healthHandler *handler = new healthHandler(config, root_path);
  return handler;
}

unique_ptr<reply> healthHandler::HandleRequest(const Request &request)
{
  unique_ptr<reply> response(new reply());
  response->set_header(200);
  string body = "OK";
  response->set_mime("text/plain");
  response->set_length(body.length());
  response->set_content(body);
  return response;
}