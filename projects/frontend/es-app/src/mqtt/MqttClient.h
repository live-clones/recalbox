//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

#include <string>
#include "IMQTTMessageReceived.h"
#include "utils/os/system/Mutex.h"
#include "utils/sync/SyncMessageSender.h"
#include <mqtt/paho/cpp/async_client.h>
#include <mqtt/paho/cpp/connect_options.h>

class MqttClient
  : mqtt::iaction_listener
  , ISyncMessageReceiver<void>
{
  public:
    /*!
     * @brief Default constructor
     */
    MqttClient(const char* clientid, IMQTTMessageReceived* callback);

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
    void Subscribe(const char* topic);

  private:
    //! MQTT Host
    static constexpr const char* sMqttHost = "127.0.0.1";
    //! MQTT Port
    static constexpr int sMqttPort = 1883;

    SyncMessageSender<void> mSender;

    //! Pending subscribes
    String::List mPendingSubs;
    //! mPending subs locker
    Mutex mPendingLocker;

    //! MQTT Client
    mqtt::async_client mMqtt;

    //! Callback interface for message reception
    IMQTTMessageReceived* mCallbackInterface;

    /*
     * mqtt::iaction_listener implementation
     */

    // Connection failure
    void on_failure(const mqtt::token& asyncActionToken) override;

    // Connection success
    void on_success(const mqtt::token& asyncActionToken) override;

    /*!
     * @brief Call the interface when a message is received, translating topic and paypload into strings
     * @param msg Original message
     */
    void MessageReceived(mqtt::const_message_ptr& msg);

    /*!
     * @brief Receive a synchronized void message
     */
    void ReceiveSyncMessage() override;
};
