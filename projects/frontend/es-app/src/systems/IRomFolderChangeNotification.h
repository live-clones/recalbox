//
// Created by bkg2k on 14/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "utils/os/fs/Path.h"

class IRomFolderChangeNotification
{
  public:
    //! Destructor
    virtual ~IRomFolderChangeNotification() = default;

    /*!
     * @brief Notify a new rompath has been added
     * @param root rompath added
     */
    virtual void RomPathAdded(const DeviceMount& root) = 0;

    /*!
     * @brief Notify an existing rompath has been removed
     * @param root rompath removed
     */
    virtual void RomPathRemoved(const DeviceMount& root) = 0;

    /*!
     * @brief Notify a device has been added with no valid rom path found
     * @param deviceRoot Device mount point
     */
    virtual void NoRomPathFound(const DeviceMount& deviceRoot) = 0;
};