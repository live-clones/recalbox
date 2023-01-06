//
// Created by bkg2k on 05/01/23.
//

#include "DeviceStatus.h"
#include <rapidjson/document.h>
#include <utils/Log.h>

DeviceStatus DeviceStatus::Deserialize(const String& message)
{
  DeviceStatus result;

  rapidjson::Document json;
  json.Parse(message.data());
  if (!json.HasParseError())
  {
    { LOG(LogDebug) << "[Bluetooth] MQTT Message: " << message; }
    bool active = json.HasMember("is_discovering") && json["is_discovering"].GetBool();
    if (active)
    {
      // Pairing time
      result.mRemainingTime = json.HasMember("discovery_timeout") ? json["discovery_timeout"].GetInt() : -1;
      result.mTotalTime = json.HasMember("discovery_total") ? json["discovery_total"].GetInt() : -1;
      // Devices
      if (json.HasMember("devices"))
        for(const auto& node : json["devices"].GetArray())
        {
          Device newDevice;
          newDevice.UUID = node.HasMember("id") ? node["id"].GetString() : "<no-id>";
          newDevice.Name = node.HasMember("name") ? node["name"].GetString() : "<no-name>";
          newDevice.Alias = node.HasMember("alias") ? node["alias"].GetString() : newDevice.Name;
          newDevice.Address = node.HasMember("address") ? node["address"].GetString() : "<no-address>";
          int rssi = node.HasMember("rssi") ? node["rssi"].GetInt() : 10000;
          if      (rssi <   -90) newDevice.RSSI = SignalStrength::Awfull;
          else if (rssi <   -60) newDevice.RSSI = SignalStrength::Bad;
          else if (rssi <   -30) newDevice.RSSI = SignalStrength::Average;
          else if (rssi <     0) newDevice.RSSI = SignalStrength::Good;
          else if (rssi < 10000) newDevice.RSSI = SignalStrength::Best;
          else newDevice.RSSI = SignalStrength::Unknown;
          newDevice.Paired = node.HasMember("paired") && node["paired"].GetBool();
          newDevice.Trusted = node.HasMember("trusted") && node["trusted"].GetBool();
          newDevice.Connected = node.HasMember("connected") && node["connected"].GetBool();
          newDevice.Blocked = node.HasMember("blocked") && node["blocked"].GetBool();
          if (!newDevice.Blocked)
            result.mDevices.push_back(newDevice);
        }
    }
  }
  else { LOG(LogError) << "[DeviceStatus] Received invalid JSON " << message; }

  return result;
}
