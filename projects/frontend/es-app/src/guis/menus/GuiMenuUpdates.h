//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuUpdates : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuUpdates(WindowManager& window);

  private:
    //! Enable updates
    std::shared_ptr<SwitchComponent> mEnableUpdate;
    //! Enable updates
    std::shared_ptr<TextComponent> mAvailable;
    //! Update type
    std::shared_ptr<OptionListComponent<std::string>> mType;

    //! Set update enabled
    static void SetEnabled(bool on);
    //! Set update enabled
    static void SetUpdateType(const std::string& type);

    //! Show changelog
    void ShowChangelog();
    //! Show changelog
    void StartUpdate();
};



