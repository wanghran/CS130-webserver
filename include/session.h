#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "DataProcessor.h"
#include "UrlProcessor.h"
#include "logger.h"

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, NginxConfig* config);

  tcp::socket& socket();

  void start(NginxConfig* config);

  NginxConfig* server_config;

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);

  void map_insert(std::unordered_map<std::string, int> &m, std::string k);

  tcp::socket socket_;

  enum { max_length = 1024 };

  DataProcessor dataProcessor;

  UrlProcessor urlProcessor;

  Logger logger;

  std::string response;
};
#endif