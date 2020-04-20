//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <string>
#include <mqtt/Countdown.h>
#include <mqtt/MQTTClient/MQTTClient.h>
#include "TcpNetwork.h"
#include <utils/Log.h>
#include <utils/os/system/Thread.h>

class MqttClient : Thread, MQTT::Client<TCPNetwork, Countdown, 16 << 10, 5>
{
  private:
    //! MQTT Host
    static constexpr const char* sMqttHost = "127.0.0.1";
    //! MQTT Port
    static constexpr int sMqttPort = 1883;

    //! TCP socket simple wrapper
    TCPNetwork mNetwork;

    //! Client identifier
    const char* mClientId;

    /*!
     * @brief Lazy connection.
     * Check if we are connected, and if not, try to connect
     * @return Connection status
     */
    bool EnsureConnection();

    /*!
     * @brief Disconnect al
     */
    void Disconnect();

    /*
     * Thread implementation
     */

    /*!
     * Calls the inheriting object's Run() method.
     * @note set fIsRunning false to exit
     */
    void Run() override;

  public:
    /*!
     * @brief Default constructor
     */
    explicit MqttClient(const char* clientid);

    ~MqttClient() override
    {
      Disconnect();
      Thread::Stop();
    }

    /*!
     * @brief Send a string to the specified topic
     * @param topic Target topic
     * @param data UTF8 string to send
     * @return True if the string has been sent w/o error
     */
    bool Send(const std::string& topic, const std::string& data);

    /*!
     * @brief Subscribe and set callback to the given method
     * @tparam T Callback's class
     * @param topic Topic to subscibe to
     * @param item
     * @param method
     */
    template<class T> void Subscribe(const char* topic, T* item, void (T::*method)(MQTT::MessageData&))
    {
      EnsureConnection();
      if (subscribe(topic, MQTT::QOS0, item, method) != 0)
      {
        LOG(LogError) << "Error subscribing to: " << topic;
        return;
      }
      LOG(LogInfo) << "Subscribed to: " << topic;
    }
};
