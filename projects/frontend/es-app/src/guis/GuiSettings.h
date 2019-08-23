#pragma once

#include <guis/GuiMsgBoxScroll.h>
#include <Locale.h>
#include "GuiComponent.h"
#include "components/MenuComponent.h"

// This is just a really simple template for a GUI that calls some save functions when closed.
class GuiSettings : public GuiComponent
{
public:
	GuiSettings(Window* window, const char* title);
	~GuiSettings() override; // just calls save();

	void save();
	inline void updatePosition() {mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);};
	inline void addRow(const ComponentListRow& row) { mMenu.addRow(row); updatePosition();};
	inline void addRowWithHelp(ComponentListRow& row, const std::string& label, const std::string& help) { mMenu.addRowWithHelp(row, label, help); updatePosition();};
	inline void addWithLabel(const std::shared_ptr<GuiComponent>& comp, const std::string& label, const std::string& help = "") {
		mMenu.addWithLabel(comp, label, help, false, true, nullptr);
		updatePosition();
	};
    inline void addSubMenu(const std::string& label, const std::function<void()>& func, const std::string& help = "") {
		ComponentListRow row;
		row.makeAcceptInputHandler(func);
		auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
		const std::string& helpLabel(label);
		if(!help.empty()) {
			row.makeHelpInputHandler([this, help, helpLabel] {
				mWindow->pushGui(new GuiMsgBoxScroll(
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

	bool input(InputConfig* config, Input input) override;
	std::vector<HelpPrompt> getHelpPrompts() override;

	inline MenuComponent *getMenu() const { return &mMenu; }

private:
        bool doSave = true;
	mutable MenuComponent mMenu;
	std::vector< std::function<void()> > mSaveFuncs;
};
