#ifndef FORM_HANDLER_H
#define FORM_HANDLER_H

#include <string>
#include <sqlite3.h>
#include <fstream>
#include "common_handler.h"
#include "base64_rfc4648.h"

class formHandler : public common_handler
{
  public:
    formHandler(const NginxConfig& config, const std::string& root_path);

    static formHandler* create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<reply> HandleRequest(const Request& request);

    private:
      void get_form_content(const Request* req, std::string* top, std::string* bottom, std::string* image, std::string* name);
      int insert_table(std::string &image, std::string name, std::string top, std::string bottom);
      int update_table(std::string &image, std::string name, std::string top, std::string bottom);
      int check_content_type(std::string* content);
      NginxConfig config;
      std::string root_path;
      //void encode(std::string &page);
      int last_id;
      int curr_id;
};
#endif