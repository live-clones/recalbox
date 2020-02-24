//
// Created by bkg2k on 21/12/2019.
//
#pragma once

#include <input/InputEvent.h>
#include <input/OrderedDevices.h>
#include "../Configuration.h"
#include "PadItems.h"

class PadConfiguration
{
  public:
    //! Pad item configuration
    struct PadItemConfiguration
    {
      PadItems Item;              //!< Pad item (up, x, j1left, ...)
      InputEvent::EventType Type; //!< Pad item type (axis, hat, button, ...)
      int Id;                     //!< Identifier
      int Value;                  //!< "ON" value
      int Code;                   //!< Low level code
    };

    //! All item configurations
    struct PadAllItemConfiguration
    {
      PadItemConfiguration Items[(int)PadItems::__Count]; //!< All items
    };

  private:
    //! Pad configuration file
    static constexpr const char* sInputFile = "/recalbox/share/system/.emulationstation/es_input.cfg";

    //! Configuration reference
    const Configuration* mConfiguration;

    //! All pad configuration
    PadAllItemConfiguration mPads[Input::sMaxInputDevices];

    //! Ready flag
    bool mReady;

    /*!
     * @brief Load pad configuration from es_input.cfg according to the global configuration
     */
    void Load();

  public:
    /*!
     * @brief Constructor
     * @param configuration Global configuration
     */
    explicit PadConfiguration(const Configuration& configuration);

    /*!
     * @brief Constructor
     * @param configuration Global configuration
     */
    PadConfiguration();

    /*!
     * @brief Get pad configuration bt index
     * @param index Index
     * @return Pad configuration
     */
    const PadAllItemConfiguration& Pad(int index) const { return mPads[(unsigned int)index <= Input::sMaxInputDevices ? index : 0]; }

    /*!
     * @brief Load pad configuration from es_input.cfg according to the global configuration
     */
    void Load(const OrderedDevices& orderedDevices);

    /*!
     * @brief Check if the pad configuration are loaded
     * @return True if the current configuration is ready
     */
    bool Ready() const { return mReady;}
};
