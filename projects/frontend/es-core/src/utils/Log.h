#ifndef _LOG_H_
#define _LOG_H_

#define LOG(level) \
if (LogLevel::level <= Log::getReportingLevel()) Log().get(LogLevel::level)

#include <string>
#include <utils/os/fs/Path.h>
#include "Strings.h"

enum class LogLevel
{
	LogError,
	LogWarning,
	LogInfo,
	LogDebug,
};

class Log
{
  public:
    ~Log();
    Log& get(LogLevel level = LogLevel::LogInfo);

    static LogLevel getReportingLevel() { return reportingLevel; }
    static void setReportingLevel(LogLevel level) { reportingLevel = level; }

    static Path getLogPath();

    static void flush();
    static void open();
    static void close();

    Log& operator << (char v) { mMessage += v; return *this; }
    Log& operator << (const char* v) { mMessage += v; return *this; }
    Log& operator << (const std::string& v) { mMessage += v; return *this; }
    Log& operator << (int v) { mMessage += Strings::ToString(v); return *this; }
    Log& operator << (unsigned int v) { mMessage += Strings::ToString(v); return *this; }
    Log& operator << (long long v) { mMessage += Strings::ToString(v); return *this; }
    Log& operator << (unsigned long long v) { mMessage += Strings::ToString(v); return *this; }
    Log& operator << (long v) { mMessage += Strings::ToString((long long)v); return *this; }
    Log& operator << (unsigned long v) { mMessage += Strings::ToString((unsigned long long)v); return *this; }
    Log& operator << (bool v) { mMessage += Strings::ToString(v); return *this; }
    Log& operator << (float v) { mMessage += Strings::ToString(v, 4); return *this; }

  protected:
    std::string mMessage;
    static FILE* sFile;

  private:
    static LogLevel reportingLevel;
    LogLevel messageLevel;

    static void doClose();
};

#endif
