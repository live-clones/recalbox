#pragma once

class ISystemDateTimeInterface
{
  public:
    // Required virtual destructor
    virtual ~ISystemDateTimeInterface() {}

    // Load stored TimeZone
    virtual char LoadTimeZone() = 0;
    // Store TimeZone
    virtual void SaveTimeZone(char tz) = 0;

    // Load RTC values
    virtual bool LoadRTCValues(short& millis, short& year, char& month, char& day, char& hour, char& minute, char& second) = 0;
    // Load RTC values
    virtual void SaveRTCValues(short millis, short year, char month, char day, char hour, char minute, char second) = 0;
};
