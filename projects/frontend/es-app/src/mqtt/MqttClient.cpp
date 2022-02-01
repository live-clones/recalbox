//
// Created by Bkg2k on 11/03/2020.
//

#include "MqttClient.h"
#include <mqtt/paho/cpp/connect_options.h>

MqttClient::MqttClient(const char* clientId)
  : mMqtt("tcp://127.0.0.1:1883", clientId, 0, nullptr)
{
  // Set options
  mqtt::connect_options connectOptions;
  connectOptions.set_automatic_reconnect(true);  // Auto-reconnect
  connectOptions.set_clean_session(true); // Clean session: do not receive old messages

  // Connect
  try
  {
    mOriginalTocken = mMqtt.connect(connectOptions, nullptr, *this);
  }
  catch(std::exception& e)
  {
    { LOG(LogError) << "[MQTT] Connexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed (ctor) !"; }
  }
}

bool MqttClient::Send(const std::string& topic, const std::string& message)
{
  try
  {
    mMqtt.publish(topic, message.data(), message.size(), 0, false, nullptr, *this);
    return true;
  }
  catch(std::exception& e)
  {
    { LOG(LogError) << "[MQTT] Sending messageConnexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed (send) !"; }
  }
  return false;
}

void MqttClient::on_failure(const mqtt::token& asyncActionToken)
{
  switch(asyncActionToken.get_type())
  {
    case mqtt::token::CONNECT:
    {
      { LOG(LogError) << "[MQTT] Connexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed!"; }
      break;
    }
    case mqtt::token::SUBSCRIBE:
    {
      { LOG(LogError) << "[MQTT] Subscribing to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed!"; }
      break;
    }
    case mqtt::token::PUBLISH:
    {
      { LOG(LogError) << "[MQTT] Publishing to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed!"; }
      break;
    }
    case mqtt::token::UNSUBSCRIBE:
    case mqtt::token::DISCONNECT:
    default:
    {
      { LOG(LogError) << "[MQTT] Unknown failure in action " << asyncActionToken.get_type() << " to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id(); }
      break;
    }
  }
}

void MqttClient::on_success(const mqtt::token& asyncActionToken)
{
  switch(asyncActionToken.get_type())
  {
    case mqtt::token::CONNECT:
    {
      { LOG(LogDebug) << "[MQTT] Connexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " OK!"; }
      break;
    }
    case mqtt::token::SUBSCRIBE:
    {
      { LOG(LogInfo) << "[MQTT] Subscribing to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " OK!"; }
      break;
    }
    case mqtt::token::PUBLISH:
    {
      { LOG(LogInfo) << "[MQTT] Publishing to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " OK!"; }
      break;
    }
    case mqtt::token::UNSUBSCRIBE:
    case mqtt::token::DISCONNECT:
    default:
    {
      { LOG(LogInfo) << "[MQTT] Unknown success in action " << asyncActionToken.get_type() << " to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id(); }
      break;
    }
  }
}

