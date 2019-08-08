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
	fclose(file);
	file = nullptr;
}

Log::~Log()
{
	os << std::endl;

	if(file == nullptr)
	{
		// not open yet, print to stdout
		std::cerr << "ERROR - tried to write to log file before it was open! The following won't be logged:\n";
		std::cerr << os.str();
		return;
	}

	fprintf(file, "%s", os.str().c_str());

	//if it's an error, also print to console
	//print all messages if using --debug
	if(messageLevel == LogLevel::LogError || reportingLevel >= LogLevel::LogDebug)
		fprintf(stderr, "%s", os.str().c_str());
}
