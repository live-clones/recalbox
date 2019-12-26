#include "Log.h"
#include "RootFolders.h"
#include "utils/datetime/DateTime.h"

LogLevel Log::reportingLevel = LogLevel::LogInfo;
FILE* Log::sFile = nullptr;

static const char* StringLevel[] =
{
  "ERROR",
  "WARN!",
  "INFO ",
	"DEBUG",
};

Path Log::getLogPath(const char* filename)
{
	return RootFolders::DataRootFolder / "system/.emulationstation" / filename;
}

void Log::open(const char* filename)
{
	sFile = fopen(getLogPath(filename != nullptr ? filename : "es_log.txt").ToChars(), "w");
}

Log& Log::get(LogLevel level)
{
	mMessage.append(1, '[')
	        .append(DateTime().ToPreciseTimeStamp())
	        .append("] (")
	        .append(StringLevel[(int)level])
	        .append(") : ");
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
