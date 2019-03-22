#include "UrlProcessor.h"
#include "logger.h"
#include <boost/lexical_cast.hpp>

UrlProcessor::UrlProcessor(NginxConfig* config)
    : logger(config)
    {
    }

bool UrlProcessor::checkExist(std::string filepath)
{
  std::string file = filepath;
  std::ifstream f(file);
  return f.good();
}

bool UrlProcessor::handle_url(Request& req)
{
    std::string input = req.uri;
    bool err = false;
    if(parser.parse(input)) {
        req.uri = parser.path();
        req.query = parser.query();
        path_status = check_dir(parser.path());
        if(path_status == UrlProcessor::STATIC_)
        {
            filename = get_filename(parser.path());
            if(getTargetPath(parser.path(), filename) && checkExist(target_static_path))
            {
                req.file_name = filename;
                req.file_path = target_static_path;
                logger.log(logging::trivial::info, filename + " exists");
            }
            else
            {
                logger.log(logging::trivial::warning, target_static_path + " does not exist");
                err = true;
            }
        }
        else if(path_status == UrlProcessor::PROXY_) {
            // Get the root
            getTargetProxy(parser.path());
            req.file_path = target_proxy_path;

            // Get full path of file
            int rootPos = parser.path().find('/', 1);
            if (rootPos == std::string::npos) {
                req.file_name = "/";
            }
            else {
                req.file_name = parser.path().substr(rootPos);
            }

            // Get port
            req.proxy_port = target_proxy_port;

            logger.log(logging::trivial::info, "Obtaining object " + req.file_name + " from " + req.file_path + " at port " + req.proxy_port);
        }
        else if (path_status == UrlProcessor::MIME_) {
            int idPos = input.find('?', 1);
            if (idPos == std::string::npos) {
                req.file_name = "/";
            }
            else if (req.uri == edit_path) {
                req.file_name = input.substr(idPos+7);
            }
            else {
                req.file_name = input.substr(idPos+3);
            }
            std::size_t found = req.uri.find("/mime/view");
            if (found != std::string::npos)
            {
                try
                {
                    int id = boost::lexical_cast<int>(req.file_name.substr(1));
                    logger.log(logging::trivial::info, "Obtaining MEME ID: " + req.file_name);
                }
                catch (boost::bad_lexical_cast)
                {
                    path_status = UrlProcessor::BAD_REQUEST_;
                    logger.log(logging::trivial::warning, "Invalid MEME ID");
                }
            }
        }
        else if(path_status == UrlProcessor::INVALID_)
        {
            logger.log(logging::trivial::warning, "Invalid path: " + parser.path());
            err = true;
        }
    }
    else 
    {
        logger.log(logging::trivial::error, "URL parsing error");
        err = true;
    }

    if(err)
    {
        return false;
    }

    return true;
}

/*
There is a serious bug that we should fix later.

For example, if the location of static files is /s, then /status will be matched to STATIC_ 
    because the value of the second argument of compare() is 2.

We should avoid setting the location static files to have the same prefix as the /status.
    But this is not a good approach, we should fix this later!
*/
UrlProcessor::RequestContent UrlProcessor::check_dir(std::string path)
{
    for(int i = 0; i < client_access_static_.size() ; i++)
    {
        if(!path.compare(0, client_access_static_[i].size(), client_access_static_[i]))
        {
            return UrlProcessor::STATIC_;
        }
    }
    for(int i = 0; i < client_access_proxy_.size() ; i++)
    {
	if(!path.compare(0, client_access_proxy_[i].size(), client_access_proxy_[i]))
	{
	    return UrlProcessor::PROXY_;
	}
    }

    if(!path.compare(0, echo_path.size(), echo_path))
    {
        return UrlProcessor::ECHO_;
    }
    if(!path.compare(0, stat_path.size(), stat_path))
    {
        return UrlProcessor::STATUS_;
    }
    if(!path.compare(0, upload_path.size(), upload_path) || !path.compare(0, list_path.size(), list_path) || !path.compare(0, edit_path.size(), edit_path) || !path.compare(0, delete_path.size(), delete_path))
    {
        return UrlProcessor::MIME_;
    }
    if(!path.compare(0, health_path.size(), health_path))
    {
        return UrlProcessor::HEALTH_;
    }

    return UrlProcessor::INVALID_;
}

bool UrlProcessor::getTargetPath(std::string path, std::string filename)
{
    int i = 0;
    int max_length = 0;
    std::string candidate_path = "";
    //for(auto it = static_path_.begin(); it != static_path_.end();it++)
    for(int i = 0; i < client_access_static_.size() ; i++)
    {
        if(!path.compare(0, client_access_static_[i].size(), client_access_static_[i]))
        {
            if (candidate_path == "") {
                candidate_path = static_path_[i];
            }
            else if (static_path_[i].size() > candidate_path.size()) {
                candidate_path = static_path_[i];
            }
        }
        /*
        if(path.compare(0, static_path_[i].size(), static_path_[i]) == 0)
        {
            return &(static_handler_.at(i));
        }
        i++;
        */
    }
    if (candidate_path == "") {
        return false;
    }
    else {
        target_static_path = root_path + candidate_path + filename;
        return true;
    }
}

bool UrlProcessor::getTargetProxy(std::string path)
{
    for(int i = 0; i < client_access_proxy_.size() ; i++)
    {
	// Add a slash to end of address if not there

	if((path + "/").find(client_access_proxy_[i] + "/") != std::string::npos)
	{
             logger.log(logging::trivial::info, "OK");
	     // Need to get rid of last slash of path
             target_proxy_path =  proxy_path_[i];
	     target_proxy_port = proxy_port_[i];
	     return true;
        }
    }
    target_proxy_path = "";
    return false;
}


void UrlProcessor::setup(NginxConfig* server_config)
{
    root_path = server_config->root_path;
    echo_path = server_config->echo_path;
    stat_path = server_config->status_path;
    upload_path = server_config->upload_path;
    list_path = server_config->list_path;
    health_path = "/health";
    edit_path = "/mime/new";
    delete_path = "/mime/delete";
    int i = 0;
    for(auto it = server_config->static_path_.begin(); it != server_config->static_path_.end();it++)
    {
      static_path_.push_back(server_config->static_path_.at(i));
      client_access_static_.push_back(server_config->client_access_static_.at(i));
    //   static_handler_.push_back(staticHandler(server_config->client_access_static_.at(i), server_config->root_path + server_config->static_path_.at(i)));
      i++;
    }
    i = 0;
    for(auto it = server_config->proxy_path_.begin(); it != server_config->proxy_path_.end();it++)
    {
	proxy_path_.push_back(server_config->proxy_path_.at(i));
	client_access_proxy_.push_back(server_config->client_access_proxy_.at(i));
	proxy_port_.push_back(server_config->proxy_port_.at(i));
	i++;
    }
}

std::string UrlProcessor::get_filename(std::string const & path)
{
   return path.substr(path.find_last_of("/\\"));
}

std::string UrlProcessor::give_filename()
{
    return filename;
}

UrlProcessor::RequestContent UrlProcessor::give_RequestContent()
{
    return path_status;
}
