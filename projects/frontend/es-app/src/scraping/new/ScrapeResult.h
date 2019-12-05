//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <utils/os/fs/Path.h>

class ScrapeResult
{
  public:
    enum class Status
    {
      Ok,
      QuotaReached,
      Error,
    };

    Status mStatus;
    Path mPath;
    Path mImage;
    Path mVideo;
    Path mMarquee;
    Path mWheel;
    std::string mName;
    std::string mDeveloper;
    std::string mEditor;
    std::string mDescription;

    void Reset(const Path& path)
    {
      mPath = path;
      mImage = Path::Empty;
      mVideo = Path::Empty;
      mMarquee = Path::Empty;
      mWheel = Path::Empty;
      mName.clear();
      mDeveloper.clear();
      mEditor.clear();
      mDescription.clear();
    }

    ScrapeResult()
      : mStatus(Status::Ok)
    {
      Reset(Path());
    }
};
