//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/storage/Set.h>
#include <utils/os/system/Mutex.h>

class ProtectedSet
{
  public:
    bool Exists(const std::string& string)
    {
      mMutex.Lock();
      bool result = mSet.contains(string);
      if (!result) mSet.insert(string);
      mMutex.UnLock();
      return result;
    }

  private:
    //! String set
    HashSet<std::string> mSet;

    //! Set protector
    Mutex mMutex;
};
