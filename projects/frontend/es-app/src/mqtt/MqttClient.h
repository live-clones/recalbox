//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <string>
#include "TcpNetwork.h"
#include <utils/Log.h>
#include <utils/os/system/Thread.h>
#include <mqtt/paho/cpp/async_client.h>

class MqttClient : mqtt::iaction_listener
{
  private:
    //! MQTT Host
    static constexpr const char* sMqttHost = "127.0.0.1";
    //! MQTT Port
    static constexpr int sMqttPort = 1883;

    //! MQTT Client
    mqtt::async_client mMqtt;

    //! Initial connection token
    mqtt::token_ptr mOriginalTocken;

  public:
    /*!
     * @brief Default constructor
     */
    explicit MqttClient(const char* clientid);

    ~MqttClient() override = default;

    /*!
     * @brief Send a string to the specified topic
     * @param topic Target topic
     * @param data UTF8 string to send
     * @return True if the string has been sent w/o error
     */
    bool Send(const std::string& topic, const std::string& data);

  private:
    /*
     * mqtt::iaction_listener implementation
     */

    // Connection failure
    void on_failure(const mqtt::token& asyncActionToken) override;

    // Connection success
    void on_success(const mqtt::token& asyncActionToken) override;

    /*!
     * @brief Subscribe and set callback to the given method
     * @tparam T Callback's class
     * @param topic Topic to subscibe to
     * @param item
     * @param method
     */
    /*template<class T> void Subscribe(const char* topic, T* item, void (T::*method)(MQTT::MessageData&))
    {
      EnsureConnection();
      if (subscribe(topic, MQTT::QOS0, item, method) != 0)
      {
        { LOG(LogError) << "Error subscribing to: " << topic; }
        return;
      }
      { LOG(LogInfo) << "Subscribed to: " << topic; }

    }*/
};
