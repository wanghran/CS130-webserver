#include "logger.h"
#include <iostream>
#include <string>

Logger::Logger(NginxConfig* config)
    : server_config(config)
    {
    }

void Logger::init()
{
    // Construct the file log
    boost::log::add_file_log
    (
        keywords::file_name = "./log/%Y-%m-%d_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format =
        (
            expr::format("[%1%] - %2% - %3% - %4%")
            % expr::attr< boost::posix_time::ptime >("TimeStamp")
            % expr::attr< attrs::current_thread_id::value_type >("ThreadID")
            % logging::trivial::severity
            % expr::smessage
        ),
        keywords::open_mode = std::ios_base::app,
        keywords::auto_flush = true
    );

    // Construct the sink
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

    // Add a stream to write log to
    sink->locked_backend()->add_stream(
        boost::shared_ptr< std::ostream >(&std::clog, boost::null_deleter()));
    
    sink->set_formatter
    (
        expr::format("[%1%] - %2% - %3% - %4%")
            % expr::attr< boost::posix_time::ptime >("TimeStamp")
            % expr::attr< attrs::current_thread_id::value_type >("ThreadID")
            % logging::trivial::severity
            % expr::smessage
    );
    
    // Register the sink in the logging core
    logging::core::get()->add_sink(sink);

    boost::log::add_common_attributes();
}

void Logger::log(logging::trivial::severity_level lvl, const std::string message)
{
    if (server_config->portnum == 80)
    {
        using namespace logging::trivial;
        src::severity_logger< severity_level > lg;
        BOOST_LOG_SEV(lg, lvl) << message;
    }
}

void Logger::log_ip(logging::trivial::severity_level lvl, const std::string message, const std::string ip)
{
    log(lvl, "[" + ip + "] - " + message + "::ResponseMetrics::");
}