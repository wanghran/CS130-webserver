#ifndef HEADER_BUILDER_H
#define HEADER_BUILDER_H
#include <memory>
#include "header.h"

//Do Not Use This Class
//Do Not Use This Class
//Do Not Use This Class
class header_builder
{
  private:
    header* header_line = new header();
    header_builder(const header_builder& hb);
    header_builder& operator=(const header_builder&) = delete;
    struct method_flag
    {
      bool status = false;
      bool content_type = false;
      bool content_length = false;
      bool connection_close = false;
      bool give_header = false;
    };
    method_flag mf;

  public:
    header_builder();
    ~header_builder();
    header_builder add_status(int status_code);
    header_builder add_content_type(std::string type);
    header_builder add_content_length(int length);
    header_builder add_connection_close();
    header* build();
};
#endif