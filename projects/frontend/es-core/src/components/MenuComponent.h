#pragma once

#include <guis/GuiInfoPopupBase.h>
#include <utils/Strings.h>
#include <components/NinePatchComponent.h>
#include <components/ComponentList.h>
#include <components/TextComponent.h>
#include <components/ComponentGrid.h>
#include <WindowManager.h>
#include <themes/MenuThemeData.h>
#include <guis/GuiMsgBoxScroll.h>
#include <components/DateTimeComponent.h>
#include <guis/GuiInfoPopup.h>

class ButtonComponent;
class ImageComponent;

std::shared_ptr<ComponentGrid> makeButtonGrid(WindowManager&window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons);
std::shared_ptr<ComponentGrid> makeMultiDimButtonGrid(WindowManager&window, const std::vector< std::vector< std::shared_ptr<ButtonComponent> > >& buttons, float outerWidth, float outerHeight);
std::shared_ptr<ImageComponent> makeArrow(WindowManager&window);

#define TITLE_VERT_PADDING (Renderer::Instance().DisplayHeightAsFloat()*0.0637f)

class MenuComponent : public Component
{
  public:
    MenuComponent(WindowManager&window, const std::string& title, const std::shared_ptr<Font>& titleFont);
    MenuComponent(WindowManager&window, const std::string& title)
      : MenuComponent(window, title, Font::get(FONT_SIZE_LARGE))
    {
    }

    void onSizeChanged() override;

    inline std::function<void()> buildHelpGui(const std::string& label, const std::string& help)
    {
      return [this, label, help] () {
        int dur = RecalboxConf::Instance().GetPopupHelp();
        if (dur != 0)
          mWindow.InfoPopupAdd(new GuiInfoPopup(mWindow, label + "\n" + help, dur, GuiInfoPopupBase::PopupType::Help));
        return true;
      };
    }

    inline void addRow(const ComponentListRow& row, bool setCursorHere = false, bool updateGeometry = true) { mList->addRow(row, setCursorHere, updateGeometry); if (updateGeometry) updateSize(); }

    inline void addRowWithHelp(ComponentListRow& row, const std::string& label, const std::string& help = "", bool setCursorHere = false, bool updateGeometry = true)
    {
      if (!help.empty()) {
        row.makeHelpInputHandler(buildHelpGui(label, help));
      }
      addRow(row, setCursorHere, updateGeometry);
    }

    inline void addWithLabel(const std::shared_ptr<Component>& comp, const std::string& label, const std::string& help = "", bool setCursorHere = false, bool invert_when_selected = true, const std::function<void()>& acceptCallback = nullptr)
    {
      ComponentListRow row;
      auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
      row.addElement(std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(label), menuTheme->menuText.font, menuTheme->menuText.color), true);
      row.addElement(comp, false, invert_when_selected);
      if (acceptCallback) {
        row.makeAcceptInputHandler(acceptCallback);
      }
      if (!help.empty()) {
        row.makeHelpInputHandler(buildHelpGui(label, help));
      }
      addRow(row, setCursorHere, true);
    }

    inline void addWithLabel(const std::shared_ptr<Component>& comp, const Path& iconPath, const std::string& label, const std::string& help = "", bool setCursorHere = false, bool invert_when_selected = true, const std::function<void()>& acceptCallback = nullptr)
    {
      ComponentListRow row;
      auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

      if (!iconPath.IsEmpty())
      {
        // icon
        auto icon = std::make_shared<ImageComponent>(mWindow);
        icon->setImage(iconPath);
        icon->setColorShift(menuTheme->menuText.color);
        icon->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
        row.addElement(icon, false, invert_when_selected);

        // spacer between icon and text
        auto spacer = std::make_shared<Component>(mWindow);
        spacer->setSize(10 + Math::roundi(menuTheme->menuText.font->getLetterHeight() * 1.25f) - Math::roundi(icon->getSize().x()), 0);
        row.addElement(spacer, false, invert_when_selected);
      }

      row.addElement(std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(label), menuTheme->menuText.font, menuTheme->menuText.color), true);
      row.addElement(comp, false, invert_when_selected);

      if (acceptCallback) {
        row.makeAcceptInputHandler(acceptCallback);
      }
      if (!help.empty()) {
        row.makeHelpInputHandler(buildHelpGui(label, help));
      }
      addRow(row, setCursorHere, true);
    }

    void addButton(const std::string& label, const std::string& helpText, const std::function<void()>& callback);

    void setFooter(const std::string& label);

    void setTitle(const std::string& title, const std::shared_ptr<Font>& font = Font::get(FONT_SIZE_LARGE));

    inline void setCursorToList() { mGrid.setCursorTo(mList); }
    inline int getButtonsSize() { return mButtons.size(); }
    inline void setCursorToButtons() { assert(mButtonGrid); mGrid.setCursorTo(mButtonGrid); }
    inline void setCursorToButton(int index) {
        setCursorToButtons();
        mGrid.moveCursor(Vector2i(index, 0));
    }
    inline void clear() { mList->clear(); }

    bool getHelpPrompts(Help& help) override;

    /*!
     * @brief Notification of an input event
     * @param event Compact event
     * @return Implementation must return true if it consumed the event.
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
    * @brief Called once per frame. Override to implement your own drawings.
    * Call your base::Update() to ensure animation and childrens are updated properly
    * @param deltaTime Elapsed time from the previous frame, in millisecond
    */
    void Update(int deltaTime) override;

    void refresh()
    {
      updateGrid();
      updateSize();
    }

    void SetDefaultButton(int index);

protected:
    inline ComponentList* getList() const { return mList.get(); }

  private:

    void updateSize();
    void updateGrid();
    float getButtonGridHeight() const;
    float getFooterHeight() const;

    NinePatchComponent mBackground;
    ComponentGrid mGrid;

    std::shared_ptr<TextComponent> mTitle;
    std::shared_ptr<TextComponent> mBattery;
    std::shared_ptr<DateTimeComponent> mDateTime;
    std::shared_ptr<ComponentList> mList;
    std::shared_ptr<ComponentGrid> mButtonGrid;
    std::shared_ptr<TextComponent> mFooter;
    std::vector< std::shared_ptr<ButtonComponent> > mButtons;

    int mTimeAccumulator;
};
