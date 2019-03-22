#ifndef MIME_HANDLER_H
#define MIME_HANDLER_H

#include "common_handler.h"

class mimeHandler : public common_handler
{
  public:
    mimeHandler(const NginxConfig &config, const std::string &root_path);
    static mimeHandler *create(const NginxConfig &config, const std::string &root_path);
    std::unique_ptr<reply> HandleRequest(const Request &request);

    /*
    Make it public for unit tests
      flag = 0: normal,
      flag = 1: 200 test,
      flag = -1: 404 test,
      flag = -2: 503 test,
    */
    std::unique_ptr<reply> view(const Request &request, int flag);

  private:
    NginxConfig config;
    std::string root_path;
    std::string top = "";
    std::string bot = "";
    std::string image = "";
    std::string name = "";
    int read_db(int id);
    int get_last_id();
    std::unique_ptr<reply> list(const Request &request);
    std::unique_ptr<reply> edit(const Request &request);
    std::unique_ptr<reply> del(const Request &request);
    void encode(std::string &page);

    struct Item
    {
      int id;
      std::string name;
      std::string top;
      std::string bot;
      std::string image;
    };
    
    
};
#endif