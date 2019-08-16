#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>
#include "platform.h"
#include "RootFolders.h"
#include "utils/datetime/DateTime.h"

LogLevel Log::reportingLevel = LogLevel::LogInfo;
FILE* Log::file = nullptr;

static std::map<LogLevel, std::string> StringLevel =
{
	{ LogLevel::LogDebug  , "DEBUG" },
	{ LogLevel::LogInfo   , "INFO " },
	{ LogLevel::LogWarning, "WARN!" },
	{ LogLevel::LogError  , "ERROR" },
};

std::string Log::getLogPath()
{
	std::string home = RootFolders::DataRootFolder + "/system/.emulationstation/es_log.txt";
	return home;
}

void Log::open()
{
	file = fopen(getLogPath().c_str(), "w");
}

std::ostringstream& Log::get(LogLevel level)
{
	os << '[' << DateTime().ToPreciseTimeStamp() << "] (" << StringLevel[level] << ") : ";
	messageLevel = level;

	return os;
}

void Log::flush()
{
	fflush(file);
}

void Log::close()
{
  {
    // *DO NOT REMOVE* the enclosing block as it allow the destructor to be called immediately
    // before calling doClose()
    // Generate an immediate log.
    Log().get(LogLevel::LogInfo) << "Closing logger...";
  }
  doClose();
}

void Log::doClose()
{
  fclose(file);
  file = nullptr;
}

Log::~Log()
{
	bool loggerClosed = (file == nullptr);
	// Reopen temporarily
	if (loggerClosed)
  {
	  open();
	  os << " [closed!]";
  }

  os << std::endl;
	fputs(os.str().c_str(), file);
	if (!loggerClosed) flush();
	else doClose();

  // if it's an error, also print to console
  // print all messages if using --debug
  if(messageLevel == LogLevel::LogError || reportingLevel >= LogLevel::LogDebug)
    fputs(os.str().c_str(), stderr);
}
