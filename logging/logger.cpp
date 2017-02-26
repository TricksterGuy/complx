#include "logger.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>

std::string time_to_string(std::chrono::system_clock::time_point& time_point)
{
    std::time_t time_secs = std::chrono::system_clock::to_time_t(time_point);
    std::chrono::system_clock::time_point time_point_sec = std::chrono::system_clock::from_time_t(time_secs);
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point - time_point_sec);

    char buffer[128];
    strftime(buffer, 128, "%H:%M:%S", localtime(&time_secs));
    char currentTime[128] = "";
    snprintf(currentTime, 128, "%s.%03d", buffer, (int)ms.count());

    return currentTime;
}

std::unique_ptr<AbstractLogger> logger(new Logger());

void SetLogger(AbstractLogger* logobj)
{
    logger.reset(logobj);
}

inline const char* GetLogAbbrev(LogLevel level)
{
    switch(level)
    {
    case LogLevel::FATAL:
        return "F";
    case LogLevel::DEBUG:
        return "D";
    case LogLevel::WARNING:
        return "W";
    case LogLevel::INFO:
        return "I";
    case LogLevel::VERBOSE:
        return "V";
    default:
        return "?";
    }
}

inline const char* GetLogColor(LogLevel level)
{
#ifndef WINDOWS
    switch(level)
    {
    case LogLevel::FATAL:
        return "\033[1;31m";
    case LogLevel::DEBUG:
        return "\033[1;33m";
    case LogLevel::WARNING:
        return "\033[1;33m";
    case LogLevel::INFO:
        return "";
    case LogLevel::VERBOSE:
        return "\033[2;34m";
    default:
        return "";
    }
#else
    return "";
#endif
}

inline const char* EndLogColor()
{
#ifndef WINDOWS
    return "\033[0m";
#else
    return "";
#endif
}

void AbstractLogger::Log(LogLevel level, const char* format, va_list ap)
{
    if (level > log_level)
        return;

    if (log_time)
    {
        std::chrono::time_point<std::chrono::system_clock> time_now(std::chrono::system_clock::now());
        (*out) << GetLogColor(level) << GetLogAbbrev(level) << "[" << time_to_string(time_now) << "]" << EndLogColor();
    }

    DoLog(level, format, ap);
}

void Logger::DoLog(LogLevel level, const char* format, va_list ap)
{
    char buffer[1024];
    vsnprintf(buffer, 1024, format, ap);
    (*out) << buffer << std::endl;
    if (level == LogLevel::FATAL) exit(EXIT_FAILURE);
}

EventLog::EventLog(const char* function) : func(function), startTime(std::chrono::system_clock::now())
{
    VerboseLog("start: %s", func);
}

EventLog::~EventLog()
{
    std::chrono::time_point<std::chrono::system_clock> endTime(std::chrono::system_clock::now());
    std::chrono::duration<double> elapsed_seconds = endTime-startTime;
    VerboseLog("end: %s time: %fs", func, elapsed_seconds.count());
}
