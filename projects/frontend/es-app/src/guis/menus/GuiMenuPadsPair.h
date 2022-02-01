//
// Created by bkg2k on 13/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <guis/GuiWaitLongExecution.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuPadsPair : public GuiMenuBase
                      , ILongExecution<std::string, bool>
                      , IGuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuPadsPair(WindowManager& window, const Strings::Vector& deviceList);

  private:
    enum class Components
    {
      Devices,
    };

    //! Device list
    Strings::Vector mDevices;

    /*
     * ILongExecution implementation
     */

    /*!
     * @brief Try to pair a device
     * @param from Source interface
     * @param parameter Device to pair
     * @return True if the device is paired, fals eotherwise
     */
    bool Execute(GuiWaitLongExecution<std::string, bool>& from, const std::string& parameter) override;

    /*!
     * @brief Process result of pairing
     * @param parameter Device to paired
     * @param result Pairing result
     */
    void Completed(const std::string& parameter, const bool& result) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;
};