#include <string>
#include <vector>
#include <SDL2/SDL.h>

struct Parameters
{
  bool CurrentModeOnly;
  bool JsonOutput;
  bool FullInfo;
  bool DefaultDisplayOnly;
  bool Error;
};

Parameters Parse(int argc, char* argv[])
{
  Parameters result { false, false, false, false, false };

  for(int i = argc; --i >= 0; )
  {
    std::string s(argv[i]);
    if (s[0] == '-')
    {
      if (s.length() > 1 && s[1] == '-')
      {
        s = s.erase(0, 2);
        if (s == "json") result.JsonOutput = true;
        else if (s == "current") result.CurrentModeOnly = true;
        else if (s == "fullinfo") result.FullInfo = true;
        else if (s == "default") result.DefaultDisplayOnly = true;
        else result.Error = true;
      }
      else
      {
        for(int p = (int)s.length(); --i > 0; )
          switch(s[p])
          {
            case 'c': result.CurrentModeOnly = true; break;
            case 'd': result.DefaultDisplayOnly = true; break;
            case 'f': result.FullInfo = true; break;
            case 'j': result.JsonOutput = true; break;
            default: result.Error = true; break;
          }
      }
    }
  }

  return result;
}

struct Resolution
{
  int Display;
  int Width;
  int Height;
  int Bpp;
  int Frequency;

  std::string ToString(bool json, bool all, bool lastitem) const
  {
    if (json)
    {
      if (all) return std::string("{ Display: \"")
                     .append(std::to_string(Display))
                     .append("\", Width: \"")
                     .append(std::to_string(Width))
                     .append("\", Height: \"")
                     .append(std::to_string(Height))
                     .append("\", Bpp: \"")
                     .append(std::to_string(Bpp))
                     .append("\", Frequency: \"")
                     .append(std::to_string(Frequency))
                     .append(lastitem ? "\" }" : "\" },");
      return std::string("{ Width: \"")
            .append(std::to_string(Width))
            .append("\", Height: \"")
            .append(std::to_string(Height))
            .append("\", Bpp: \"")
            .append(std::to_string(Bpp))
            .append(lastitem ? "\" }" : "\" },");
    }
    else
    {
      if (all) return std::to_string(Display)
                     .append(1, ':')
                     .append(std::to_string(Width))
                     .append(1, ':')
                     .append(std::to_string(Height))
                     .append(1, ':')
                     .append(std::to_string(Bpp))
                     .append(1, ':')
                     .append(std::to_string(Frequency));
      return std::to_string(Display)
            .append(1, ':')
            .append(std::to_string(Width))
            .append(1, ':')
            .append(std::to_string(Height));
    }
  }
};
typedef std::vector<Resolution> ResolutionList;

const ResolutionList& GetResolutionList()
{
  SDL_Init(SDL_INIT_VIDEO);
  static ResolutionList resolutions;
  SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr };
  int displayCount = SDL_GetNumVideoDisplays();
  for (int i = 0; i < displayCount; ++i)
  {
    int modesCount = SDL_GetNumDisplayModes(i);
    for (int m = 0; m < modesCount; ++m)
      if (SDL_GetDisplayMode(i, m, &mode) == 0)
        resolutions.push_back({ i, mode.w, mode.h, (int)SDL_BITSPERPIXEL(mode.format), mode.refresh_rate });
  }
  SDL_Quit();

  return resolutions;
}

const ResolutionList& GetCurrentResolutions()
{
  SDL_Init(SDL_INIT_VIDEO);
  static ResolutionList resolutions;
  SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr };
  int displayCount = SDL_GetNumVideoDisplays();
  for (int i = 0; i < displayCount; ++i)
    if (SDL_GetDesktopDisplayMode(i, &mode) == 0)
      resolutions.push_back({ i, mode.w, mode.h, (int)SDL_BITSPERPIXEL(mode.format), mode.refresh_rate });
  SDL_Quit();

  return resolutions;
}

int main(int argc, char* argv[])
{
  Parameters param = Parse(argc, argv);

  if (param.Error)
  {
    printf("Usage: videmode [options]\n");
    printf("       List all resolutions available\n");
    printf("       -c, --current  Output current resolution only\n");
    printf("       -d, --default  Output resolutions from default display only\n");
    printf("       -f, --fullinfo Add bit-per-plane and frequency to informations\n");
    printf("       -j, --json     Output json instead of plain text\n");
    return 1;
  }

  ResolutionList list = param.CurrentModeOnly ? GetCurrentResolutions() : GetResolutionList();
  if (param.JsonOutput) printf("{\n");
  int count = (int)list.size();
  for(const Resolution& resolution : list)
  {
    if (param.DefaultDisplayOnly && resolution.Display != 0) continue;
    printf("%s\n", resolution.ToString(param.JsonOutput, param.FullInfo, --count == 0).c_str());
  }
  if (param.JsonOutput) printf("}\n");

  return 0;
}
