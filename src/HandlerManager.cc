#include "HandlerManager.h"

HandlerManager::HandlerManager() {}

std::unique_ptr<common_handler> HandlerManager::createByName(const std::string& name, 
                                             const NginxConfig& config, 
                                             const std::string& root_path)
{
    int name_code = 0;
    if (name == "ECHO") {
        name_code = 1;
    }
    else if (name == "STATIC") {
        name_code = 2;
    }
    else if (name == "STATUS") {
        name_code = 3;
    }
    else if (name == "PROXY") {
	    name_code = 4;
    }
    else if (name == "FORM") {
        name_code = 5;
    }
    else if (name == "MIME") {
        name_code = 6;
    }
    else if (name == "HEALTH") {
        name_code = 7;
    }
    else if (name == "MALFORM") {
        name_code = 8;
    }
    switch(name_code) {
        case 1: {
            std::unique_ptr<common_handler> echo_handler(echoHandler::create(config, root_path));
            return echo_handler;
        }
        case 2:  {
            std::unique_ptr<common_handler> static_handler(staticHandler::create(config, root_path));
            return static_handler;
        }
        case 3: {
            std::unique_ptr<common_handler> status_handler(statusHandler::create(config, root_path));
            return status_handler;
        }
	    case 4: {
	        std::unique_ptr<common_handler> proxy_handler(proxyHandler::create(config, root_path));
	        return proxy_handler;
	    }
        case 5: {
            std::unique_ptr<common_handler> form_handler(formHandler::create(config, root_path));
	        return form_handler;
        }
        case 6: {
            std::unique_ptr<common_handler> mime_handler(mimeHandler::create(config, root_path));
	        return mime_handler;
        }
        case 7: {
            std::unique_ptr<common_handler> health_handler(healthHandler::create(config, root_path));
	        return health_handler;
        }
        case 8: {
            std::unique_ptr<common_handler> malform_handler(malformHandler::create(config, root_path));
            return malform_handler;
        }
        default: {
            std::unique_ptr<common_handler> invalid_handler(invalidHandler::create(config, root_path));
            return invalid_handler;
        }
    }
}
