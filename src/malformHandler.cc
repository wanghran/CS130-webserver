#include "malformHandler.h"

using namespace std;

malformHandler::malformHandler(const NginxConfig &config, const std::string &root_path)
{
  this->config = config;
  this->root_path = root_path;
}

malformHandler* malformHandler::create(const NginxConfig &config, const std::string &root_path)
{
  malformHandler* handler = new malformHandler(config, root_path);
  return handler;
}

unique_ptr<reply> malformHandler::HandleRequest(const Request &request)
{
  unique_ptr<reply> response(new reply());
  response->set_header(400);
  response->set_length(0);
  return response;
}