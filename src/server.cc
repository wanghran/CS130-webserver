#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "server.h"
#include "logger.h"

server::server(boost::asio::io_service& io_service, NginxConfig* config)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), config->portnum)),
      server_config(config),
      server_root(config->root_path),
      logger(config)
  {
    start_accept();
  }

void server::start_accept()
  {
    session* new_session = new session(io_service_, server_config);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

void server::handle_accept(session* new_session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start(server_config);
      std::string ip = new_session->socket().remote_endpoint().address().to_string();
      logger.log_ip(logging::trivial::info, "New session starting...", ip);
    }
    else
    {
      logger.log(logging::trivial::error, "server::handle_accept failed: " + error.message());
      delete new_session;
    }

    start_accept();
  }