#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <cstdarg>
#include <memory>
#include <chrono>

enum class LogLevel
{
    FATAL = 0,   // printed and stops program
    DEBUG = 1,
    WARNING = 2,
    INFO = 3,
    VERBOSE = 4,
};

class AbstractLogger
{
    public:
        AbstractLogger(std::ostream* target = &std::cerr) : out(target), log_level(LogLevel::INFO), log_time(true) {}
        virtual ~AbstractLogger() {}
        virtual void Log(LogLevel level, const char* format, va_list ap);
        virtual void DoLog(LogLevel level, const char* format, va_list ap) {}
        void SetLogTarget(std::ostream* stream) {out = stream;}
        void SetLogLevel(LogLevel level) {log_level = level;}
        void SetLogTime(bool logging_time) {log_time = logging_time;}
    protected:
        std::ostream* out;
        LogLevel log_level;
        bool log_time;
};

class Logger : public AbstractLogger
{
    public:
        virtual void DoLog(LogLevel level, const char* format, va_list ap);
};

extern std::unique_ptr<AbstractLogger> logger;

void SetLogger(AbstractLogger* logobj);

static inline void Log(LogLevel level, const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    logger->Log(level, format, argptr);
    va_end(argptr);
}

static inline void Log(LogLevel level, const char* format, va_list arg)
{
    logger->Log(level, format, arg);
}

static inline void FatalLog(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::FATAL, format, argptr);
    va_end(argptr);
}

static inline void DebugLog(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::DEBUG, format, argptr);
    va_end(argptr);
}

static inline void WarnLog(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::WARNING, format, argptr);
    va_end(argptr);
}

static inline void InfoLog(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::INFO, format, argptr);
    va_end(argptr);
}

static inline void VerboseLog(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::VERBOSE, format, argptr);
    va_end(argptr);
}

/** Object that only exists to print out start and end of event call in a function */
class EventLog
{
    public:
        EventLog(const char* function);
        ~EventLog();
    private:
        const char* func;
        std::chrono::time_point<std::chrono::system_clock> startTime;
};

#endif
