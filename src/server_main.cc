//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <boost/asio.hpp>
#include "session.h"
#include "server.h"
#include "config_parser.h"
#include "logger.h"

NginxConfig config;

void handler(const boost::system::error_code& error, int signal_number)
{
  if (!error)
  {
    Logger logger(&config);
    logger.log(logging::trivial::info, "Server shutdown by SIGINT(CTRL + C)...");
    exit(0);
  }
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   for(int i = 0; i < argc; i++) {
      std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
   }
   std::cout << std::endl;
   return 0;
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <path to config file>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    NginxConfigParser config_parser;
    bool status = false;
    
    status = config_parser.Parse(argv[1], &config);
    if (!status) {
      throw std::runtime_error("Config parser failed!\n");
    }

    Logger logger(&config);
    logger.init();
    logger.log(logging::trivial::info, "Server initializing...");

    signals.async_wait(handler);

    // Open database
    int rc = sqlite3_open("images.db", &config.db);

    if (rc) {
      std::string err(sqlite3_errmsg(config.db));
      logger.log(logging::trivial::error, "Can't open database: " + err);
      return 0;
    } else {
      logger.log(logging::trivial::info, "Opened database successfully");
    }

    // Create SQL statement
    char *sql;
    sql = (char*)"CREATE TABLE IMAGES("
                  "ID INTEGER PRIMARY KEY     AUTOINCREMENT,"
                  "IMAGE          BLOB        NOT NULL,"
                  "NAME           TEXT        NOT NULL,"
                  "TEXT1          TEXT        NOT NULL,"
                  "TEXT2          TEXT        NOT NULL );";

    // Execute SQL statement
    char *zErrMsg = 0;
    rc = sqlite3_exec(config.db, sql, callback, 0, &zErrMsg);
   
    if (rc != SQLITE_OK) {
      std::string err(zErrMsg);
      logger.log(logging::trivial::warning, "SQL error: " + err);
      sqlite3_free(zErrMsg);
    } else {
      logger.log(logging::trivial::info, "Table created successfully");
    }

    server s(io_service, &config);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
