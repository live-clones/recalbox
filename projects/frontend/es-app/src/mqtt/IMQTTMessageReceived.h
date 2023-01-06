//
// Created by bkg2k on 07/12/22.
//
#pragma once

#include <utils/String.h>

class IMQTTMessageReceived
{
  public:
    //! Destructor
    virtual ~IMQTTMessageReceived() = default;

    /*!
     * @brief MQTT message received
     * @param topic Message topic
     * @param data Message body
     */
    virtual void MqttMessageReceived(const String& topic, const String& data) = 0;
};