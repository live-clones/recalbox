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
#include <mqtt/MqttClient.h>

enum class Command
{
    StartDiscovery     = 0x00001,
    StopDiscovery      = 0x00002,
};

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuPads : public GuiMenuBase
                  , ILongExecution<bool, Strings::Vector>
                  , IInputChange
                  , IOptionListComponent<int>
                  , IOptionListComponent<std::string>
                  , IGuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuPads(WindowManager& window);

  private:
    enum class Components
    {
      Pads,
      Configure = (int)Pads + Input::sMaxInputDevices,
      Pair,
      Unpair,
      Driver,
    };

    //! Pad lists
    std::shared_ptr<OptionListComponent<int>> mDevices[Input::sMaxInputDevices];

    //! Pad Mapper
    InputMapper mMapper;
    //! Refreshing component, ignore change event
    bool mRefreshing;

    //! MQTT Topic
    static constexpr const char* sEventTopic = "bluetooth/operation";

    //! Called when the user start configuring a pad
    void StartConfiguring();

    //! Detect the device to configure
    static void RunDeviceDetection(GuiMenuPads* thiz);

    //! Refresh the device list
    void RefreshDevices();

    //! Called when the user starts bluetooth discovery
    void StartScanningDevices();

    //! Unpair all device
    void UnpairAll();

    //! Get modes
    static std::vector<GuiMenuBase::ListEntry<std::string>> GetModes();

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

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * IOptionListComponent<int> implementation
     */

    void OptionListComponentChanged(int id, int index, const int& value) override;

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * @brief convert action enum to string
     */
    const char* ActionToString(Command action);
};



