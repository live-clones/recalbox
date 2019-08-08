#include "DateTime.h"
#include "TimeSpan.h"

TimeSpan& TimeSpan::Build(long long ms)
{
  Units._Millis = (int)(ms % 1000LL); ms /= 1000LL;
  Units._Seconds = (int)(ms % 60LL); ms /= 60LL;
  Units._Minutes = (int)(ms % 60LL); ms /= 60LL;
  Units._Hours = (int)ms;
  return *this;
}

TimeSpan::TimeSpan(const DateTime& start, const DateTime& stop)
{
  long long epochStop = (stop.ToEpochTime()) * 1000LL + (long long)stop.Millisecond();
  long long epochStart = (start.ToEpochTime()) * 1000LL + (long long)start.Millisecond();
  Build(epochStop - epochStart);
}

std::string TimeSpan::ToStringFormat(const char* format) const
{
  std::string result;
  bool Escaped = false;

  for (int index = 0; format[index] != 0;)
  {
    char c = format[index];
    if (c == '\\')
    {
      Escaped = true;
      index++;
      continue;
    }
    if ((c != '%') || Escaped)
    {
      result += c;
      Escaped = false;
      index++;
      continue;
    }
    // c is '%', get first format char
    c = format[++index];
    int repeat = 0;
    if (format[++index] == c)
    {
      repeat++;
      if (format[++index] == c)
      {
        repeat++;
        if (format[++index] == c)
        {
          repeat++;
          index++;
        }
      }
    }
    switch (c)
    {
      case 'd':
      {
        if (repeat == 0) result += (Units._Hours / 24);
        break;
      }
      case 'H':
      {
        if (repeat == 0) result +=(Units._Hours);
        else if (repeat == 1) { result += ((char) ('0' + (((Units._Hours % 24) / 10) % 10))); result += ((char) ('0' + ((Units._Hours % 24) % 10))); }
        break;
      }
      case 'm':
      {
        if (repeat == 0) result += (Units._Minutes);
        else if (repeat == 1) { result += ((char) ('0' + ((Units._Minutes / 10) % 10))); result += ((char) ('0' + (Units._Minutes % 10))); }
        else if (repeat == 3) result+= (TotalMinutes());
        break;
      }
      case 's':
      {
        if (repeat == 0) result += (Units._Seconds);
        else if (repeat == 1) { result += ((char) ('0' + ((Units._Seconds / 10) % 10))); result += ((char) ('0' + (Units._Seconds % 10))); }
        else if (repeat == 3) result += (TotalSeconds());
        break;
      }
      case 'f':
      {
        if (repeat == 0) result += (Units._Millis);
        else if (repeat == 2) { result += ((char) ('0' + ((Units._Millis / 100) % 10))); result += ((char) ('0' + ((Units._Millis / 10) % 10))); result += ((char) ('0' + (Units._Millis % 10))); }
        else if (repeat == 3) result += (TotalMilliseconds());
        break;
      }
      case '%':
      {
        result.append(repeat, '%');
        break;
      }
      default:
      {
        result += ("<Unk:");
        result.append(repeat + 1, c);
        result += ('>');
        break;
      }
    }
  }

  return std::move(result);
}

