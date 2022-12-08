//
// Created by Bkg2k on 11/03/2020.
//

#include "MqttClient.h"
#include <utils/Log.h>
#include <mqtt/paho/cpp/connect_options.h>

MqttClient::MqttClient(const char* clientId, IMQTTMessageReceived* callback)
  : mSender(*this)
  , mMqtt("tcp://127.0.0.1:1883", clientId, 0, nullptr)
  , mCallbackInterface(callback)
{
  // Set options
  mqtt::connect_options connectOptions;
  connectOptions.set_automatic_reconnect(true);  // Auto-reconnect
  connectOptions.set_clean_session(true); // Clean session: do not receive old messages

  // Connect
  try
  {
    if (mMqtt.connect(connectOptions, nullptr, *this) != nullptr)
      { LOG(LogError) << "[MQTT] Connexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " successful !"; }
    else
      { LOG(LogError) << "[MQTT] Connexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed (init) !"; }
  }
  catch(std::exception& e)
  {
    { LOG(LogError) << "[MQTT] Connexion to " << mMqtt.get_server_uri() << " from " << mMqtt.get_client_id() << " failed (catch) !"; }
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
      mSender.Send();
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

void MqttClient::Subscribe(const char* topic)
{
  if (!mMqtt.is_connected())
  {
    if (std::find(mPendingSubs.begin(), mPendingSubs.end(), String(topic)) == mPendingSubs.end())
      mPendingSubs.push_back(topic);
    { LOG(LogInfo) << "[MQTT] Pending subscribing to: " << topic; }
    return;
  }

  if (mMqtt.subscribe(topic, 0, this, *this) == nullptr)
  {
    LOG(LogError) << "[MQTT] Error subscribing to: " << topic;
    return;
  }
  { LOG(LogInfo) << "[MQTT] Subscribed to: " << topic; }
  mMqtt.set_message_callback([this](mqtt::const_message_ptr msg)
                             {
                               MessageReceived(msg);
                             });
}

void MqttClient::MessageReceived(mqtt::const_message_ptr& msg)
{
  if (mCallbackInterface != nullptr)
    mCallbackInterface->MqttMessageReceived(String(msg->get_topic().data(), (int)msg->get_topic().length()),
                                            String(msg->get_payload_str().data(), (int)msg->get_payload_str().length()));
}

void MqttClient::ReceiveSyncMessage()
{
  String::List pendings = mPendingSubs;
  mPendingSubs.clear();
  for(const String& pending : pendings)
    Subscribe(pending.data());
}
