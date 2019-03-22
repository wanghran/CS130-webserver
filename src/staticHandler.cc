#include "staticHandler.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

staticHandler::staticHandler(const NginxConfig& config, const std::string& root_path)
{
  this->config = config;
	this->root_path = root_path;
}

staticHandler* staticHandler::create(const NginxConfig& config, const string& root_path)
{
  staticHandler* static_handler = new staticHandler(config, root_path);
  return static_handler;
}

std::unique_ptr<reply> staticHandler::HandleRequest(const Request& request)
{
	file = request.file_path;
  unique_ptr<reply> response(new reply);

  string response_status = "HTTP/1.1 200 OK";
  auto file_type = getFileType();
  string content_type;
  switch (file_type)
  {
    case staticHandler::txt:
      content_type = "text/plain"; 
      break;

    case staticHandler::jpg:
      content_type = "image/jpeg";
      break;

    case staticHandler::zip:
      content_type = "application/zip";
      break;

    case staticHandler::html:
      content_type = "text/html";
      break;

    default:
      response->set_header(404);
      response->set_length(0);
      return response;
      break;
  }
  fstream inputFile(file, ios::binary | ios::in);
  ostringstream ost;
  int size = 0;
  if (inputFile.is_open())
  {
    inputFile.seekg(0, inputFile.end);
    size = inputFile.tellg();
    inputFile.seekg(0, inputFile.beg);
    ost << inputFile.rdbuf();
    inputFile.close();
    }
  else
  {
    response->set_header(404);
    response->set_length(0);
    return response;
  }

  response->set_header(200);
  response->set_mime(content_type);
  response->set_keep_alive(5, 1000);
  response->set_length(size);
  response->set_content(ost.str());

  return response;
}

staticHandler::fileType staticHandler::getFileType()
{
  string file_type = file.substr(file.find_last_of(".\\"));

  if (file_type.compare(".txt") == 0)
  {
    return staticHandler::txt;
  }
  else if (file_type.compare(".jpg") == 0 ||
           file_type.compare(".jpeg") == 0)
  {
    return staticHandler::jpg;
  }
  else if (file_type.compare(".zip") == 0)
  {
    return staticHandler::zip;
  }
  else if (file_type.compare(".html") == 0 ||
           file_type.compare(".htm") == 0)
  {
    return staticHandler::html;
  }
  else
  {
    return staticHandler::unsupported;
  }
}

std::string staticHandler::get_file_path() {
  return file;
}