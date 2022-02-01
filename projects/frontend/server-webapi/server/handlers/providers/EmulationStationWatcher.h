//
// Created by bkg2k on 10/04/2020.
//
#pragma once

#include <utils/os/system/Thread.h>
#include <utils/storage/HashMap.h>
#include <utils/IniFile.h>
#include <mqtt/MqttClient.h>
#include <utils/json/JSONBuilder.h>

class EmulationStationWatcher : private IMQTTMessageReceived
{
  private:
    //! EmulationStation info file
    static constexpr const char* sEmulationStationEventFile = "/tmp/es_state.inf";

    //! Simple method datatype
    typedef JSONBuilder (*SimpleMethodReference)(const IniFile& data);

    //! MQTT client reference
    MqttClient mMqttClient;

    //! Fast event to method lookup
    HashMap<std::string, SimpleMethodReference> mEventMethod;

    /*!
     * @brief Mqtt message receiver
     * @param topic Topic
     * @param payload Message content
     */
    void MqttMessageReceived(const String& topic, const String& payload) override;

    /*
     * Tools
     */

    static JSONBuilder BuildSystemObject(const IniFile& data);

    static JSONBuilder BuildGameObject(const IniFile& data);

    /*
     * Individual event processing
     */

    static JSONBuilder EventStart(const IniFile& data);

    static JSONBuilder EventStop(const IniFile& data);

    static JSONBuilder EventShutdown(const IniFile& data);

    static JSONBuilder EventReboot(const IniFile& data);

    static JSONBuilder EventQuit(const IniFile& data);

    static JSONBuilder EventRelaunch(const IniFile& data);

    static JSONBuilder EventSystemBrowsing(const IniFile& data);

    static JSONBuilder EventGamelistBrowsing(const IniFile& data);

    static JSONBuilder EventRunGame(const IniFile& data);

    static JSONBuilder EventRunDemo(const IniFile& data);

    static JSONBuilder EventEndGame(const IniFile& data);

    static JSONBuilder EventEndDemo(const IniFile& data);

    static JSONBuilder EventSleep(const IniFile& data);

    static JSONBuilder EventWakup(const IniFile& data);

    static JSONBuilder EventScrapStart(const IniFile& data);

    static JSONBuilder EventScrapStop(const IniFile& data);

    static JSONBuilder EventScrapGame(const IniFile& data);

    static JSONBuilder EventConfigurationChanged(const IniFile& data);

  public:
    /*!
     * @brief Default constructor
     */
    EmulationStationWatcher();
};
