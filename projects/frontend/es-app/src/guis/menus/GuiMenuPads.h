//
// Created by bkg2k on 12/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <guis/GuiWaitLongExecution.h>
#include <input/InputMapper.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuPads : public GuiMenuBase, ILongExecution<bool, Strings::Vector>, IInputChange
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuPads(WindowManager& window);

  private:
    //! Configure a pad
    std::shared_ptr<TextComponent> mConfigure;
    //! Pair a pad
    std::shared_ptr<TextComponent> mPair;
    //! Unpair all pads
    std::shared_ptr<TextComponent> mUnpair;
    //! Pad lists
    std::shared_ptr<OptionListComponent<int>> mDevices[Input::sMaxInputDevices];

    //! Pad Mapper
    InputMapper mMapper;
    //! Refreshing component, ignore change event
    bool mRefreshing;

    //! Called when the user start configuring a pad
    static void StartConfiguring(GuiMenuPads* thiz);

    //! Detect the device to configure
    static void RunDeviceDetection(GuiMenuPads* thiz);

    //! Refresh the device list
    static void RefreshDevices(GuiMenuPads* thiz);

    //! Called when the user start configuring a pad
    static void StartScanningDevices(GuiMenuPads* thiz);

    //! Unpair all device
    static void UnpairAll(GuiMenuPads* thiz);

    /*!
     * @brief Pad has been changed at position Index
     * @param thiz Class instance
     * @param index Index of changed pad
     */
    static void ChangePadAt(GuiMenuPads* thiz, int index);

    /*
     * ILongExecution
     */

    /*!
     * @brief Scan bluetooth devices
     * @param from Source interface
     * @param parameter Useless "in" parameter
     * @return List of available devices
     */
    Strings::Vector Execute(GuiWaitLongExecution<bool, Strings::Vector>& from, const bool& parameter) override;

    /*!
     * @brief Called when the device scan is complete
     * @param parameter Useless "in" parameter
     * @param result List of available devices
     */
    void Completed(const bool& parameter, const Strings::Vector& result) override;

    /*
     * IInputChange implementation
     */

    //! Refresh pad list
    void PadsAddedOrRemoved() override;
};



