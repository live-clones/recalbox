//
// Created by gugue_u on 08/02/2022.
//

#pragma once

#include "GuiMenuBase.h"
#include "systems/SystemManager.h"

class GuiMemuSmartCollection : public GuiMenuBase
  , private IEditableComponent
  , private IOptionListMultiComponent<std::string>


{
  public:

    explicit GuiMemuSmartCollection(WindowManager& window, SystemManager& systemManager, SmartCollection* smartCollection);

  private:

    ~GuiMemuSmartCollection() override;

    enum class Components
    {
        fullName,
        nameFilter,
        platformFilter
    };

    SystemManager& mSystemManager;

    SmartCollection* mSmartCollection;
    std::string mFullName;
    std::string mName;
    std::string mPlatform;

    std::vector<ListEntry<std::string>> GetSystemEntries();


    void OptionListMultiComponentChanged(int id, const std::vector<std::string>& value) override;
    void EditableComponentTextChanged(int id, const std::string& text) override;

    void Create();
};




