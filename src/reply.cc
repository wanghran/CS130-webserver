#include "reply.h"

reply::reply() {}

void reply::set_header(int status_code)
{
  stat_code = status_code;
  header_line.add_status(status_code);
}

void reply::set_content(std::string content)
{
  this->content = content;
}

void reply::set_error_code(int error_code)
{
  this->error_code = error_code;
}

void reply::set_length(int length)
{
  this->length = length;
}

void reply::set_mime(std::string type)
{
  MIME_type = type;
}

void reply::set_keep_alive(int timeout, int max)
{
  ka.timeout = timeout;
  ka.max = max;
}

std::string reply::give_reply()
{
  if(!MIME_type.empty())
  {
    header_line.add_content_type(MIME_type);
  }
  header_line.add_content_length(length);
  header_line.add_binary_encoding();
  if(ka.timeout != 0 || ka.max != 0)
  {
    header_line.add_connect_alive(ka.timeout, ka.max);
  }
  else
  {
    header_line.add_connection_close();
  }
  return header_line.give_header() + content;
}

std::string reply::give_content()
{
  return content;
}

std::string reply::give_header()
{
  return header_line.give_header();
}

int reply::get_status_code()
{
  return stat_code;
}
