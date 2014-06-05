#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <iostream>
#include "Location.h"

#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>

enum {
    LOG_OFF     = 0,
    LOG_FATAL   = 10,
    LOG_CRITI   = 20,
    LOG_ERROR   = 30,
    LOG_WARN    = 40,
    LOG_INFO    = 50,
    LOG_DEBUG   = 60,
    LOG_VERBOSE = 70,
};

class LogStream
{
public:
    virtual ~LogStream() {}

    virtual bool open() { return true; }
    virtual void write(const std::string& str) = NULL;
    virtual void flush() {}
    virtual void close() {}
};

class ConsoleStream : public LogStream
{
public:
    void write(const std::string& str) {
        std::cout << str << std::endl;
#ifdef _DEBUG
        OutputDebugStringA(str.c_str());
        OutputDebugStringA("\r\n");
#endif
    }
};

class FileLogStream: public LogStream
{
    std::string     path_;
    std::ofstream   ofs_;
public:
    FileLogStream() {}
    FileLogStream(const std::string& path) : path_(path) {}
    virtual ~FileLogStream(){ close(); }

    bool open() { ofs_.open(path_.c_str(), std::ios::app); return ofs_.is_open(); }
    void write(const std::string& str) {
        ofs_ << str << std::endl;
#ifdef _DEBUG
        OutputDebugStringA(str.c_str());
        OutputDebugStringA("\r\n");
#endif
    }
    void close() { if (ofs_.is_open()) ofs_.close(); }
    void flush() { if (ofs_.is_open()) ofs_.flush(); }
};

class LogMessage
{
    Location                    location_;
    boost::posix_time::ptime    time_;
    std::string                 msg_;
    std::string                 level_;

    static std::string          time_format_;
public:
    LogMessage(const Location& location, const std::string& level, const std::string& msg);
    LogMessage(const LogMessage& right);

    static void setTimeFormat(const std::string& format) { time_format_ = format; }

    std::string getTimeString();
    std::string toString();
};

#define LOG_FUNC(fn, level) \
    inline void fn(LogMessage& msg) {   \
        if (level <= level_) {          \
            write(level, msg);          \
        }                               \
    }

class Logger
{
    static Logger                   logger_;

    int                             level_;
    boost::scoped_ptr<LogStream>    strm_;
    boost::asio::io_service         ios_;
    boost::thread                   thread_;

    bool                            running_;

private:
    Logger();
    virtual ~Logger();

public:
    inline void set_level(int level) { level_ = level; }
    inline void set_level(const std::string& level) { level_ = fromLevelString(level); }
    inline void set_stream(LogStream* strm) { strm_.reset(strm); }

    void run();

    LOG_FUNC(Fatal, LOG_FATAL);
    LOG_FUNC(Criti, LOG_CRITI);
    LOG_FUNC(Error, LOG_ERROR);
    LOG_FUNC(Warn, LOG_WARN);
    LOG_FUNC(Info, LOG_INFO);
    LOG_FUNC(Debug, LOG_DEBUG);
    LOG_FUNC(Verbose, LOG_VERBOSE);

    static Logger& get() { return logger_; }
    static int fromLevelString(const std::string& str);
    static std::string toLevelString(int level);

protected:
    void write(int level, LogMessage& msg);
    void do_write(int level, LogMessage& msg);
};

#define FLOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "FATAL", str(msg)))
#define CLOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "CRITI", str(msg)))
#define ELOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "ERROR", str(msg)))
#define WLOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "WARN", str(msg)))
#define ILOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "INFO", str(msg)))
#define DLOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "DEBUG", str(msg)))
#define VLOG(msg)  Logger::get().Fatal(LogMessage(FROM_HERE, "VERB", str(msg)))

#define FLOG_IF(condition, msg) !(condition) ? (void)0 : FLOG(msg)
#define CLOG_IF(condition, msg) !(condition) ? (void)0 : CLOG(msg)
#define ELOG_IF(condition, msg) !(condition) ? (void)0 : ELOG(msg)
#define WLOG_IF(condition, msg) !(condition) ? (void)0 : WLOG(msg)
#define ILOG_IF(condition, msg) !(condition) ? (void)0 : ILOG(msg)
#define DLOG_IF(condition, msg) !(condition) ? (void)0 : DLOG(msg)
#define VLOG_IF(condition, msg) !(condition) ? (void)0 : VLOG(msg)

#define logfmt(x) boost::format(x)

#define LOG_ASSERT(condition) FLOG_IF(!(condition), logfmt("Assert failed: "#condition))
#define CHECK(condition) FLOG_IF(!(condition), logfmt("Check failed: "#condition))

#ifdef _DEBUG
#define DCHECK(condition) CHECK(condition)
#endif

#endif