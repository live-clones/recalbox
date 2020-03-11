//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <string>
#include <mqtt/Countdown.h>
#include <mqtt/MQTTClient/MQTTClient.h>
#include "TcpNetwork.h"

class MqttClient : MQTT::Client<TCPNetwork, Countdown>
{
  private:
    //! MQTT Host
    static constexpr const char* sMqttHost = "127.0.0.1";
    //! MQTT Port
    static constexpr int sMqttPort = 1883;

    //! TCP socket simple wrapper
    TCPNetwork mNetwork;

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

  public:
    /*!
     * @brief Default constructor
     */
    MqttClient();

    ~MqttClient()
    {
      Disconnect();
    }

    /*!
     * @brief Send a string to the specified topic
     * @param topic Target topic
     * @param data UTF8 string to send
     * @return True if the string has been sent w/o error
     */
    bool Send(const std::string& topic, const std::string& data);
};
