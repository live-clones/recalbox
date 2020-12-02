#pragma once

#include <guis/GuiMsgBoxScroll.h>
#include <utils/locale/LocaleHelper.h>
#include "guis/Gui.h"
#include "components/MenuComponent.h"

// This is just a really simple template for a GUI that calls some save functions when closed.
class GuiSettings : public Gui
{
  public:
    GuiSettings(WindowManager& window, const std::string& title);
    ~GuiSettings() override; // just calls save();

    void save();
    inline void updatePosition() {mMenu.setPosition((Renderer::Instance().DisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mMenu.getSize().y()) / 2);};
    inline void addRow(const ComponentListRow& row) { mMenu.addRow(row); updatePosition();};
    inline void addRowWithHelp(ComponentListRow& row, const std::string& label, const std::string& help) { mMenu.addRowWithHelp(row, label, help); updatePosition();};
    inline void addWithLabel(const std::shared_ptr<Component>& comp, const std::string& label, const std::string& help = "") {
      mMenu.addWithLabel(comp, label, help, false, true, nullptr);
      updatePosition();
    };
    inline void addWithLabel(const std::shared_ptr<Component>& comp, const Path& iconPath, const std::string& label, const std::string& help = "") {
      mMenu.addWithLabel(comp, iconPath, label, help, false, true, nullptr);
      updatePosition();
    };
    inline void addSubMenu(const std::string& label, const std::function<void()>& func, const std::string& help = "") {
		ComponentListRow row;
		row.makeAcceptInputHandler(func);
		auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
		const std::string& helpLabel(label);
		if(!help.empty()) {
			row.makeHelpInputHandler([this, help, helpLabel] {
				mWindow.pushGui(new GuiMsgBoxScroll(
						mWindow, helpLabel, help, _("OK"),
						[] {}, "", nullptr, "", nullptr));
				return true;
			});
		}

		auto entryMenu = std::make_shared<TextComponent>(mWindow,
															 label,
															 menuTheme->menuText.font, menuTheme->menuText.color);
		row.addElement(entryMenu, true);
		row.addElement(makeArrow(mWindow), false);
        mMenu.addRowWithHelp(row, label, help);
		updatePosition();
    };
	inline void addSaveFunc(const std::function<void()>& func) { mSaveFuncs.push_back(func); };
        inline void setSave(bool sav) { doSave = sav; };

	bool ProcessInput(const InputCompactEvent& event) override;
	bool getHelpPrompts(Help& help) override;

	const MenuComponent& getMenu() const { return mMenu; }

private:
  bool doSave = true;
	MenuComponent mMenu;
	std::vector< std::function<void()> > mSaveFuncs;
};
