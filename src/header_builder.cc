#include "header_builder.h"

header_builder::header_builder() {}
header_builder::~header_builder() {}

header_builder::header_builder(const header_builder &hb)
{
  header_line = hb.header_line;
  mf = hb.mf;
}
header_builder header_builder::add_status(int status_code)
{
  if (mf.status)
  {
    return *this;
  }
  header_line -> add_status(status_code);
  mf.status = true;
  return *this;
}

header_builder header_builder::add_connection_close()
{
  if (mf.connection_close)
  {
    return *this;
  }
  header_line -> add_connection_close();
  mf.connection_close = true;
  return *this;
}

header_builder header_builder::add_content_length(int length)
{
  if (mf.content_length)
  {
    return *this;
  }
  header_line -> add_content_length(length);
  mf.content_length = true;
  return *this;
}

header_builder header_builder::add_content_type(std::string type)
{
  if (mf.content_type)
  {
    return *this;
  }
  header_line -> add_content_type(type);
  mf.content_type = true;
  return *this;
}

header* header_builder::build()
{
  return header_line;
}