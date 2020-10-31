//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <guis/menus/GuiMenuBase.h>

class GuiMenuThemeConfiguration : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    GuiMenuThemeConfiguration(Window& window, const std::string& theme);

    //! Destructor
    ~GuiMenuThemeConfiguration() override;

  private:
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
    //! Region
    OptionList mRegion;

    OptionList BuildSelector(const std::string& label, const std::string& selected, const StringMaps& items, const Callback& callback);

    /*!
     * @brief Save & set Color Set
     * @param time new Color set
     */
    void SetColorSet(const std::string& colorSet);

    /*!
     * @brief Set & save Icon set
     * @param time new Icon set
     */
    void SetIconSet(const std::string& iconSet);

    /*!
     * @brief Set & save Menu Set
     * @param time new Menu set
     */
    void SetMenuSet(const std::string& menuSet);

    /*!
     * @brief Set & save System View
     * @param time new System view
     */
    void SetSystemView(const std::string& SystemView);

    /*!
     * @brief Set & save Gamelist View
     * @param time new Gamelist view
     */
    void SetGameListView(const std::string& gameListView);

    /*!
     * @brief Set & save Region
     * @param time new Region
     */
    void SetRegion(const std::string& region);
};
