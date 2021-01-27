//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
template<class T> class OptionListComponent;
class SwitchComponent;
class TextComponent;

class GuiMenuSystem : public GuiMenuBase
                    , private IOptionListComponent<std::string>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuSystem(WindowManager& window);

    //! Destructeur
    ~GuiMenuSystem() override;

  private:
    enum class Components
    {
      Storage,
      Culture,
      Keyboard,
    };

    //! Kayboard name
    struct Keyboard
    {
      std::string Displayable; //!< Kayboard displayable name
      std::string Identifier;  //!< Keyboard identifier
    };
    //! Keyboard list
    typedef std::vector<Keyboard> KeyboardList;

    //! Culture structure
    struct Culture
    {
      std::string Displayable; //!< Language name
      std::string Identifier;  //!< Culture identifier
    };
    //! Culture list
    typedef std::vector<Culture> CultureList;

    //! Version
    std::shared_ptr<TextComponent> mVersion;
    //! Share Free Space
    std::shared_ptr<TextComponent> mFreeSpace;
    //! Available storages
    std::shared_ptr<OptionListComponent<std::string>> mStorages;
    //! Culture
    std::shared_ptr<OptionListComponent<std::string>> mCulture;
    //! Keyboard
    std::shared_ptr<OptionListComponent<std::string>> mKeyboard;

    //! Original storage
    std::string mOriginalStorage;
    //! Original culture
    std::string mOriginalCulture;
    //! Original keyboard
    std::string mOriginalKeyboard;

    //! Get Storage List
    std::vector<ListEntry<std::string>> GetStorageEntries();
    //! Get Culture List
    std::vector<ListEntry<std::string>> GetCultureEntries();
    //! Get Keyboard List
    std::vector<ListEntry<std::string>> GetKeyboardEntries();

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;
};



