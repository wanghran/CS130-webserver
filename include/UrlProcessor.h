#ifndef URL_PROCESSOR_H
#define URL_PROCESSOR_H

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "config_parser.h"
#include <algorithm>
#include <vector>
//#include "staticHandler.h"
#include "urlparser.h"
#include "request.h"
#include "logger.h"

/**
 * This is a utility class used to parse url and determine which handler to use.
*/ 
class UrlProcessor
{
public:
    UrlProcessor(NginxConfig* config);

/**
 * This is a enum of the status of url of request.
*/ 
    enum RequestContent{
        ECHO_ = 0,
        STATIC_ = 1,
        INVALID_ = 2,
        STATUS_ = 3,
	    PROXY_ = 4,
        MIME_ = 5,
        HEALTH_ = 6,
        BAD_REQUEST_ = 7
    };

    /**
     * Check if a file exists.
     * 
     * @param Name of the file to be checked.
     * @return Ture if file exists, otherwise False.
    */ 
    bool checkExist(std::string filename);

    /**
     * Try to parse a url and set all parameters.
     * Should Call This Method Before You Create Handlers
     * 
     * @param Request Object
     * @return Ture if the url is valid, otherwise False.
    */ 
    bool handle_url(Request& req);

    /**
     * Check if the request path is valid. 
     * Should Call This Method Before You Call CheckExist and Create Handlers
     * 
     * @param a parsed path.
     * @return The status of path. It could be static, echo, status, and invalid.
    */ 
    RequestContent check_dir(std::string path);

    /**
     * Set valid paths, i.e. static, echo, status, root. 
     * 
     * @param NginxConfig object created by NginxConfigParser.
    */ 
    void setup(NginxConfig* server_config);

    /**
     * Check if a static path exists and set target_static_path.
     * 
     * @param a parsed path.
     * @param a filename. 
     * @return Ture if static path exists.
    */ 
    bool getTargetPath(std::string path, std::string filename);

    /**
     * Set target_proxy_path.
     * @param a parsed path.
     * @param a filename.
     * @return True if path is able to be constructed.
    */
    bool getTargetProxy(std::string path);

    /**
     * Parse a filename from a path.
     * 
     * @param a parsed path with filename.
     * @return Parsed filename, e.g. /1.txt
    */ 
    std::string get_filename(std::string const & path);

    httpparser::UrlParser parser;

    /**
     * Get private member: filename.
     * 
     * @return filename
    */ 
    std::string give_filename();

    /**
     * Get private member: path_status.
     * 
     * @return path_status
    */
    RequestContent give_RequestContent();

  private:
    std::string root_path;
    std::string echo_path;
    std::string stat_path;
    std::string upload_path;
    std::string list_path;
    std::string health_path;
    std::string edit_path;
    std::string delete_path;
    std::vector<std::string> static_path_;
    std::vector<std::string> client_access_static_;
    //std::vector<staticHandler> static_handler_;
    std::vector<std::string> proxy_path_;
    std::vector<std::string> client_access_proxy_;
    std::vector<std::string> proxy_port_;
    RequestContent path_status;
    std::string target_static_path;
    std::string target_proxy_path;
    std::string target_proxy_port;
    std::string filename;
    Logger logger;
};
#endif
