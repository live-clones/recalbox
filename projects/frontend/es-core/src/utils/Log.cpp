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
	return RootFolders::DataRootFolder / "system/logs" / filename;
}

void Log::open(const char* filename)
{
  // Build log path
  Path logpath(filename != nullptr ? filename : getLogPath("es_log.txt").ToChars());

  // Backup?
  if (logpath.Exists())
  {
    Path backup(logpath.ToString() + ".backup");
    if (!backup.Delete()) { printf("[Logs] Cannot remove old log!"); }
    if (!Path::Rename(logpath, backup)) { printf("[Logs] Cannot backup current log!"); }
  }

  // Open new log
  sFile = fopen(logpath.ToChars(), "w");
}

Log& Log::get(LogLevel level)
{
	mMessage.Append('[')
	        .Append(DateTime().ToPreciseTimeStamp())
	        .Append(LEGACY_STRING("] ("))
	        .Append(StringLevel[(int)level])
	        .Append(LEGACY_STRING(") : "));
	messageLevel = level;

	return *this;
}

void Log::flush()
{
	if (fflush(sFile) != 0) { printf("[Logs] Cannot flush current log!"); }
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
    if (fclose(sFile) != 0) { printf("[Logs] Cannot close current log!"); }
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
  if (messageLevel == LogLevel::LogError || reportingLevel >= LogLevel::LogDebug)
    fputs(mMessage.c_str(), stdout);
}
