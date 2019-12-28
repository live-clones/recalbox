#include "Log.h"
#include <map>
#include "platform.h"
#include "RootFolders.h"
#include "utils/datetime/DateTime.h"

LogLevel Log::reportingLevel = LogLevel::LogInfo;
FILE* Log::sFile = nullptr;

static std::map<LogLevel, std::string> StringLevel =
{
	{ LogLevel::LogDebug  , "DEBUG" },
	{ LogLevel::LogInfo   , "INFO " },
	{ LogLevel::LogWarning, "WARN!" },
	{ LogLevel::LogError  , "ERROR" },
};

Path Log::getLogPath()
{
	return RootFolders::DataRootFolder / "/system/.emulationstation/es_log.txt";
}

void Log::open()
{
	sFile = fopen(getLogPath().ToChars(), "w");
}

Log& Log::get(LogLevel level)
{
	mMessage = '[' + DateTime().ToPreciseTimeStamp() + "] (" + StringLevel[level] + ") : ";
	messageLevel = level;

	return *this;
}

void Log::flush()
{
	fflush(sFile);
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
  if (sFile != nullptr)
    fclose(sFile);
  sFile = nullptr;
}

Log::~Log()
{
	bool loggerClosed = (sFile == nullptr);
	// Reopen temporarily
	if (loggerClosed)
  {
	  open();
	  mMessage += " [closed!]";
  }

  mMessage += '\n';
	fputs(mMessage.c_str(), sFile);
	if (!loggerClosed) flush();
	else doClose();

  // if it's an error, also print to console
  // print all messages if using --debug
  if(messageLevel == LogLevel::LogError || reportingLevel >= LogLevel::LogDebug)
    fputs(mMessage.c_str(), stderr);
}
