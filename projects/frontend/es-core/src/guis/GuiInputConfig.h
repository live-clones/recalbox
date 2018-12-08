#pragma once

#include "GuiComponent.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "components/ComponentList.h"
#include "InputStack.h"
#include "Locale.h"
#include "Util.h"

class TextComponent;

class GuiInputConfig : public GuiComponent
{
public:
	GuiInputConfig(Window* window, InputConfig* target, const std::function<void()>& doneCallback);
	void onSizeChanged() override;

private:

	class FormInput
	{
	public:
		const std::string name;
		const std::string label;
		const std::string icon;
		const bool skippable;
		const InputType preferredType;
		FormInput(std::string inName, std::string inLabel, std::string inIcon, bool inSkippable, InputType inPreferredType): name(inName), label(inLabel), icon(inIcon), skippable(inSkippable), preferredType(inPreferredType) {};
	};

	inline void setMapped() { setText(_("ALREADY TAKEN"), mMainColor); }
	inline void setSkipped() { setText(_("(skipped)"), mMainColor); }
	inline void setNotDefined() { setText("", mMainColor); }
	inline void setAssignedTo(Input input) { setText(strToUpper(input.string()), mMainColor); }

	void initFormInputs();
	void addFormInput(const char* name, std::string label, const char* icon, bool skippable, InputType preferredType);

	void setHelpMessage();
	void setPress();
	void setText(const std::string& msg, unsigned int color);
	void setText(const std::string& msg, unsigned int color, const int inputId);

	bool assign(Input input);
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

	InputConfig* mTargetConfig;
	InputStack mInputStack;

	bool mCursorOnList;
	unsigned int mMainColor;
	unsigned int mSelectColor;
};
