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
#include <unistd.h>

const ResolutionAdapter::ResolutionList& ResolutionAdapter::GetResolutionDetailedList()
{
  static ResolutionList resolutions;
  resolutions.clear();

  Path connectorMainPath("/dev/dri");
  Path::PathList connectorList = connectorMainPath.GetDirectoryContent();

  int connectedMonitors = 0;
  for(const Path& connectorPath : connectorList)
  {
    // Open connector
    int fd = open(connectorPath.ToChars(), O_RDWR);
    if (fd < 0) continue; // Error opening connector

    // Get connector resources
    drmModeResPtr resource = drmModeGetResources(fd);
    if (resource != nullptr)
    {
      { LOG(LogDebug) << "[DRM] Open card '" << connectorPath.ToString() << "' with " << resource->count_connectors << " connectors"; }
      // Run through connectors
      for(int connectorIndex = 0; connectorIndex < resource->count_connectors; ++connectorIndex)
      {
       // Get connectors
        drmModeConnectorPtr connector = drmModeGetConnector(fd, resource->connectors[connectorIndex]);
        if (connector != nullptr)
        {
          { LOG(LogDebug) << "[DRM]   Open connector #" << connectorIndex << " - Connected " << (connector->connection == DRM_MODE_CONNECTED ? "Yes" : "No"); }
          if (connector->connection == DRM_MODE_CONNECTED) {
            // List modes
            bool defaultFound = false;
            drmModeModeInfo *defaultMode;
            for (int m = 0; m < connector->count_modes; ++m) {
              drmModeModeInfo &mode = connector->modes[m];
              if ((mode.type & DRM_MODE_TYPE_DEFAULT) != 0)
                defaultMode = &mode;
              // 4k not supported on KMS boards
              if (mode.vdisplay > 2000) {
                { LOG(LogDebug) << "[DRM]      Skip resolution: " << mode.hdisplay << 'x' << mode.vdisplay; }
                continue;
              }
              resolutions.push_back({connectedMonitors, mode.hdisplay, mode.vdisplay, -1, (int) mode.vrefresh,
                                     (mode.flags & DRM_MODE_FLAG_INTERLACE) != 0,
                                     (mode.type & DRM_MODE_TYPE_DEFAULT) != 0});
              defaultFound |= (mode.type & DRM_MODE_TYPE_DEFAULT) != 0;
              { LOG(LogDebug) << "[DRM]     Mode #" << m << " : " << resolutions.back().ToString(); }
            }

            // Default 4K has been eliminated, let's set 1080p as default mode
            if (!defaultFound)
              for (Resolution &r: resolutions)
                if (r.Height == 1080 && r.Width == 1920) {
                  {
                    LOG(LogDebug) << "[DRM]      Setting non desktop default resolution: " << r.Width << 'x'
                                  << r.Height;
                  }
                  defaultFound = true;
                  r.IsDefault = true;
                  break;
                }
            // 4K has been eliminated, but 1080p not found
            // This should not happen, but we cannot allow it, we put back the desktop resolution
            if (!defaultFound)
              resolutions.push_back(
                  {connectedMonitors, defaultMode->hdisplay, defaultMode->vdisplay, -1, (int) defaultMode->vrefresh,
                   (defaultMode->flags & DRM_MODE_FLAG_INTERLACE) != 0, true});
            // Inc connected monitors index
            connectedMonitors++;
          }
          else { LOG(LogDebug) << "[DRM]   Skipping unconnected connector " << connectorIndex; }
          // Free connector
          drmModeFreeConnector(connector);
        }
        else { LOG(LogDebug) << "[DRM]   Error opening connector " << connectorIndex; }
      }
      // Free resources
      drmModeFreeResources(resource);
    }
    else { LOG(LogDebug) << "[DRM] Error opening card '" << connectorPath.ToString() << '\''; }

    close(fd);
  }

  return resolutions;
}

#else // SDL2

const ResolutionAdapter::ResolutionList& ResolutionAdapter::GetResolutionDetailedList()
{
  { LOG(LogDebug) << "[ResolutionAdapter] Getting resolution from sdl"; }

  if (mResolutionsDetailed.empty())
  {
    SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, nullptr };
    int displayCount = SDL_GetNumVideoDisplays();
    for (int i = 0; i < displayCount; ++i)
    {
      SDL_DisplayMode defaultMode = mode;
      SDL_GetDesktopDisplayMode(i, &defaultMode);
      int modesCount = SDL_GetNumDisplayModes(i);
      for (int m = 0; m < modesCount; ++m)
        if (SDL_GetDisplayMode(i, m, &mode) == 0)
          mResolutionsDetailed.push_back({i, mode.w, mode.h, (int)SDL_BITSPERPIXEL(mode.format), mode.refresh_rate, false, mode.w == defaultMode.w && mode.h == defaultMode.h && mode.format == defaultMode.format && mode.refresh_rate == defaultMode.refresh_rate });
    }
  }

  return mResolutionsDetailed;
}

#endif // USE_KMSDRM

bool ResolutionAdapter::AdjustResolution(int display, const std::string& value, Resolutions::SimpleResolution& output)
{
  { LOG(LogDebug) << "[ResolutionAdapter] Adjusting " << value; }
  // Process default
  if (value == "default")
  {
    Resolution res = GetDefaultResolution();
    { LOG(LogDebug) << "[ResolutionAdapter] Default resolution asked, adjusting with current default resolution: " << res.Width << "x" << res.Height; }
    output.Height = res.Height;
    output.Width = res.Width;
    return true;
  }

  // Convert resolution
  Resolutions::SimpleResolution sr = Resolutions::ConvertSimpleResolution(value);
  if (!sr.IsValid())
  {
    { LOG(LogDebug) << "[ResolutionAdapter] " << value << " is invalid!"; }
    return false;
  }

  const ResolutionList& list = GetResolutionDetailedList();

  // Check if the resolution match an existing resolution
  for(const Resolution& r : list)
    if (r.Display == display &&
        r.Width == sr.Width &&
        r.Height == sr.Height)
    {
      output = { sr.Width, sr.Height };
      { LOG(LogDebug) << "[ResolutionAdapter] " << value << " already match an existing resolution"; }
      return true; // No need to adjust
    }

  // Check best higher height
  const Resolution* best = nullptr;
  for(const Resolution& r : list)
    if (r.Display == display && sr.Width == r.Width)
      if ((best == nullptr && r.Height > sr.Height) || (r.Height > sr.Height && r.Height < best->Height))
        best = &r;
  if (best != nullptr)
  {
    output = { best->Width, best->Height };
    { LOG(LogDebug) << "[ResolutionAdapter] Adjusted to " << output.Width << 'x' << output.Height; }
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
    { LOG(LogDebug) << "[ResolutionAdapter] Adjusted to " << output.Width << 'x' << output.Height; }
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
    { LOG(LogDebug) << "[ResolutionAdapter] Adjusted to " << output.Width << 'x' << output.Height; }
    return true;
  }

  // Something goes wrong...
  { LOG(LogDebug) << "[ResolutionAdapter] Not adjusting " << value << " because something goes wrong!"; }
  return false;
}

const ResolutionAdapter::Resolution ResolutionAdapter::GetDefaultResolution()
{
  ResolutionList allResolutions = GetResolutionDetailedList();
  for(const Resolution& rawResolution : allResolutions){
    if(rawResolution.IsDefault) return rawResolution;
  }
}

const ResolutionAdapter::ResolutionList& ResolutionAdapter::GetResolutionList()
{
  if(mResolutions.empty())
  {
    ResolutionList allResolutions = GetResolutionDetailedList();
    for(const Resolution& rawResolution : allResolutions)
    {
      bool alreadyPresent = false;
      // if mResolutions.contains(rawResolution)
      for(Resolution& filteredResolution : mResolutions)
      {
        if(filteredResolution.Width == rawResolution.Width && filteredResolution.Height == rawResolution.Height)
        {
          // The resolution already exists, we should update the frequency value if it's higher
          if(filteredResolution.Frequency < rawResolution.Frequency) {
            filteredResolution.Frequency = rawResolution.Frequency;
            if(filteredResolution.Bpp < rawResolution.Bpp)
              filteredResolution.Bpp = rawResolution.Bpp;
            if(filteredResolution.Interlaced && !rawResolution.Interlaced)
              filteredResolution.Interlaced = false;
          }
          alreadyPresent = true;
          break;
        }
      }
      if(!alreadyPresent)
        mResolutions.push_back(rawResolution);
      }
    }
  return mResolutions;
}

