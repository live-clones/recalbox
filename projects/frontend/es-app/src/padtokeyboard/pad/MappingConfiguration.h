//
// Created by bkg2k on 21/12/2019.
//
#pragma once

#include "../mouse/VirtualMouse.h"
#include "../keyboard/VirtualKeyboard.h"
#include "PadItems.h"
#include "Pad.h"

class MappingConfiguration
{
  public:
    //! Mapping type
    enum class Types
    {
      None,
      Keyboard,         //!< Maps to one or more keyboard key, pressed "simultaneously"
      KeyboardSequence, //!< Maps to a key sequence, one press & release one after each other
      MouseButton,      //!< Maps to a mouse button
      MouseMove,        //!< Maps to a mouse move
    };

    //! Mouse move direction
    enum class MouseMove
    {
      Up    = 1,
      Right = 2,
      Down  = 4,
      Left  = 8,
    };

    //! Hint type
    typedef std::vector<std::pair<std::pair<int, PadItems>, const std::string*>> Hints;

    /*!
     * @brief Constructor
     * @param romPath Rom path, use to search for c onfiguration files
     */
    explicit MappingConfiguration(const Path& romPath);

    /*!
     * @brief Translate pad event into kayboard or mouse event
     * @param event Input event
     * @param keyboard Keyoard event to fill in (if the mapping maps to a kayboard event)
     * @param mouse Mouse event to fill in (if the mapping mas to a mouse event)
     * @return Mapping type (keyoard, mouse button or mouse move)
     */
    MappingConfiguration::Types Translate(Pad::Event& event, VirtualKeyboard::EventList& keyboard, VirtualMouse::Event& mouse) const;

    /*!
     * @brief Get comment per mapping list
     * @return Hashmap
     */
    const Hints& HintList() const;

    /*!
     * @brief Check if the mapping has been properly loaded & configured
     * @return True if the mapping is configured.
     */
    bool Valid() const;

    /*!
     * @brief Return mapping count
     * @return Mapping count
     */
    int Count() const;

  private:
    //! Pad2Keyboard configuration file extention
    static constexpr const char* ConfigurationExtensions = ".p2k.cfg";

    /*!
     * @brief Mapping structure
     */
    class  Mapping
    {
      public:
        //! For convenience only
        typedef short CodeArray[VirtualKeyboard::sMax];

        /*!
         * @brief Default constructor
         */
        Mapping();

        /*!
         * @brief Check if the mapping array contains at least one valid mapping
         * @return
         */
        bool Valid() const;

        /*!
         * @brief Get number of mapped pad items
         * @return Mapped pad item count
         */
        int Count() const;

        /*!
         * @brief Assign an item
         * @param padItem Pad item
         * @param type Type of target
         * @param code Target code
         * @param comment Comment (doc)
         */
        void Assign(PadItems padItem, Types type, const CodeArray codes, int count, int delay, const std::string& comment)
        {
          int index = (int)padItem;
          mCount[index] = count;
          mDelay[index] = delay;
          mType[index] = type;
          memcpy(&mCodes[index][0], codes, sizeof(CodeArray));
          mComment[index] = comment;
        }

        //! Get item type
        Types Type(PadItems item) const { return mType[(int)item]; }
        //! Get item delay
        int Delay(PadItems item) const { return mDelay[(int)item]; }
        //! Get item code count
        int Count(PadItems item) const { return mCount[(int)item]; }
        //! Get item codes
        int Code(PadItems item, int index) const { return mCodes[(int)item][index % VirtualKeyboard::sMax]; }
        //! Get item comment
        const std::string& Comment(PadItems item) const { return mComment[(int)item]; }

      private:
        //! Mapping type
        Types mType[(int)PadItems::__Count];
        //! Code count
        int mCount[(int)PadItems::__Count];
        //! Delay between press for multiple key codes
        int mDelay[(int)PadItems::__Count];
        //! Pad item to keycode
        CodeArray mCodes[(int)PadItems::__Count];
        //! Pad item to comment (mini-doc)
        std::string mComment[(int)PadItems::__Count];
    };

    //! Mapping
    Mapping mMapping[Input::sMaxInputDevices];

    /*!
     * @brief Load all configuration/overrides from root path to rom path
     * @param path rom path
     */
    void Load(const Path& path, bool folder = false);

    /*!
     * @brief Try to assign a new mapping using key as pad item and value as keycode name
     * @param key Pad item ("X:paditem", where X is the pad number)
     * @param value Key code
     * @param comment Comment/doc
     */
    void AssignMapping(const std::string& key, const std::string& value, const std::string& comment);

    /*!
     * @brief Parse keycode name and translate to KEY_XXXXX constants
     * @param keyname Key name ("backspace", "a", "leftshift", ...), uppercase mandatory!
     * @param code Output code.
     * @param type Target mapping: kayboard, mouse button or mouse move
     * @param count Code count
     * @param delay Delay between key press/release
     * @return True if the keyname has been converted successfully.
     */
    static bool ParseKeyCode(const std::string& keyname, Mapping::CodeArray codes, Types& type, int& count, int& delay);

    /*!
     * @brief Parse pad item number & name and translate it to a PadItems enum value
     * @param padItemName Pad item name ("0:up", "1:x", "0:j1left", ...)
     * @param num Output pad number
     * @param items Output PadItems value
     * @return True if the parsing has been successful
     */
    static bool ParsePadItems(const std::string& padItemName, int& num, PadItems& items);
};
