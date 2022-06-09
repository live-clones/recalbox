#include <string>
#include <utils/Strings.h>
#include <utils/IniFile.h>
#include <utils/Log.h>
#include "Resolutions.h"
#include "ResolutionAdapter.h"

struct Parameters
{
  bool Verbose;
  bool Fix;
  bool Error;
  std::string File;
};

Parameters Parse(int argc, char* argv[])
{
  Parameters result { false, false, false, "/recalbox/share/system/recalbox.conf" };

  for(int i = argc; --i > 0; )
  {
    std::string s(argv[i]);
    if (s[0] == '-')
    {
      if (s.length() > 1 && s[1] == '-')
      {
        s = s.erase(0, 2);
        if (s == "verbose") result.Verbose = true;
        else if (s == "fix") result.Fix = true;
        else result.Error = true;
      }
      else
      {
        for(int p = (int)s.length(); --i > 0; )
          switch(s[p])
          {
            case 'v': result.Verbose = true; break;
            case 'f': result.Fix = true; break;
            default: result.Error = true; break;
          }
      }
    }
    else result.File = s;
  }

  return result;
}

int main(int argc, char* argv[])
{
  Parameters param = Parse(argc, argv);

  if (param.Verbose) Log::setReportingLevel(LogLevel::LogDebug);
  Log::open();

  if (param.Error)
  {
    printf("Usage: check-resolution [options] FILE...\n");
    printf("       Check resolution keys of /recalbox/share/system/recalbox.conf\n");
    printf("       -v, --verbose   Verbose output\n");
    printf("       -f, --fix       Fix values in conf file\n");
    printf("       FILE defaults to /recalbox/share/system/recalbox.conf if omitted\n");
    return 1;
  }

  // Get supported resolutions
  { LOG(LogInfo) << "[Check] Get resolutions list..."; }
  const ResolutionAdapter::ResolutionList& list = ResolutionAdapter::GetResolutionList();

  IniFile ini(Path(param.File), false);
  { LOG(LogInfo) << "[Check] Loaded configuration: " << param.File; }
  Strings::Vector keys = ini.GetKeyEndingWith(".videomode");
  { LOG(LogDebug) << "[Check] Extracted " << (int)keys.size() << "videomode keys"; }

  bool changes = false;
  for(const std::string& videomode : keys)
  {
    std::string value = ini.AsString(videomode);
    Resolutions::SimpleResolution reso { 0, 0 };
    { LOG(LogInfo) << "[Check]   Key " << videomode << " = " << value; }
    if (ResolutionAdapter::AdjustResolution(0, value, list, reso))
    {
      if (reso.IsValid())
      {
        std::string newResolution = Strings::ToString(reso.Width).append(1, 'x').append(Strings::ToString(reso.Height));
        ini.SetString(videomode, newResolution);
        { LOG(LogInfo) << "[Check]     Adjusted to " << newResolution; }
        changes = true;
      }
      else { LOG(LogInfo) << "[Check]     Not adjusted"; }
    }
    else { LOG(LogInfo) << "[Check]     Kept as is"; }
  }
  if (changes && param.Fix)
  {
    ini.Save();
    { LOG(LogInfo) << "[Check] Saved configuration: " << param.File; }
  }
  else ini.Cancel();

  return 0;
}
