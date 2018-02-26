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
	virtual ~GuiSettings(); // just calls save();

	void save();
	inline void addRow(const ComponentListRow& row) { mMenu.addRow(row); };
	inline void addRowWithHelp(ComponentListRow& row, const std::string& label, const std::string& help) { mMenu.addRowWithHelp(row, label, help); };
	inline void addWithLabel(const std::shared_ptr<GuiComponent>& comp, const std::string& label, const std::string& help = "") {
		mMenu.addWithLabel(comp, label, help, false, true, nullptr);
	};
    inline void addSubMenu(const std::string& label, const std::function<void()>& func, std::string help = "") {
		ComponentListRow row;
		row.makeAcceptInputHandler(func);
		auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
		std::string helpLabel(label);
		if(help != "") {
			row.makeHelpInputHandler([this, help, helpLabel] {
				mWindow->pushGui(new GuiMsgBoxScroll(
						mWindow, helpLabel, help.c_str(), _("OK"),
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
