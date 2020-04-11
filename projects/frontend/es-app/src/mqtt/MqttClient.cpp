//
// Created by Bkg2k on 11/03/2020.
//

#include "MqttClient.h"

MqttClient::MqttClient(const char* clientId)
  : Client(mNetwork, 100),
    mClientId(clientId)
{
}

bool MqttClient::EnsureConnection()
{
  // TCP connect
  if (!mNetwork.isConnected())
    if (mNetwork.connect(sMqttHost, sMqttPort) != 0)
    {
      LOG(LogError) << "Cannot connect MQTT client (socket).";
      return false;
    }

  // MQTT connect
  if (!isConnected())
  {
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char*) mClientId;
    int rc = connect(data);
    if (rc != 0)
    {
      LOG(LogError) << "Cannot connect MQTT client (mqtt packet).";
      Disconnect();
      return false;
    }
    LOG(LogInfo) << "Connected to MQTT server.";
  }

  return true;
}

void MqttClient::Disconnect()
{
  // MQTT Disconnect
  if (isConnected()) disconnect();
  // TCP disconnect
  if (mNetwork.isConnected()) mNetwork.disconnect();
}

bool MqttClient::Send(const std::string& topic, const std::string& data)
{
  static unsigned short staticid = 0;
  MQTT::Message message
  {
    .qos = MQTT::QOS0,
    .retained = false,
    .dup = false,
    .id = ++staticid,
    .payload = (void*)data.c_str(),
    .payloadlen = data.size() + 1,
  };
  EnsureConnection();
  int rc = publish(topic.c_str(), message);
  if (rc != 0)
  {
    LOG(LogError) << "Cannot send MQTT data: " << data;
    Disconnect();
    return false;
  }
  return true;
}

void MqttClient::Yield(int timeout)
{
  EnsureConnection();
  yield(timeout);
}
