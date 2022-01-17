//
// Created by gugue_u on 17/01/2022.
//

#pragma once

#include "guis/menus/GuiMenuBase.h"

class WindowManager;
class SystemManager;

class GuiCheckMenu : public GuiMenuBase
{
  public:
    //! Constructor
    GuiCheckMenu(WindowManager& window,
                 const std::string& title,
                 int lastChoice,
                 const std::string& name1,
                 const std::string& help1,
                 const std::function<void()>& func1,
                 const std::string& name2,
                 const std::string& help2,
                 const std::function<void()>& func2);

    GuiCheckMenu(WindowManager& window,
                 const std::string& title,
                 int lastChoice,
                 const std::string& name1,
                 const std::string& help1,
                 const std::function<void()>& func1,
                 const std::string& name2,
                 const std::string& help2,
                 const std::function<void()>& func2,
                 const std::string& name3,
                 const std::string& help3,
                 const std::function<void()>& func3);

    GuiCheckMenu(WindowManager& window,
                 const std::string& title,
                 int lastChoice,
                 const std::string& name1,
                 const std::string& help1,
                 const std::function<void()>& func1,
                 const std::string& name2,
                 const std::string& help2,
                 const std::function<void()>& func2,
                 const std::string& name3,
                 const std::string& help3,
                 const std::function<void()>& func3,
                 const std::string& name4,
                 const std::string& help4,
                 const std::function<void()>& func4);

    void Update(int deltaTime) override;

private:
    //! Menu initialized flag
    bool mMenuInitialized;

    bool ProcessInput(const InputCompactEvent& event) override;
    void CloseAndCall(const std::function<void()>& func);

    void build(const std::string& name1, const std::string& help1, const std::function<void()>& func1,
               const std::string& name2, const std::string& help2, const std::function<void()>& func2,
               const std::string& name3, const std::string& help3, const std::function<void()>& func3,
               const std::string& name4, const std::string& help4, const std::function<void()>& func4,
               int lastChoice);
};
