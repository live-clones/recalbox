//
// Created by bkg2k on 24/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>

//! User object (reduced to Recalbox needs)
struct ScreenScraperUser
{
  int mThreads = 1;        //! maxthreads
  int mRequestDone = 0;    //! requeststoday
  int mMaxRatePerMin = 0;  //! maxrequestspermin
  int mMaxRatePerDay = 0;  //! maxrequestsperday
  std::string mRegion;     //! favregion

  ScreenScraperUser()
    : mThreads(1)
    , mRequestDone(0)
    , mMaxRatePerMin(0)
    , mMaxRatePerDay(0)
  {
  }

  ScreenScraperUser(int threads)
    : mThreads(threads)
    , mRequestDone(0)
    , mMaxRatePerMin(0)
    , mMaxRatePerDay(0)
  {
  }
};