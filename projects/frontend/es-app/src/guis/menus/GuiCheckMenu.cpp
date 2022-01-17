//
// Created by gugue_u on 17/01/2022.
//

#include <utils/locale/LocaleHelper.h>
#include <WindowManager.h>

#include "GuiCheckMenu.h"

GuiCheckMenu::GuiCheckMenu(WindowManager& window,
                           const std::string& title,
                           int lastChoice,
                           const std::string& name1,
                           const std::string& help1,
                           const std::function<void()>& func1,
                           const std::string& name2,
                           const std::string& help2,
                           const std::function<void()>& func2)

                           : GuiMenuBase(window, title, nullptr)
                           , mMenuInitialized(false)
{
    build(name1, help1, func1,
          name2, help2, func2,
          std::string(), std::string(), nullptr,
          std::string(), std::string(), nullptr,
          lastChoice);

}

GuiCheckMenu::GuiCheckMenu(WindowManager& window,
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
                           const std::function<void()>& func3)

                           : GuiMenuBase(window, title, nullptr)
                           , mMenuInitialized(false)
{
    build(name1, help1, func1,
          name2, help2, func2,
          name3, help3, func3,
          std::string(), std::string(), nullptr,
          lastChoice);

}

GuiCheckMenu::GuiCheckMenu(WindowManager& window,
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
                           const std::function<void()>& func4)

                           : GuiMenuBase(window, title, nullptr)
                           , mMenuInitialized(false)
{
    build(name1, help1, func1,
          name2, help2, func2,
          name3, help3, func3,
          name4, help4, func4,
          lastChoice);
}

bool GuiCheckMenu::ProcessInput(const InputCompactEvent& event)
{
    if (event.AnyDownPressed() || event.AnyUpPressed())
    {
        return true;
    }

    if (event.CancelPressed() || event.StartPressed())
    {
        Close();
        return true;
    }

    return GuiMenuBase::ProcessInput(event);
}


// because GuiMenuBase::Update reset buttons grid cursor on fist loop
void GuiCheckMenu::Update(int deltaTime) {
    if (!mMenuInitialized)
    {
        mMenu.setPosition((Renderer::Instance().DisplayWidthAsFloat() - mMenu.getSize().x()) / 2,
                          (Renderer::Instance().DisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
        setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());
        mMenuInitialized = true;
    }

    Component::Update(deltaTime);
}

void GuiCheckMenu::CloseAndCall(const std::function<void()>& func)
{
    Close();
    if (func) func();
}

void GuiCheckMenu::build(const std::string &name1, const std::string &help1, const std::function<void()> &func1,
                         const std::string &name2, const std::string &help2, const std::function<void()> &func2,
                         const std::string &name3, const std::string &help3, const std::function<void()> &func3,
                         const std::string &name4, const std::string &help4, const std::function<void()> &func4,
                         int lastChoice) {

    mMenu.addButton(name1, help1, std::bind(&GuiCheckMenu::CloseAndCall, this, func1));
    mMenu.addButton(name2, help2, std::bind(&GuiCheckMenu::CloseAndCall, this, func2));

    if(!name3.empty())
        mMenu.addButton(name3, help3, std::bind(&GuiCheckMenu::CloseAndCall, this, func3));

    if(!name4.empty())
        mMenu.addButton(name4, help4, std::bind(&GuiCheckMenu::CloseAndCall, this, func4));

    mMenu.setCursorToButtons();
    mMenu.SetDefaultButton(lastChoice);
}
