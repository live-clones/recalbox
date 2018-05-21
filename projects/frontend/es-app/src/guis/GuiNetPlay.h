//
// Created by xizor on 20/05/18.
//

#ifndef EMULATIONSTATION_ALL_GUINETPLAY_H
#define EMULATIONSTATION_ALL_GUINETPLAY_H

#include <FileData.h>
#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "resources/Font.h"

class TextComponent;
class ButtonComponent;

class GuiNetPlay : public GuiComponent
{
public:
	GuiNetPlay(Window* window, FileData* file);

	bool input(InputConfig* config, Input input) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override;

private:

	NinePatchComponent mBackground;
	ComponentGrid mGrid;


	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mSubTitle;
	std::vector< std::shared_ptr<ButtonComponent> > mButtons;
	std::shared_ptr<ComponentGrid> mButtonGrid;
};

#endif //EMULATIONSTATION_ALL_GUINETPLAY_H
