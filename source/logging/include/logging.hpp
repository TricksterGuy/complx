#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <chrono>
#include <cstdarg>
#include <functional>
#include <iostream>
#include <istream>
#include <memory>

enum class LogLevel
{
    FATAL = 0,   // printed and stops program
    DEBUG = 1,   // printed in debug builds
    WARNING = 2,
    INFO = 3,
    VERBOSE = 4,
};

class AbstractLogger
{
public:
    explicit AbstractLogger(std::reference_wrapper<std::ostream> target = std::ref(std::cerr)) : out(target) {}
    virtual ~AbstractLogger() = default;
    virtual void Log(LogLevel level, const char* format, va_list ap);
    virtual void DoLog(LogLevel level, const char* format, va_list ap);
    void SetLogTarget(std::reference_wrapper<std::ostream> stream) { out = stream; }
    void SetLogLevel(LogLevel level) { log_level = level; }
    void SetLogTime(bool logging_time) { log_time = logging_time; }
    void SetLogColor(bool logging_color) { log_color = logging_color; }
    const std::ostream& GetLogTarget() const { return out; }
    LogLevel GetLogLevel() const { return log_level; }
    bool GetLogTime() const { return log_time; }
protected:
    std::reference_wrapper<std::ostream> out;
    LogLevel log_level = LogLevel::INFO;
    bool log_time = true;
    bool log_color = true;
};

class Logger : public AbstractLogger
{
public:
    void DoLog(LogLevel level, const char* format, va_list ap) override;
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

static inline void DFatalLog(const char* format, ...)
{
#ifndef NDEBUG
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::FATAL, format, argptr);
    va_end(argptr);
#else
    // unused
    (void)format;
#endif
}

static inline void DebugLog(const char* format, ...)
{
#ifndef NDEBUG
    va_list argptr;
    va_start(argptr, format);
    Log(LogLevel::DEBUG, format, argptr);
    va_end(argptr);
#else
    // unused
    (void)format;
#endif
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
    explicit EventLog(const char* function);
    ~EventLog();
private:
    const char* func;
    std::chrono::time_point<std::chrono::system_clock> startTime;
};

#endif
