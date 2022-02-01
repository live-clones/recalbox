//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <string>
#include "TcpNetwork.h"
#include "IMqttReceiver.h"
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

    //! Receiver
    IMqttReceiver* mReceiver;

  public:
    /*!
     * @brief Default constructor
     * @param clientid Client ID
     * @param receiver Receiver interface
     */
    explicit MqttClient(const char* clientid, IMqttReceiver* receiver = nullptr);

    /*!
     * @brief Default destructor
     */
    ~MqttClient() override;

    /*!
     * @brief Send a string to the specified topic
     * @param topic Target topic
     * @param data UTF8 string to send
     * @return True if the string has been sent w/o error
     */
    bool Send(const std::string& topic, const std::string& data);

    /*!
     * @brief Subscribe to a topic
     * @param topic Topic to subscibe to
     * @return True if the subscribing is successful, false otherwise
     */
    bool Subscribe(const char* topic);

  private:
    /*
     * mqtt::iaction_listener implementation
     */

    //! Connection failure
    void on_failure(const mqtt::token& asyncActionToken) override;

    //! Connection success
    void on_success(const mqtt::token& asyncActionToken) override;
};
