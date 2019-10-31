#pragma once

#include <utils/StringUtil.h>
#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "components/ComponentList.h"
#include "input/InputStack.h"
#include "Locale.h"

class TextComponent;

class GuiInputConfig : public GuiComponent
{
public:
	GuiInputConfig(Window* window, InputDevice* target, const std::function<void()>& doneCallback);
	~GuiInputConfig() override;
	void onSizeChanged() override;

private:

	class FormInput
	{
	public:
		const std::string name;
		const std::string label;
		const std::string icon;
		const bool skippable;
		const InputEvent::EventType preferredType;
		FormInput(const std::string& inName, const std::string& inLabel, const std::string& inIcon, bool inSkippable, InputEvent::EventType inPreferredType)
		  : name(inName),
		    label(inLabel),
		    icon(inIcon),
		    skippable(inSkippable),
		    preferredType(inPreferredType)
		{
		};
	};

	inline void setMapped() { setText(_("ALREADY TAKEN"), mMainColor); }
	inline void setSkipped() { setText(_("(skipped)"), mMainColor); }
	inline void setNotDefined() { setText("", mMainColor); }
	inline void setAssignedTo(InputEvent input) { setText(StringUtil::toUpper(input.ToString()), mMainColor); }

	void initFormInputs();
	void addFormInput(const char* name, const std::string& label, const char* icon, bool skippable, InputEvent::EventType preferredType);

	void setHelpMessage();
	void setPress();
	void setText(const std::string& msg, unsigned int color);
	void setText(const std::string& msg, unsigned int color, int inputId);

	bool assign(InputEvent input);
	void unAssign();
	void restaurePreviousAssignment();
	bool isAssigned();

	void rowDone();

	NinePatchComponent mBackground;
	ComponentGrid mGrid;
	
	std::vector< FormInput > mFormInputs;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mSubtitle1;
	std::shared_ptr<TextComponent> mSubtitle2;
	std::shared_ptr<ComponentList> mList;
	std::vector< std::shared_ptr<TextComponent> > mMappings;
	std::shared_ptr<ComponentGrid> mButtonGrid;

	InputDevice* mTargetConfig;
	InputStack mInputStack;

	bool mCursorOnList;
	unsigned int mMainColor;
};
