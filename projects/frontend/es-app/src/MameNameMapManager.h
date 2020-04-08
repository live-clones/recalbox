//
// Created by bkg2k on 24/11/18.
//
#pragma once

#include <string>
#include <utils/storage/HashMap.h>

class MameNameMapManager
{
  private:
    static constexpr int sMameListSize = 30444;

    static const char* mMameNameToRealName[sMameListSize * 2];
    static int mMameNameHashes[sMameListSize];

    static HashMap<std::string, bool> mMameDevices;
    static HashMap<std::string, bool> mMameBios;

    static void GenerateMameNameHashes();
  public:

    static const char* GetCleanMameName(const std::string& from);

    static bool IsGame(const std::string& name)
    {
      return (!mMameBios.contains(name) && !mMameDevices.contains(name));
    }
};