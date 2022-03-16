//
// Created by bkg2k on 15/03/2022.
//

#include "ResolutionAdapter.h"
#include "Resolutions.h"
#include <SDL2/SDL.h>
#include <utils/math/Misc.h>
#include <utils/os/fs/Path.h>
#include <utils/Log.h>

#ifdef USE_KMSDRM

#include <xf86drmMode.h>
#include <libdrm/drm.h>
#include <fcntl.h>

const ResolutionAdapter::ResolutionList& ResolutionAdapter::GetResolutionList()
{
  static ResolutionList resolutions;

  Path connectorMainPath("/dev/dri");
  Path::PathList connectorList = connectorMainPath.GetDirectoryContent();

  int monitor = 0;
  for(const Path& connectorPath : connectorList)
  {
    // Open connector
    int fd = open(connectorPath.ToChars(), O_RDWR);
    if (fd < 0) continue; // Error opening connector

    // Get connector resources
    drmModeResPtr resource = drmModeGetResources(fd);
    if (resource)
    {
      { LOG(LogDebug) << "[DRM] Open card '" << connectorPath.ToString() << "' with " << resource->count_connectors << " connectors"; }
      // Run through connectors
      for(int i = 0; i < resource->count_connectors; ++i)
      {
       // Get connectors
        drmModeConnectorPtr connector = drmModeGetConnector(fd, resource->connectors[i]);
        if (connector)
        {
          { LOG(LogDebug) << "[DRM]   Open connector #" << i << " - Connected " << (connector->connection == DRM_MODE_CONNECTED ? "Yes" : "No"); }
          // List modes
          for(int m = 0; m < connector->count_modes; ++m)
          {
            drmModeModeInfo& mode = connector->modes[m];
            resolutions.push_back({ monitor, mode.hdisplay, mode.vdisplay, -1, (int)mode.vrefresh, (mode.flags & DRM_MODE_FLAG_INTERLACE) != 0, (mode.type & DRM_MODE_TYPE_DEFAULT) != 0 });
            { LOG(LogDebug) << "[DRM]     Mode #" << m << " : " << resolutions.back().ToString(); }
          }

          // Inc monitor index
          monitor++;
          // Free connector
          drmModeFreeConnector(connector);
        }
        else { LOG(LogDebug) << "[DRM]   Error opening connector " << i; }
      }
      // Free resources
      drmModeFreeResources(resource);
    }
    else { LOG(LogDebug) << "[DRM] Error opening card '" << connectorPath.ToString() << '\''; }
  }

  return resolutions;
}

#else // SDL2

const ResolutionAdapter::ResolutionList& ResolutionAdapter::GetResolutionList()
{
  SDL_Init(SDL_INIT_VIDEO);
  static ResolutionList resolutions;
  SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr };
  int displayCount = SDL_GetNumVideoDisplays();
  for (int i = 0; i < displayCount; ++i)
  {
    SDL_DisplayMode defaultMode = mode;
    SDL_GetDesktopDisplayMode(i, &defaultMode);
    int modesCount = SDL_GetNumDisplayModes(i);
    for (int m = 0; m < modesCount; ++m)
      if (SDL_GetDisplayMode(i, m, &mode) == 0)
        resolutions.push_back({ i, mode.w, mode.h, (int)SDL_BITSPERPIXEL(mode.format), mode.refresh_rate, false, mode.w == defaultMode.w && mode.h == defaultMode.h && mode.format == defaultMode.format && mode.refresh_rate == defaultMode.refresh_rate });
  }
  SDL_Quit();

  return resolutions;
}

#endif // USE_KMSDRM

bool ResolutionAdapter::AdjustResolution(int display, const std::string& value, const ResolutionList& list, Resolutions::SimpleResolution& output)
{
  // Process default
  if (value == "default")
    return false;

  // Convert resolution
  Resolutions::SimpleResolution sr = Resolutions::ConvertSimpleResolution(value);
  if (!sr.IsValid()) return false;

  // Check if the resolution match an existing resolution
  for(const Resolution& r : list)
    if (r.Display == display &&
        r.Width == sr.Width &&
        r.Height == sr.Height)
      return false; // No need to adjust

  // Check best higher height
  const Resolution* best = nullptr;
  for(const Resolution& r : list)
    if (r.Display == display && sr.Width == r.Width)
      if ((best == nullptr && r.Height > sr.Height) || (r.Height > sr.Height && r.Height < best->Height))
        best = &r;
  if (best != nullptr)
  {
    output = { best->Width, best->Height };
    return true;
  }

  // Check best lower height
  best = nullptr;
  for(const Resolution& r : list)
    if (r.Display == display && sr.Width == r.Width)
      if ((best == nullptr && r.Height < sr.Height) || (r.Height < sr.Height && r.Height > best->Height))
        best = &r;
  if (best != nullptr)
  {
    output = { best->Width, best->Height };
    return true;
  }

  // Get nearest resolution
  best = nullptr;
  int area = SDL_MAX_SINT32;
  for(const Resolution& r : list)
  {
    int dw = Math::absi(r.Width - sr.Width);
    int dh = Math::absi(r.Height - sr.Height);
    int delta = dw * dw + dh * dh;
    if (r.Display == display && delta < area)
    {
      area = delta;
      best = &r;
    }
  }
  if (best != nullptr)
  {
    output = { best->Width, best->Height };
    return true;
  }

  // Something goes wrong...
  return false;
}

