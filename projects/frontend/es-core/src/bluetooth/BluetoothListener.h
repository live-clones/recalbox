//
// Created by bkg2k on 05/01/23.
//
#pragma once

#include <mqtt/IMQTTMessageReceived.h>
#include <mqtt/MqttClient.h>
#include "DeviceStatus.h"
#include "BluetoothOverlayGUI.h"
#include "IBluetoothDeviceStatusListener.h"

class BluetoothListener
  : public StaticLifeCycleControler<BluetoothListener>
  , private IMQTTMessageReceived
  , ISyncMessageReceiver<void>
{
  public:
    //! Constructor
    BluetoothListener();

    /*!
     * @brief Register a new listener
     * @param listener new listener
     */
    void Register(IBluetoothDeviceStatusListener* listener)
    {
      if (!mListeners.Contains(listener))
        mListeners.Add(listener);
    }

    void Unregister(IBluetoothDeviceStatusListener* listener)
    {
      if (int i = mListeners.IndexOf(listener); i >= 0)
        mListeners.Delete(i);
    }

  private:
    //! Bluetooth pairing MQTT receiver
    MqttClient mBluetoothPairing;
    //! IBluetoothDeviceStatusListener callback
    Array<IBluetoothDeviceStatusListener*> mListeners;
    //! Message stack
    std::vector<DeviceStatus> mMessages;
    //! Message locker
    Mutex mLocker;
    //! Message Syncer
    SyncMessageSender<void> mSender;

    /*
     * MQTTMessageReceived implementation
     */

    /*!
     * @brief MQTT message received
     * @param topic Message topic
     * @param data Message body
     */
    void MqttMessageReceived(const String& topic, const String& data) final;

    /*
     * ISyncMessageReceiver<void>
     */

    void ReceiveSyncMessage() override;
};
