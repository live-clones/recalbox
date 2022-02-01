#pragma once

#include <utils/Strings.h>
#include <guis/Gui.h>
#include <components/NinePatchComponent.h>
#include <components/ComponentGrid.h>
#include <components/ComponentList.h>
#include <input/InputStack.h>
#include <utils/locale/LocaleHelper.h>

class TextComponent;

class GuiInputConfig : public Gui, private IComponentListRowEventInterceptor
{
public:
	GuiInputConfig(WindowManager& window, InputDevice* target, const std::function<void()>& doneCallback);
	~GuiInputConfig() override;
	void onSizeChanged() override;

private:

	class FormInput
	{
	public:
		const std::string name;
		const std::string label;
		const Path icon;
		const bool skippable;
		const InputEvent::EventType preferredType;
		FormInput(const std::string& inName, const std::string& inLabel, const Path& inIcon, bool inSkippable, InputEvent::EventType inPreferredType)
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
	inline void setAssignedTo(InputEvent input) { setText(Strings::ToUpperASCII(input.ToString()), mMainColor); }

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

    bool EventReceived(int id, const InputCompactEvent& event) override;

    NinePatchComponent mBackground;
	ComponentGrid mGrid;
	
	std::vector< FormInput > mFormInputs;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextComponent> mSubtitle1;
	std::shared_ptr<TextComponent> mSubtitle2;
	std::shared_ptr<ComponentList> mList;
	std::vector< std::shared_ptr<TextComponent> > mMappings;
	std::shared_ptr<ComponentGrid> mButtonGrid;

	InputDevice* mTargetDevice;
	InputStack mInputStack;

	bool mCursorOnList;
	unsigned int mMainColor;
};
