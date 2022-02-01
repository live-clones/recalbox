//
// Created by bkg2k on 30/04/2020.
//
#pragma once

class IMqttReceiver
{
  public:
    virtual void MqttReceive(const std::string& topic, const std::string& payload) = 0;
};
