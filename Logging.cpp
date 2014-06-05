#include "Logging.h"

std::string LogMessage::time_format_("%Y-%m-%d %H:%M:%S.%f");

LogMessage::LogMessage(const Location &location, const std::string& level, const std::string &msg)
: location_(location), level_(level), msg_(msg)
{
    time_ = boost::posix_time::microsec_clock::local_time();
}

LogMessage::LogMessage(const LogMessage& right)
{
    location_   = right.location_;
    level_      = right.level_;
    msg_        = right.msg_;
    time_       = right.time_;
}

std::string LogMessage::getTimeString()
{
    boost::posix_time::time_facet* tf = new boost::posix_time::time_facet(time_format_.c_str());
    std::stringstream ss;
    ss.imbue(std::locale(std::locale::classic(), tf));
    ss << time_;
    return ss.str();
}

std::string LogMessage::toString()
{
    std::string tm_str = getTimeString();

    return tm_str + " " + location_.toString() + " " + level_ + " " + msg_;
}

Logger Logger::logger_;

int Logger::fromLevelString(const std::string& str)
{
    if (str == "verbose")
        return LOG_VERBOSE;
    else if (str == "debug")
        return LOG_DEBUG;
    else if (str == "info")
        return LOG_INFO;
    else if (str == "warn")
        return LOG_WARN;
    else if (str == "error")
        return LOG_ERROR;
    else if (str == "criti")
        return LOG_CRITI;
    else if (str == "fatal")
        return LOG_FATAL;
    else if (str == "off")
        return LOG_OFF;
    else
        return LOG_INFO;
}

std::string Logger::toLevelString(int level)
{
    if (level <= LOG_OFF)
        return "off";
    else if (level <= LOG_FATAL)
        return "fatal";
    else if (level <= LOG_CRITI)
        return "criti";
    else if (level <= LOG_ERROR)
        return "error";
    else if (level <= LOG_WARN)
        return "warn";
    else if (level <= LOG_INFO)
        return "info";
    else if (level <= LOG_DEBUG)
        return "debug";
    else if (level <= LOG_VERBOSE)
        return "verbose";
    else
        return "unknown";
}

Logger::Logger() : level_(LOG_INFO), running_(false)
{
    thread_ = boost::thread(boost::bind(&Logger::run, this));
}

Logger::~Logger()
{
    running_ = false;
    thread_.join();
}

void Logger::run()
{
    ILOG(logfmt("logger thread(%1%)") % boost::this_thread::get_id());

    running_ = true;
    while (running_)
    {
        ios_.poll();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
    ios_.poll();
    ios_.run();
}

void Logger::write(int level, LogMessage& msg)
{
    ios_.post(boost::bind(&Logger::do_write, this, level, msg));
}

void Logger::do_write(int level, LogMessage& msg)
{
    if (strm_) {
        strm_->open();
        strm_->write(msg.toString());
        strm_->close();
    }
}
