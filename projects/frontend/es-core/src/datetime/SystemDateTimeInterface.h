#pragma once

#include <cstddef>
#include "ISystemDateTimeInterface.h"
#include "DateTime.h"
#include <sys/time.h>

class SystemDateTimeImplementation : public ISystemDateTimeInterface
{
  public:
    static void Initialize();

    char LoadTimeZone() override
    {
      time_t t = time(nullptr);
      struct tm* lt = localtime(&t);
      return (char)((lt->tm_gmtoff / (60* 60)) * 4);
    }

    void SaveTimeZone(char tz) override
    {
      (void)tz; // Ignore
    }

    bool LoadRTCValues(short &millis, short &year, char &month, char &day, char &hour, char &minute, char &second) override
    {
      timeval tv;
      gettimeofday(&tv, nullptr);
      time_t t=tv.tv_sec;
      struct tm* lt = localtime(&t);
      millis = tv.tv_usec / 1000;
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
};
