//
// Created by bkg2k on 07/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <guis/IGuiArcadeVirtualKeyboardInterface.h>
#include <components/IEditableComponent.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuNetplay : public GuiMenuBase
                     , private IOptionListComponent<RecalboxConf::Relay>
                     , private ISwitchComponent
                     , private IGuiMenuBase
                     , private IEditableComponent
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuNetplay(WindowManager& window, SystemManager& systemManager);

  private:
    enum class Components
    {
      Enabled,
      UserName,
      Port,
      Mitm,
      Passwords,
      Hash,
    };

    //! System manager
    SystemManager& mSystemManager;

    //! Enabled
    std::shared_ptr<SwitchComponent> mEnabled;
    //! Login
    std::shared_ptr<EditableComponent> mLogin;
    //! Port
    std::shared_ptr<EditableComponent> mPort;
    //! Mitm
    std::shared_ptr<OptionListComponent<RecalboxConf::Relay>> mMitm;

    //! Get O/C List
    static std::vector<ListEntry<RecalboxConf::Relay>> GetMitmEntries();

    /*
     * IEditableComponent implementation
     */

    void EditableComponentTextChanged(int id, const std::string& text) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * IOptionListComponent implementation
     */

    void OptionListComponentChanged(int id, int index, const RecalboxConf::Relay& value) override;

    /*
     * ISWitchComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;
};



