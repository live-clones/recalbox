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
                     , private ISwitchComponent
                     , private IGuiMenuBase
                     , private IOptionListComponent<std::string>
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuUpdates(WindowManager& window);

  private:
    enum class Components
    {
      Enable,
      Version,
      Changelog,
      StartUpdate,
      UpdateType,
    };

    //! Enable updates
    std::shared_ptr<SwitchComponent> mEnableUpdate;
    //! Enable updates
    std::shared_ptr<TextComponent> mAvailable;
    //! Update type
    std::shared_ptr<OptionListComponent<std::string>> mType;

    //! Get Update type List
    static std::vector<ListEntry<std::string>> GetUpdateTypeEntries();

    /*
     * ISwitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;
};



