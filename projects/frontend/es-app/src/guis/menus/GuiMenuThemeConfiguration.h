//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <guis/menus/GuiMenuBase.h>

class GuiMenuThemeConfiguration : public GuiMenuBase
                                , private IOptionListComponent<std::string>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    GuiMenuThemeConfiguration(WindowManager& window, const std::string& theme);

    //! Destructor
    ~GuiMenuThemeConfiguration() override;

  private:
    enum class Components
    {
      ColorSet,
      IconSet,
      MenuSet,
      SystemView,
      GamelistView,
      GameClipView,
      Region,
    };

    typedef std::map<std::string, std::string> StringMaps;
    typedef std::shared_ptr<OptionListComponent<std::string>> OptionList;
    typedef std::function<void()> Callback;

    //! Theme name
    std::string mThemeName;
    //! Changed flag
    bool mReloadRequired;

    //! Color Set
    OptionList mColorSet;
    //! Icon Set
    OptionList mIconSet;
    //! Menu Set
    OptionList mMenuSet;
    //! System View
    OptionList mSystemView;
    //! Gamelist View
    OptionList mGameListView;
    //! Gameclip View
    OptionList mGameClipView;
    //! Region
    OptionList mRegion;

    //! Color Set
    std::string mOriginalColorSet;
    //! Icon Set
    std::string mOriginalIconSet;
    //! Menu Set
    std::string mOriginalMenuSet;
    //! System View
    std::string mOriginalSystemView;
    //! Gamelist View
    std::string mOriginalGameListView;
    //! Gameclip View
    std::string mOriginalGameClipView;
    //! Region
    std::string mOriginalRegion;

    /*!
     * @brief Build an option menu
     * @param label Menu label
     * @param help Help msg
     * @param selected Currently selected item
     * @param items All items
     * @param id Menu id
     * @return OptionList component
     */
    OptionList BuildSelector(const std::string& label, const std::string& help, const std::string& selected, const StringMaps& items, Components id, std::string& original);

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;
};
