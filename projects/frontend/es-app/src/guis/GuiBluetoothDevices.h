//
// Created by bkg2k on 06/01/23.
//
#pragma once

#include <bluetooth/IBluetoothDeviceStatusListener.h>
#include <guis/Gui.h>
#include <components/NinePatchComponent.h>
#include <components/ComponentGrid.h>
#include "components/ComponentList.h"
#include "themes/MenuThemeData.h"

class GuiBluetoothDevices final
  : public Gui
  , public IBluetoothDeviceStatusListener
{
  public:
    /*!
     * @brief Constructor
     * @param windowManager Window manager
     */
    explicit GuiBluetoothDevices(WindowManager& windowManager);

    ~GuiBluetoothDevices() final;

  private:
    //! Background frame
    NinePatchComponent mBackground;
    //! Full grid (entire frame)
    ComponentGrid mGrid;
    //! Title
    std::shared_ptr<TextComponent> mTitle;
    //! Device list
    std::shared_ptr<ComponentList> mList;
    //! Details
    std::shared_ptr<TextComponent> mDetails;
    //! Low button
    std::shared_ptr<ComponentGrid> mButtons;
    //! Menu theme
    std::shared_ptr<MenuTheme> mMenuTheme;

    //! Latest devices info
    std::vector<DeviceStatus::Device> mDevices;

    /*!
     * @brief Compose string name of a given device, including its name and all appropriate icons
     * @param device Device to compose name from
     * @return Device name
     */
    static String ComposeName(const DeviceStatus::Device& device);

    /*!
     * @brief Build all components
     */
    void BuildComponents();

    /*!
     * @brief Fill the whole device list, trying to keep cursor position when possible
     */
    void FillDeviceList();

    /*!
     * @brief Fill current selectionselection details
     */
    void FillDetails();

    /*
     * Component override
     */

    //! Render override
    void Render(const Transform4x4f& parentTrans) override;

    //! Called when the Gui size changes (at least once at start)
    void onSizeChanged() override;

    /*
     * IBluetoothDeviceStatusListener implementation
     */

    /*!
     * @brief Receive latest device status
     * @param status DeviceStatus instance
     */
    void ReceiveBluetoothDeviceStatus(DeviceStatus& status) final
    {
      mDevices = status.Devices();
      FillDeviceList();
    }
};
