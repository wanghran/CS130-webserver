#ifndef LOGGER_H
#define LOGGER_H

#define BOOST_THREAD_USE_LIB 1

#include <string>
#include <fstream>
#include <istream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/asio/signal_set.hpp>
#include "config_parser.h"

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace src = boost::log::sources;
namespace attrs = boost::log::attributes;

class Logger {
    public:
        Logger(NginxConfig* config);
        void init();
        void log(logging::trivial::severity_level lvl, const std::string message);
        void log_ip(logging::trivial::severity_level lvl, const std::string message, const std::string ip);

    private:
        NginxConfig* server_config;
};

#endif