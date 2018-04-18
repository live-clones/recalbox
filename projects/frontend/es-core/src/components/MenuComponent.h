#pragma once

#include <guis/GuiInfoPopup.h>
#include <Settings.h>
#include "components/NinePatchComponent.h"
#include "components/ComponentList.h"
#include "components/TextComponent.h"
#include "components/ComponentGrid.h"
#include "Util.h"
#include "Window.h"
#include "MenuThemeData.h"
#include "Locale.h"
#include "guis/GuiMsgBoxScroll.h"
#include "DateTimeComponent.h"

class ButtonComponent;
class ImageComponent;

std::shared_ptr<ComponentGrid> makeButtonGrid(Window* window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons);
std::shared_ptr<ImageComponent> makeArrow(Window* window);

#define TITLE_VERT_PADDING (Renderer::getScreenHeight()*0.0637f)

class MenuComponent : public GuiComponent
{
public:
	MenuComponent(Window* window, const char* title, const std::shared_ptr<Font>& titleFont = Font::get(FONT_SIZE_LARGE));

	void onSizeChanged() override;
	
	inline const std::function<void()> buildHelpGui(const std::string& label, const std::string& help) 
	{
		return [this, label, help] () {
			int dur = Settings::getInstance()->getInt("HelpPopupTime");
			if (dur != 0) {
				auto s = std::make_shared<GuiInfoPopup>(mWindow, label + "\n" + help, dur);
				mWindow->setInfoPopup(s);
			}
			return true;
		};
	}

	inline void addRow(const ComponentListRow& row, bool setCursorHere = false, bool updateGeometry = true) { mList->addRow(row, setCursorHere, updateGeometry); if (updateGeometry) updateSize(); }

	inline void addRowWithHelp(ComponentListRow& row, const std::string& label, const std::string& help = "", bool setCursorHere = false, bool updateGeometry = true) 
	{
		if (help.size()) {
			row.makeHelpInputHandler(buildHelpGui(label, help));
		}
		addRow(row, setCursorHere, updateGeometry);
	}

	inline void addWithLabel(const std::shared_ptr<GuiComponent>& comp, const std::string& label, const std::string& help = "", bool setCursorHere = false, bool invert_when_selected = true, const std::function<void()>& acceptCallback = nullptr) 
	{
		ComponentListRow row;
		auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
		row.addElement(std::make_shared<TextComponent>(mWindow, strToUpper(_(label.c_str())), menuTheme->menuText.font, menuTheme->menuText.color), true);
		row.addElement(comp, false, invert_when_selected);
		if (acceptCallback) {
			row.makeAcceptInputHandler(acceptCallback);
		}
		if (help.size()) {
			row.makeHelpInputHandler(buildHelpGui(label, help));
		}
		addRow(row, setCursorHere);
	}

	void addButton(const std::string& label, const std::string& helpText, const std::function<void()>& callback);

	void setTitle(const char* title, const std::shared_ptr<Font>& font = Font::get(FONT_SIZE_LARGE));

	inline void setCursorToList() { mGrid.setCursorTo(mList); }
	inline void setCursorToButtons() { assert(mButtonGrid); mGrid.setCursorTo(mButtonGrid); }
	inline void clear() { mList->clear(); }

	virtual std::vector<HelpPrompt> getHelpPrompts() override;

protected:
	inline ComponentList* getList() const { return mList.get(); }

private:
	void updateSize();
	void updateGrid();
	float getButtonGridHeight() const;

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<DateTimeComponent> mDateTime;
	std::shared_ptr<ComponentList> mList;
	std::shared_ptr<ComponentGrid> mButtonGrid;
	std::vector< std::shared_ptr<ButtonComponent> > mButtons;
};
