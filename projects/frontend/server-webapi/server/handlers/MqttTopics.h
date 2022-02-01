//
// Created by bkg2k on 09/04/2020.
//
#pragma once

class MqttTopics
{
  public:
    //! MQTT Topic - Where to broadcast system info
    static constexpr const char* sSystemInfoTopic = "Recalbox/WebAPI/SystemInfo";

    //! MQTT Topic - Where to broardcast emulationstation event objects
    static constexpr const char* sEventObjectTopic = "Recalbox/WebAPI/EmulationStation/Event";

    //! MQTT Topic - From where to get EmulationStation events
    static constexpr const char* sEventTopic = "Recalbox/EmulationStation/Event";
};
