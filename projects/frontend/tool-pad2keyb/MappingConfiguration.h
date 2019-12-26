//
// Created by bkg2k on 21/12/2019.
//
#pragma once

#include "PadItems.h"
#include "PadConstants.h"
#include "Pad.h"
#include "VirtualKeyboard.h"

class MappingConfiguration
{
  private:
    //! Pad2Keyboard configuration file extention
    static constexpr const char* ConfigurationExtensions = ".p2k.cfg";

    /*!
     * @brief Mapping structure
     */
    struct Mapping
    {
      //! Pad item to keycode
      int PadItemToKeyCodes[(int)PadItems::__Count];

      Mapping();

      bool Valid() const;
    };

    //! Mapping
    Mapping mMapping[PadConstants::MaxPadSupported];

    /*!
     * @brief Load all configuration/overrides from root path to rom path
     * @param path rom path
     */
    void Load(const Path& path, bool folder = false);

    /*!
     * @brief Try to assign a new mapping using key as pad item and value as keycode name
     * @param key Pad item ("X:paditem", where X is the pad number)
     * @param value Key code
     */
    void AssignMapping(const std::string& key, const std::string& value);

    /*!
     * @brief Parse keycode name and translate to KEY_XXXXX constants
     * @param keyname Key name ("backspace", "a", "leftshift", ...), uppercase mandatory!
     * @param code Output code.
     * @return True if the keyname has been converted successfully.
     */
    static bool ParseKeyCode(const std::string& keyname, int& code);

    /*!
     * @brief Parse pad item number & name and translate it to a PadItems enum value
     * @param padItemName Pad item name ("0:up", "1:x", "0:j1left", ...)
     * @param num Output pad number
     * @param items Output PadItems value
     * @return True if the parsing has been successful
     */
    static bool ParsePadItems(const std::string& padItemName, int& num, PadItems& items);

  public:
    /*!
     * @brief Constructor
     * @param romPath Rom path, use to search for c onfiguration files
     */
    explicit MappingConfiguration(const char* romPath);

    /*!
     * @brief Translate a pad event into the configured keycode if any
     * @param padnumber Pad number
     * @param event inpout event
     * @return Keyboard event
     */
    VirtualKeyboard::Event Translate(Pad::Event& event) const;

    /*!
     * @brief Check if the mapping has been properly loaded & configured
     * @return True if the mapping is configured.
     */
    bool Valid() const;
};
