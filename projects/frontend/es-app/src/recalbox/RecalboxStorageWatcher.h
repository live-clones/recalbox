//
// Created by bkg2k on 27/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class WindowManager;
class MountMonitor;
class Path;

class RecalboxStorageWatcher
{
  public:
    /*!
     * @brief Check free space on the device holding the given path
     * and bring a warning popup if the free space goes under the minimum requirtements
     * @param window Window manager
     * @param mountMonitor Mount monitor (providing mount devices)
     * @param path Path to check
     */
    static void CheckStorageFreeSpace(WindowManager& window, MountMonitor& mountMonitor, const Path& path);

  private:
    //! Storage minimum size
    static constexpr long long sStorageRequiredMinimumFreeSize = (512 << 20); // 512Mo minimum
    //! Share minimum size
    static constexpr long long sShareRequiredMinimumFreeSize = sStorageRequiredMinimumFreeSize + (3 << 30); // 3Go + minimum
};



