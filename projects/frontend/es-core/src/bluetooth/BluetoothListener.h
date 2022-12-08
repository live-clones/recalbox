//
// Created by bkg2k on 05/01/23.
//
#pragma once

#include <mqtt/IMQTTMessageReceived.h>
#include <mqtt/MqttClient.h>
#include "DeviceStatus.h"
#include "BluetoothOverlayGUI.h"

class BluetoothListener: private IMQTTMessageReceived
{
  public:
    //! Constructor
    BluetoothListener();

    void SetNotifier(BluetoothOverlayGUI& gui) { mGui = &gui; }

  private:
    //! Bluetooth pairing MQTT receiver
    MqttClient mBluetoothPairing;
    //! Bluetooth overlay gui reference
    BluetoothOverlayGUI* mGui;

    /*
     * MQTTMessageReceived implementation
     */

    /*!
     * @brief MQTT message received
     * @param topic Message topic
     * @param data Message body
     */
    void MqttMessageReceived(const String& topic, const String& data) final;
};
