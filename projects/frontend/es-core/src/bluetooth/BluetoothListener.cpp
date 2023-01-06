//
// Created by bkg2k on 05/01/23.
//

#include "BluetoothListener.h"

BluetoothListener::BluetoothListener()
  : StaticLifeCycleControler<BluetoothListener>("bluetooth")
  , mBluetoothPairing("recalbox-bluetooth", this)
  , mSender(*this)
{
  // Start listening to bluetooth messages
  mBluetoothPairing.Subscribe("bluetooth/status");
}


void BluetoothListener::MqttMessageReceived(const String& topic, const String& data)
{
  (void)topic;

  DeviceStatus status = DeviceStatus::Deserialize(data);
  if (status.IsValid())
  {
    Mutex::AutoLock locker(mLocker);
    mMessages.push_back(status);
    mSender.Send();
  }
}

void BluetoothListener::ReceiveSyncMessage()
{
  Mutex::AutoLock locker(mLocker);
  for(DeviceStatus& status : mMessages)
    for(int i = mListeners.Count(); --i >= 0;)
      mListeners[i]->ReceiveBluetoothDeviceStatus(status);
  mMessages.clear();
}
