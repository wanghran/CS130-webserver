#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "session.h"
#include "config_parser.h"
#include "logger.h"

using boost::asio::ip::tcp;

class server
{
public:
  server(boost::asio::io_service& io_service, NginxConfig* config);
  std::string server_root;
  NginxConfig* server_config;

private:
  void start_accept();

  void handle_accept(session* new_session,
      const boost::system::error_code& error);

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  Logger logger;
};

#endif