#pragma once

#ifndef __PACKED__
#  if  defined(__GNUC__) ||  defined (__clang__)
#    define __PACKED__ __attribute__((packed))
#  else
#    define __PACKED__
#  endif
#endif

#include "TimeSpan.h"
#include "ISystemDateTimeInterface.h"

/*!
 * Pure Date/Time holder. Packed to keep size contained on 10 bytes
 */
class DateTime
{
private:
  //! Interface to system datetime/timezone persistence
  static ISystemDateTimeInterface* _SystemDateTimeInterface;
  //! Default timezone
  static char _DefaultTimeZone;

  union
  {
    struct
    {
      short _Millis;   //!< Milliseconds [0..999]
      short _Year;     //!< Year    [0000..9999]
      char  _Month;    //!< Month   [1..12]
      char  _Day;      //!< Day     [1..31]
      char  _Hour;     //!< Hour    [0..23]
      char  _Minute;   //!< Minutes [0..59]
      char  _Second;   //!< Seconds [0..59]
      char  _TimeZone; //!< Time offset from UTC, expressed in quarter of hour
    } __PACKED__;
    struct
    {
      long long _low;  //!< low part for fast copy
      short     _high; //!< high part for fast copy
    } __PACKED__;
  } __PACKED__;

  //! Bit size of every single item - Total must be 64 - Unqualified private naming for implicit int casting
  enum CompactBitSize
  {
    cbsYear = 28,  //!< cbsYear
    cbsMonth = 4,  //!< cbsMonth
    cbsDay = 5,    //!< cbsDay
    cbsHour = 5,   //!< cbsHour
    cbsMinute = 6, //!< cbsMinute
    cbsSecond = 6, //!< cbsSecond
    cbsMillis = 10,//!< cbsMillis
  };

  //! Position of every item in the 64bits compacted value - Unqualified private naming for implicit int casting
  enum CompactPosition
  {
    cpYear = cbsMonth + cbsDay + cbsHour + cbsMinute + cbsSecond + cbsMillis,
    cpMonth = cbsDay + cbsHour + cbsMinute + cbsSecond + cbsMillis,
    cpDay = cbsHour + cbsMinute + cbsSecond + cbsMillis,
    cpHour = cbsMinute + cbsSecond + cbsMillis,
    cpMinute = cbsSecond + cbsMillis,
    cpSecond = cbsMillis,
    cpMillis = 0,
  };

  /*!
   * Compact 10 bytes in 64bits ordered for fast comparisons
   * @return Compacted value
   */
  long long Compact() const
  {
    DateTime utc = ToUtc();
    long long r = (long long)(utc._Year);
    r <<= cbsMonth;  r |= (long long)(utc._Month);
    r <<= cbsDay;    r |= (long long)(utc._Day);
    r <<= cbsHour;   r |= (long long)(utc._Hour);
    r <<= cbsMinute; r |= (long long)(utc._Minute);
    r <<= cbsSecond; r |= (long long)(utc._Second);
    r <<= cbsMillis; r |= (long long)(utc._Millis);
    return r;
  }

  /*!
   * Return elapsed days since the beginning of the era, using Bob Orlando's Algorithm
   * @return Elapsed days from the 0000/01/01
   */
  int ElapsedDayFromEpoch(int year, int month, int day) const;

  /*!
   * Fill the current object from RTC values
   */
  void FillFromRtc();

  /*!
   * Fill the current object with start of era values: 0000-01-01T00:00:00+0000
   */
  void FillFromStartOfEra();

  /*!
   * Fill the current object from Epoch time
   * @param epochtime Epoch from which to extract calendar fields
   */
  void FillFromEpochTime(long long epochtime);

  /*!
   * Check and adjust individual field (and emit warning accordingly)
   */
  void Control();

  /*!
   * Tool method to extract numeric values from string between minimum/maximum numeric chars.
   * @param str string to extract value from
   * @param min minimum required chars
   * @param max maximum required chars
   * @param result result value
   * @return true if the fetching was successful, false otherwise
   * @note str move forward during fetching
   */
  static bool FetchNumeric(const char*& str, int min, int max, int& result);

  /*!
   * Tool method to extract string index, comparing the given string to the ones in the given string array array.
   * @param str string to seek for
   * @param strs string array
   * @param count array value count
   * @param result resulting index if the string if found
   * @param zerobased if false, the index if increased by one to match non-zero based time unit
   * @return true if the fetching was successful, false otherwise
   * @note str move forward during fetching
   */
  static bool FetchStringIndex(const char*& str, const char* strs[], int count, int& result, bool zerobased);

public:
  static void SetSystemInterface(ISystemDateTimeInterface* interface)
  {
    _SystemDateTimeInterface = interface;
    if (interface != nullptr)
      _DefaultTimeZone = interface->LoadTimeZone();
  }

  /*!
   * Default constructor. Initialized using RTC
   */
  DateTime();
  /*!
   * Copy constructor
   */
  DateTime(const DateTime& source) { _low = source._low; _high = source._high; }
  /*!
   * Constructor to initialize or set default datetime: 0000-01-01T00:00:00+0000
   * @param initialized True to initialize from RTC, false to set to 0000-01-01T00:00:00+0000
   */
  explicit DateTime(bool initialized);
  /*!
   * Constructor with date initialization
   * @param year Year number (absolute format)
   * @param month Month number from 1 to 12
   * @param day Day number from 1 to 28/31
   */
  DateTime(int year, int month, int day);
  /*!
   * Constructor with date and time initialization
   * @param year Year number (absolute format)
   * @param month Month number from 1 to 12
   * @param day Day number from 1 to 28/31
   * @param hour Hour from 0 to 24 (24+ are wrapped around)
   * @param minute from 0 to 60 (60+ are wrapped around)
   * @param second from 0 to 60 (60+ are wrapped around)
   */
  DateTime(int year, int month, int day, int hour, int minute, int second);
  /*!
   * Constructor with date and time initialization
   * @param year Year number (absolute format)
   * @param month Month number from 1 to 12
   * @param day Day number from 1 to 28/31
   * @param hour Hour from 0 to 24 (24+ are wrapped around)
   * @param minute from 0 to 60 (60+ are wrapped around)
   * @param second from 0 to 60 (60+ are wrapped around)
   * @param millisecond from 0 to 999
   */
  DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);
  /*!
   * Constructor with date and time initialization
   * @param year Year number (absolute format)
   * @param month Month number from 1 to 12
   * @param day Day number from 1 to 28/31
   * @param hour Hour from 0 to 24 (24+ are wrapped around)
   * @param minute from 0 to 60 (60+ are wrapped around)
   * @param second from 0 to 60 (60+ are wrapped around)
   * @param millisecond from 0 to 999
   * @param tzq TimeZone in quarter of hours
   */
  DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int tzq);
  /*!
   * Constructor with date and time initialization
   * @param year Year number (absolute format)
   * @param month Month number from 1 to 12
   * @param day Day number from 1 to 28/31
   * @param hour Hour from 0 to 24 (24+ are wrapped around)
   * @param minute from 0 to 60 (60+ are wrapped around)
   * @param second from 0 to 60 (60+ are wrapped around)
   * @param millisecond from 0 to 999
   * @param tzh TimeZone hours (can be negative)
   * @param tzm TimeZone minutes (can be negative)
   */
  DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int tzh, int tzm);
  /*!
   * Initialize from epoch time (second from 1970)
   * @param epochtime Elapsed second from the 1970/01/01
   */
  explicit DateTime(long long epochtime);
  /*!
   * Constructor to build a DateTime from an ISO8601 string: YYYY-MM-ddTHH:mm:ss+ZZZZ
   * @note Only the strict YYYY-MM-ddTHH:mm:ss+ZZZZ format is accepted.
   * @param iso8601 String to parse
   */
  explicit DateTime(const std::string& iso8601) { ParseFromString("%YYYY-%MM-%ddT%HH:%mm:%ss%zzzz", iso8601, *this); }
  /*!
   * Constructor to build a DateTime from a given format
   * @param format format string (@see ToStringFormat)
   * @param strtoparse string to parse
   */
  DateTime(const char* format, const std::string& strtoparse) { ParseFromString(format, strtoparse, *this); }

  /*!
   * Return the day of week of the current object, from 0 (monday) to 6 (sunday)
   * @return
   */
  int DayOfWeek() const;

  /*!
   * Return a string representation of the current object, using the following tags:
   * Y    : Numeric year (1..4 digits)
   * YY   : 2 digits year
   * YYYY : 4 digits year
   * M    : Numeric month (1..2 digits)
   * MM   : 2 digits format Month
   * MMM  : Abbreviated Month name (english)
   * MMMM : Full length Month name (english)
   * d    : Numeric day (1..2 digits)
   * dd   : 2 digits day (1..2 digits)
   * ddd  : Abbreviated Day name (english)
   * dddd : Full-length Day name (english)
   * H    : Numeric hour in 24h format (1..23)
   * HH   : 2 digits Numeric hour in 24h format
   * m    : Numeric minutes (1..2 digits)
   * mm   : 2 digits minutes
   * s    : Numeric seconds (1..2 digits)
   * ss   : 2 digits seconds
   * f    : Numeric milliseconds (1..3 digits)
   * fff  : 3 digits milliseconds
   * z    : Quarters of hour to UTC
   * zz   : +-Hours to UTC
   * zzzz : +-Hours/Minutes to UTC
   * Tags must follow a '%' (percent) character. Escape any character using '\'.
   * @param format String format
   * @return String representation of the current
   */
  std::string ToStringFormat(const char* format) const;

  /*!
   * Parse a string according to a given format.
   * If any error occurs during the parsing, the returned DateTime is still build from what has been properly parsed.
   * @param format String format (see ToFormat)
   * @param str string to parse
   * @param destination Datetime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool ParseFromString(const char* format, const std::string& str, DateTime& destination);

  /*!
   * Parse a string according to the strict ISO6801 representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromISO6801(const std::string& from, DateTime& destination) { return ParseFromString("%YYYY-%MM-%ddT%HH:%mm:%ss%zzzz", from, destination); }

  /*!
   * Parse a string according to the short ISO6801 representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromCompactISO6801(const std::string& from, DateTime& destination) { return ParseFromString("%YYYY%MM%ddT%HH%mm%ss", from, destination); }

  /*!
   * Parse a string according to the compact representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromCompactFormat(const std::string& from, DateTime& destination) { return ParseFromString("%YYYY%MM%dd%HH%mm%ss", from, destination); }

  /*!
   * Parse a string according to the short representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromShortFormat(const std::string& from, DateTime& destination) { return ParseFromString("%YYYY/%MM/%dd %HH:%mm:%ss%zz", from, destination); }

  /*!
   * Parse a string according to the precise representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromPreciseFormat(const std::string& from, DateTime& destination) { return ParseFromString("%YYYY/%MM/%dd %HH:%mm:%ss.%fff", from, destination); }

  /*!
   * Parse a string according to the long representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromLongFormat(const std::string& from, DateTime& destination) { return ParseFromString("%YYYY/%MMM/%dd %HH:%mm:%ss%zzzz", from, destination); }

  /*!
   * Parse a string according to the human readable representation
   * @param from String to parse
   * @param destination DateTime to fill with the parsing result
   * @return Set to true if the parsing was successful
   */
  static bool FromHumanFormat(const std::string& from, DateTime& destination) { return ParseFromString("%dddd %dd, %MMMM %YYYY - %HH:%mm:%ss%zzzz", from, destination); }

  /*!
   * Return the current DateTime in an ISO8601 string format: YYYY-MM-ddTHH:mm:ss+zzzz
   * @return
   */
  std::string ToISO8601() const { return std::move(ToStringFormat("%YYYY-%MM-%ddT%HH:%mm:%ss%zzzz")); }

  /*!
   * Return the current DateTime in an ISO8601 string format: YYYY-MM-ddTHH:mm:ss+zzzz
   * @return
   */
  std::string ToCompactISO8601() const { return std::move(ToStringFormat("%YYYY%MM%ddT%HH%mm%ss")); }

    /*!
   * Return very compact 14 digit DateTime representation
   * @return Compact representation of the current DateTime
   */
  std::string ToCompactFormat() const { return std::move(ToStringFormat("%YYYY%MM%dd%HH%mm%ss")); }

  /*!
   * Return short DateTime representation
   * @return Short representation of the current DateTime
   */
  std::string ToShortFormat() const { return std::move(ToStringFormat("%YYYY/%MM/%dd %HH:%mm:%ss%zz")); }

  /*!
   * Return precise timestamp DateTime representation
   * @return Short representation of the current DateTime
   */
  std::string ToPreciseTimeStamp() const { return std::move(ToStringFormat("%YYYY/%MM/%dd %HH:%mm:%ss.%fff")); }

  /*!
   * Return long DateTime representation (english names)
   * @return Long representation of the current DateTime
   */
  std::string ToLongFormat() const { return std::move(ToStringFormat("%YYYY/%MMM/%dd %HH:%mm:%ss%zzzz")); }

  /*!
   * Return Human readable DateTime representation (english names)
   * @return Human readable representation of the current DateTime
   */
  std::string ToHumanFormat() const { return std::move(ToStringFormat("%dddd %dd, %MMMM %YYYY - %HH:%mm:%ss%zzzz")); }

  /*!
   * Return the epoch time representation of the current DateTime
   * @return Epoch time
   */
  long long ToEpochTime() const;

  /*!
   * Set the RTC using current DateTime
   */
  void SaveToRtc();

  //! Return the Year part of the current DateTime
  int Year() const { return _Year; }
  //! Return the Month part of the current DateTime
  int Month() const { return _Month; }
  //! Return the Day part of the current DateTime
  int Day() const { return _Day; }
  //! Return the Hour part of the current DateTime
  int Hour() const { return _Hour; }
  //! Return the Minute part of the current DateTime
  int Minute() const { return _Minute; }
  //! Return the Second part of the current DateTime
  int Second() const { return _Second; }
  //! Return the Millisecond part of the current DateTime
  int Millisecond() const { return _Millis; }
  //! Return the TimeZone part of the current DateTime in quarter of hour
  int TimeZone() const { return _TimeZone; }

  /*!
   * Return the current DateTime converted to UTC
   * Example: 2017-03-16 16:05:33+0400 => 2017-03-16 12:05:33+0000
   *          2017-03-16 16:05:33-0900 => 2017-03-17 01:05:33+0000
   *          2017-03-16 16:05:33-0930 => 2017-03-17 01:35:33+0000
   * @return UTC DateTime
   */
  DateTime ToUtc() const
  {
    DateTime result(*this);
    result -= TimeSpan(_TimeZone * 15, 0, 0);
    result._TimeZone = 0;
    return result;
  }

  /*!
   * Return the current DateTime converted to Local Time
   * @return UTC DateTime
   */
  DateTime ToLocal() const
  {
    int localtz = (_SystemDateTimeInterface != nullptr) ? _SystemDateTimeInterface->LoadTimeZone() : 0;
    DateTime result(*this);
    result += TimeSpan((localtz - _TimeZone) * 15, 0, 0);
    result._TimeZone = localtz;
    return result;
  }

    /*!
   * Add a Timespan to the current DateTime
   * @param ts TimeSpan to add
   * @return The current DateTime
   */
  DateTime& operator += (const TimeSpan& ts) { long long epochMs = (ToEpochTime() * 1000LL + _Millis) + ts.TotalMilliseconds(); FillFromEpochTime(epochMs / 1000LL); _Millis = epochMs % 1000; return *this; }
  /*!
  * Substract a Timespan to the current DateTime
  * @param ts TimeSpan to substract
  * @return The current DateTime
  */
  DateTime& operator -= (const TimeSpan& ts) { long long epochMs = (ToEpochTime() * 1000LL + _Millis) - ts.TotalMilliseconds(); FillFromEpochTime(epochMs / 1000LL); _Millis = epochMs % 1000; return *this; }
  /*!
  * Get a new DateTime representing the current DateTime plus a Timespan
  * @param ts TimeSpan to add
  * @return New DateTime
  */
  DateTime operator + (const TimeSpan& ts) const { long long epochMs = (ToEpochTime() * 1000LL + _Millis) + ts.TotalMilliseconds(); DateTime result(epochMs / 1000LL); result._Millis = epochMs % 1000; return result; }
  /*!
  * Get a new DateTime representing the current DateTime minus a Timespan
  * @param ts TimeSpan to substract
  * @return New DateTime
  */
  DateTime operator - (const TimeSpan& ts) const { long long epochMs = (ToEpochTime() * 1000LL + _Millis) - ts.TotalMilliseconds(); DateTime result(epochMs / 1000LL); result._Millis = epochMs % 1000; return result; }

  /*!
   * Substract the given DateTime to the current DateTime and return the difference as a TimeSpan
   * @param dt DateTime to substract
   * @return TimeSpan representing the signed difference
   */
  TimeSpan operator - (const DateTime& dt) const { return TimeSpan((ToEpochTime() * 1000LL + _Millis) - (dt.ToEpochTime() * 1000LL + dt._Millis)); }

  /*!
   * Equality operator
   * @param to DateTime instance to compare to
   * @return True if both instances are equal. False otherwise.
   */
  bool operator == (const DateTime& to) const { return Compact() == to.Compact(); }
  /*!
   * Inequality operator
   * @param to DateTime instance to compare to
   * @return True if both instances are not equal. False otherwise.
   */
  bool operator != (const DateTime& to) const { return Compact() != to.Compact(); }
  /*!
   * Lesser operator
   * @param to DateTime instance to compare to
   * @return True if the current instance is lesser than the one it compares to. False otherwise.
   */
  bool operator <  (const DateTime& to) const { return Compact() < to.Compact(); }
  /*!
   * Lesser or equal operator
   * @param to DateTime instance to compare to
   * @return True if the current instance is lesser than or equal to the one it compares to. False otherwise.
   */
  bool operator <= (const DateTime& to) const { return Compact() <= to.Compact(); }
  /*!
   * Greater operator
   * @param to DateTime instance to compare to
   * @return True if the current instance is greater than the one it compares to. False otherwise.
   */
  bool operator >  (const DateTime& to) const { return Compact() > to.Compact(); }
  /*!
   * Greater or equal operator
   * @param to DateTime instance to compare to
   * @return True if the current instance is greater than or equal to the one it compares to. False otherwise.
   */
  bool operator >= (const DateTime& to) const { return Compact() >= to.Compact(); }

  //! Date & Time fields used as precision level in Compare method
  enum class Item
  {
    Year = 0,
    Month = 1,
    Day = 2,
    Hour = 3,
    Minute = 4,
    Second = 5,
    Millisecond = 6
  };

  /*!
   * Compare the current DateTime to another instance with a given level of precision
   * @param to DateTime instance to compares to
   * @param precision Precision level, from Year to Milliseconds.
   * @return -1 if current < "to". +1 if current > "to". 0 if both are equal.
   */
  int Compare(const DateTime& to, Item precision) const
  {
    // Static precision mask shift.
    static int MaskPositionShift[] = { cpYear, cpMonth, cpDay, cpHour, cpMinute, cpSecond, cpMillis };
    long long difference = (Compact() >> MaskPositionShift[(int)precision]) - (to.Compact() >> MaskPositionShift[(int)precision]);
    return (difference < 0 ? -1 : (difference > 0 ? 1 : 0));
  }

  /*!
   * Return true if the current Year is a leap year
   * @return True if the Year is a leap year. False otherwise
   */
  bool IsLeapYear() const { return ((bool)((((_Year & 3) == 0) && (_Year % 100 != 0)) || (_Year % 400 == 0))); }

  /*!
  * Return true if the current DateTime is UTC (TimeZone = 0)
  * @return True if the current DateTime is UTC. False otherwise
  */
  bool IsUtc() const { return _TimeZone == 0; }

  /*!
   * Return the number of day in the current month.
   * @return Number of days
   */
  int DayPerMonth() const;

  /*!
   * Return the number of day in the given month.
   * @param month Month to get number of days
   * @param year If the given year is leap, february is 29 days.
   * @return Number of days
   */
  static int DayPerMonth(int month, int year);

  DateTime& AddMilliseconds(int ms) { return operator +=(TimeSpan(ms)); }
  DateTime& AddSeconds(int seconds) { return operator +=(TimeSpan(seconds, 0)); }
  DateTime& AddMinutes(int minutes) { return operator +=(TimeSpan(minutes, 0, 0)); }
  DateTime& AddHours(int hours)     { return operator +=(TimeSpan(hours, 0, 0, 0)); }
  DateTime& AddDays(int days)       { return operator +=(TimeSpan(days * 24, 0, 0, 0)); }
  DateTime& AddMonth(int month)
  {
    _Month += month;
    if (_Month < 1)
    {
      _Year -= (--_Month / 12) + 1;            // Month to 0-11
      _Month = (char)((12 + _Month % 12) + 1); // Month to 1-12
    }
    if (_Month > 12)
    {
      _Year += (--_Month / 12);           // Month to 0-11
      _Month = (char)((_Month % 12) + 1); // Month to 1-12
    }
    if (_Day > DayPerMonth(_Month, _Year)) _Day = (char)DayPerMonth(_Month, _Year);
    return *this;
  }
  DateTime& AddYears(int years)
  {
    _Year += years;
    if (_Day > DayPerMonth(_Month, _Year)) _Day = (char)DayPerMonth(_Month, _Year); // 29 february case
    return *this;
  }

  DateTime& MoveNextMillisecond() { return AddMilliseconds(1); }
  DateTime& MoveNextSecond()      { return AddSeconds(1); }
  DateTime& MoveNextMinute()      { return AddMinutes(1); }
  DateTime& MoveNextHour()        { return AddHours(1); }
  DateTime& MoveNextDay()         { return AddDays(1); }
  DateTime& MoveNextMonth()       { return AddMonth(1); }
  DateTime& MoveNextYear()        { return AddYears(1); }

  DateTime GetPlusMilliseconds(int ms) const { return DateTime(*this).AddMilliseconds(ms); }
  DateTime GetPlusSeconds(int seconds) const { return DateTime(*this).AddSeconds(seconds); }
  DateTime GetPlusMinutes(int minutes) const { return DateTime(*this).AddMinutes(minutes); }
  DateTime GetPlusHours(int hours)     const { return DateTime(*this).AddHours(hours); }
  DateTime GetPlusDays(int days)       const { return DateTime(*this).AddDays(days); }
  DateTime GetPlusMonth(int month)     const { return DateTime(*this).AddMonth(month); }
  DateTime GetPlusYear(int years)      const { return DateTime(*this).AddYears(years); }

  DateTime GetNextMillisecond() const { return DateTime(*this).AddMilliseconds(1); }
  DateTime GetNextSecond()      const { return DateTime(*this).AddSeconds(1); }
  DateTime GetNextMinute()      const { return DateTime(*this).AddMinutes(1); }
  DateTime GetNextHour()        const { return DateTime(*this).AddHours(1); }
  DateTime GetNextDay()         const { return DateTime(*this).AddDays(1); }
  DateTime GetNextMonth()       const { return DateTime(*this).AddMonth(1); }
  DateTime GetNextYear()        const { return DateTime(*this).AddYears(1); }

} __PACKED__ __attribute__ ((warn_unused));
