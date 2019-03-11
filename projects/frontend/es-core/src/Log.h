#ifndef _LOG_H_
#define _LOG_H_

#define LOG(level) \
if (LogLevel::level <= Log::getReportingLevel()) Log().get(LogLevel::level)

#include <string>
#include <sstream>
#include <iostream>

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
    std::ostringstream& get(LogLevel level = LogLevel::LogInfo);

    static LogLevel getReportingLevel() { return reportingLevel; }
    static void setReportingLevel(LogLevel level) { reportingLevel = level; }

    static std::string getLogPath();

    static void flush();
    static void open();
    static void close();

  protected:
    std::ostringstream os;
    static FILE* file;

  private:
    static LogLevel reportingLevel;
    static FILE* getOutput();
    LogLevel messageLevel;
};

#endif
