//
// Created by bkg2k on 05/01/23.
//

#include "BluetoothListener.h"
#include "rapidjson/document.h"

BluetoothListener::BluetoothListener()
  : mBluetoothPairing("recalbox-bluetooth", this)
  , mGui(nullptr)
{
  // Start listening to bluetooth messages
  mBluetoothPairing.Subscribe("bluetooth/status");
}


void BluetoothListener::MqttMessageReceived(const String& topic, const String& data)
{
  (void)topic;

  DeviceStatus status = DeviceStatus::Deserialize(data);
  if (status.IsValid() && mGui != nullptr)
    mGui->UpdatePairingTime(status.RemainingTime(), status.TotalTime());
}
