#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sqlite3.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "request.h"
#include "httprequestparser.h"
#include "reply.h"
#include "HandlerManager.h"

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service, NginxConfig* config)
    : socket_(io_service),
      urlProcessor(config),
      logger(config)
  {
  }

tcp::socket& session::socket()
  {
    return socket_;
  }

void session::start(NginxConfig* config)
  {
    server_config = config;
    socket_.async_read_some(boost::asio::buffer(dataProcessor.data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    std::string ip = socket_.remote_endpoint().address().to_string();
    // Append the current received data to the buffer
    logger.log_ip(logging::trivial::info, "Received message: \n" + std::string(dataProcessor.data_, bytes_transferred), ip);
    dataProcessor.copyData(bytes_transferred);

    Request req;

    bool echo = false;
    bool stac = false;
    bool stat = false;
    bool prox = false;
    bool post = false;
    bool status = false;
    bool mime = false;
    bool health = false;
    bool bad_request = false;

    // Parse the buffer to check if it is a valid HTTP request
    bool parse_flag = true;
    httpparser::HttpRequestParser::ParseResult result = dataProcessor.parseHttpRequest(&req);
    logger.log_ip(logging::trivial::info, "Parsing message...", ip);

    if (result == httpparser::HttpRequestParser::ParsingIncompleted) {
      if (bytes_transferred == 0) {
        logger.log_ip(logging::trivial::warning, "Parsing incompleted. Received EOF. Disconnecting...", ip);
        delete this;
      } else {
        start(server_config);
        logger.log_ip(logging::trivial::info, "Parsing incompleted. Waiting for new characters...", ip);
      }
      return;
    }
    else if (result == httpparser::HttpRequestParser::ParsingError)
    {
      logger.log_ip(logging::trivial::warning, "Parsing completed. Not a HTTP request. Disconnecting...", ip);
      parse_flag = false;

      // For the integration test
      if (server_config->portnum != 80) {
        std::cerr << "Error! Not a HTTP Request!" << std::endl;
      }
    }
    else 
    {
      req.request_uri = req.uri;
      logger.log_ip(logging::trivial::info, "Parsing completed. Checking the HTTP request...", ip);

      if(req.method == "POST") {
        post = true;
      }

      std::string host;
      if(dataProcessor.check_request(&req, &host))
      {
        req.uri = dataProcessor.construct_url(&req, host);
        map_insert(server_config->num_url_requested, req.uri);
        logger.log_ip(logging::trivial::info, "Host found. The request URI: " + req.uri, ip);
      }

      urlProcessor.setup(server_config);

      logger.log_ip(logging::trivial::info, "Parsing the request URI: " + req.uri, ip);
      if(!post){
        status = urlProcessor.handle_url(req);
      }

      if (status)
      {
        if (urlProcessor.give_RequestContent() == UrlProcessor::ECHO_)
        {
          logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for the echo path", ip);
          std::string content(dataProcessor.all_data_.begin(), dataProcessor.all_data_.end());
          //echo_handler.getContent(content);
          echo = true;
        }
        else if (urlProcessor.give_RequestContent() == UrlProcessor::STATIC_)
        {
          logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for the static path", ip);
          //std::string filedir = server_config->root_path + urlProcessor.parser.path();
          //urlProcessor.current->getContent(filedir);
          stac = true;
        }
        else if (urlProcessor.give_RequestContent() == UrlProcessor::STATUS_)
        {
          logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for the status path", ip);
          stat = true;
        }
	      else if (urlProcessor.give_RequestContent() == UrlProcessor::PROXY_)
	      {
	        logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for the proxy path", ip);
  	      prox = true;
	      }
        else if (urlProcessor.give_RequestContent() == UrlProcessor::MIME_)
	      {
	        logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for the MIME path", ip);
  	      mime = true;
	      }
        else if (urlProcessor.give_RequestContent() == UrlProcessor::HEALTH_)
        {
          logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for server health check", ip);
          health = true;
        }
        else if (urlProcessor.give_RequestContent() == UrlProcessor::BAD_REQUEST_)
        {
          logger.log_ip(logging::trivial::info, "Request URI parsing completed. Received a request for MEME but the ID is invalid", ip);
          bad_request = true;
        }
      }
    }

    if (!error && parse_flag)
    {
      logger.log_ip(logging::trivial::info, "Constructing a HTTP response...", ip);
      HandlerManager handler_manager;

      if (echo)
      {
        std::unique_ptr<reply> res = handler_manager.createByName("ECHO", *server_config, server_config->root_path)->HandleRequest(req);
        map_insert(server_config->num_code_returned, std::to_string(res->get_status_code()));
        response = res->give_reply();
        dataProcessor.cleanData();
        echo = false;

        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent message: \n" + response, ip);
      }
      else if (stac)
      {
        std::unique_ptr<reply> res = handler_manager.createByName("STATIC", *server_config, server_config->root_path)->HandleRequest(req);
        map_insert(server_config->num_code_returned, std::to_string(res->get_status_code()));
        response = res->give_reply();
        dataProcessor.cleanData();
        stac = false;

        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent a file: " + urlProcessor.give_filename(), ip);
      }
      else if (stat)
      {
        std::unique_ptr<reply> res = handler_manager.createByName("STATUS", *server_config, server_config->root_path)->HandleRequest(req);
        map_insert(server_config->num_code_returned, std::to_string(res->get_status_code()));
        response = res->give_reply();
        dataProcessor.cleanData();
        stat = false;

        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent message: \n" + response, ip);
      }
      else if (prox) {
        std::unique_ptr<reply> res = handler_manager.createByName("PROXY", *server_config, server_config->root_path)->HandleRequest(req);
        map_insert(server_config->num_code_returned, std::to_string(res->get_status_code()));
        response = res->give_reply();
        dataProcessor.cleanData();
        prox = false;

        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent response: \n" + res->give_header(), ip);

      }
      else if (post) {
        std::unique_ptr<reply> res = handler_manager.createByName("FORM", *server_config, server_config->root_path)->HandleRequest(req);
        response = res->give_reply();
        post = false;
        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent response: \n" + res->give_header(), ip);
      }
      else if (mime) {
        std::unique_ptr<reply> res = handler_manager.createByName("MIME", *server_config, server_config->root_path)->HandleRequest(req);
        response = res->give_reply();
        mime = false;
        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent response: \n" + res->give_header(), ip);
      }
      else if (health) {
        std::unique_ptr<reply> res = handler_manager.createByName("HEALTH", *server_config, server_config->root_path)->HandleRequest(req);
        response = res->give_reply();
        health = false;
        logger.log_ip(logging::trivial::info, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::info, "Sent response: \n" + res->give_header(), ip);
      }
      else if (bad_request) {
        std::unique_ptr<reply> res = handler_manager.createByName("MALFORM", *server_config, server_config->root_path)->HandleRequest(req);
        response = res->give_reply();
        bad_request = false;
        logger.log_ip(logging::trivial::warning, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::warning, "Sent response: \n" + res->give_header(), ip);
      }
      else
      {
        std::unique_ptr<reply> res = handler_manager.createByName("INVALID", *server_config, server_config->root_path)->HandleRequest(req);
        map_insert(server_config->num_code_returned, std::to_string(res->get_status_code()));
        response = res->give_reply();
        dataProcessor.cleanData();
        logger.log_ip(logging::trivial::warning, "Response code: " + std::to_string(res->get_status_code()), ip);
        logger.log_ip(logging::trivial::warning, "Sent message: \n" + response, ip);
      }

      boost::asio::async_write(socket_,
                               boost::asio::buffer(response),
                               boost::bind(&session::handle_write, this,
                                           boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }

void session::handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(dataProcessor.data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

void session::map_insert(std::unordered_map<std::string, int> &m, std::string k)
{
  if(m.find(k) == m.end()) {
    m[k] = 1;
  } else {
    m[k] += 1;
  }
}
