#include <sys/time.h>
#include "ISystemDateTimeInterface.h"
#include "DateTime.h"

static class SystemDateTimeInterface : public ISystemDateTimeInterface
{
  private:
    static bool Initialize();

  public:
    static bool Initialized;

    char LoadTimeZone() override
    {
      time_t t = time(NULL);
      struct tm* lt = localtime(&t);
      return (char)((lt->tm_gmtoff / (60* 60)) * 4);
    }

    void SaveTimeZone(char tz) override
    {
      (void)tz; // Ignore
    }

    bool LoadRTCValues(short &millis, short &year, char &month, char &day, char &hour, char &minute, char &second) override
    {
      time_t t = time(NULL);
      struct tm* lt = localtime(&t);
      millis = 0;
      second = (char)lt->tm_sec;
      minute = (char)lt->tm_min;
      hour   = (char)lt->tm_hour;
      day    = (char)lt->tm_mday;
      month  = (char)(lt->tm_mon + 1);
      year   = (short)(lt->tm_year + 1900);
      return true;
    }

    void SaveRTCValues(short millis, short year, char month, char day, char hour, char minute, char second) override
    {
      (void)millis;
      (void)year;
      (void)month;
      (void)day;
      (void)hour;
      (void)minute;
      (void)second;
    }
} SystemDateTime;

bool SystemDateTimeInterface::Initialize()
{
  DateTime::SetSystemInterface(&SystemDateTime);
  return true;
}

bool SystemDateTimeInterface::Initialized = SystemDateTimeInterface::Initialize();
