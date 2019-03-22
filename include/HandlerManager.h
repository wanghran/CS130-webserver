#ifndef HANDLER_MANAGER_H
#define HANDLER_MANAGER_H

#include <string.h>
#include "config_parser.h"
#include "common_handler.h"
#include "echoHandler.h"
#include "staticHandler.h"
#include "statusHandler.h"
#include "proxyHandler.h"
#include "invalidHandler.h"
#include "formHandler.h"
#include "mimeHandler.h"
#include "healthHandler.h"
#include "malformHandler.h"

/**
 * This is an implementation of handlermanager class.
 * It is responsible for creating all handlers.
*/ 
class HandlerManager
{
    public:

      HandlerManager();

    /**
     * Create a handler by name. 
     * 
     * @param name of the handler.
     * @param NginxConfig object.
     * @param root path of where the handler is registered. 
     * @return the pointer of created handler. 
    */ 
      std::unique_ptr<common_handler> createByName(const std::string& name, 
                                   const NginxConfig& config, 
                                   const std::string& root_path);

    
    protected:
      std::string name;
      NginxConfig config;
      std::string root_path;

};
#endif
