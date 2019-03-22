#include "header.h"

header::header() {}

void header::add_status(int status_code)
{
  switch (status_code)
  {
    case OK:
      head_file.append(OK_status);
      break;
  
    case BAD_REQUEST:
      head_file.append(Bad_request);
      break;

    case SERVICE_UNAVAILABLE:
      head_file.append(Service_unavailable);
      break;

    default:
      head_file.append(not_found_status);
      break;
  }
}

void header::add_content_length(int length)
{
  head_file.append(content_length);
  head_file.append(std::to_string(length));
  head_file.append("\r\n");
}

void header::add_connection_close()
{
  head_file.append(connection_close);
}

void header::add_content_type(std::string type)
{
  head_file.append(content_type);
  head_file.append(type);
  head_file.append("\r\n");
}

void header::add_binary_encoding()
{
  head_file.append(binary_encoding);
}

void header::add_connect_alive(int timeout, int max)
{
  head_file.append(connection_keep_alive);
  head_file.append(keep_alive);
  std::string OT = "timeout=" + std::to_string(timeout);
  std::string MX = "max=" + std::to_string(max);
  head_file.append(OT);
  head_file.append(", ");
  head_file.append(MX);
  head_file.append("\r\n");
}

std::string header::give_header()
{
  if (head_file.substr(head_file.length() - 2) == "\r\n")
  {
    head_file.append("\r\n");
  }
  else
  {
    head_file.append("\r\n\r\n");
  }
  return head_file;
}