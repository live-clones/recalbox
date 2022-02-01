#pragma once

#include "components/base/Component.h"
#include "WindowManager.h"
#include "components/TextComponent.h"
#include "components/ImageComponent.h"
#include "components/MenuComponent.h"
#include "utils/Log.h"
#include "utils/locale/LocaleHelper.h"
#include "themes/MenuThemeData.h"
#include "IOptionListComponent.h"
#include "IOptionListMultiComponent.h"

//Used to display a list of options.
//Can select one or multiple options.

// if !multiSelect
// * <- curEntry ->

// always
// * press a -> open full list

#define CHECKED_PATH ":/checkbox_checked.svg"
#define UNCHECKED_PATH ":/checkbox_unchecked.svg"

template<typename T>
class OptionListComponent : public Component
{
private:
	struct OptionListData
	{
		std::string name;
		std::string value;
		T object;
		bool selected = false;
	};

	class OptionListPopup : public Gui
	{
	private:
		MenuComponent mMenu;
		OptionListComponent<T>* mParent;

	public:
		OptionListPopup(WindowManager& window, OptionListComponent<T>* parent, const std::string& title)
		  : Gui(window),
			  mMenu(window, title),
			  mParent(parent)
		{
      addChild(&mMenu);

			auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
			auto font = menuTheme->menuText.font;
			auto color = menuTheme->menuText.color;
			ComponentListRow row;

			// for select all/none
			std::vector<ImageComponent*> checkboxes;

      for (auto& e : mParent->mEntries)
			{
				row.elements.clear();
				row.addElement(std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(e.name), font, color), true);

				if(mParent->mMultiSelect)
				{
					// add checkbox
					auto checkbox = std::make_shared<ImageComponent>(mWindow);
					checkbox->setImage(Path(e.selected ? CHECKED_PATH : UNCHECKED_PATH));
					checkbox->setResize(0, font->getLetterHeight());
					checkbox->setColor(color);
					row.addElement(checkbox, false);

					// input handler
					// update checkbox state & selected value
					row.makeAcceptInputHandler([this, &e, checkbox, color]
					{
						e.selected = !e.selected;
						checkbox->setImage(Path(e.selected ? CHECKED_PATH : UNCHECKED_PATH));
						checkbox->setColor(color);
						mParent->onSelectedChanged();
					});

					// for select all/none
					checkboxes.push_back(checkbox.get());
				}else{
					// input handler for non-multiselect
					// update selected value and close
					row.makeAcceptInputHandler([this, &e]
					{
						mParent->mEntries[mParent->getSelectedId()].selected = false;
						e.selected = true;
						mParent->onSelectedChanged();
						Close();
					});
				}

				// also set cursor to this row if we're not multi-select and this row is selected
				mMenu.addRow(row, (!mParent->mMultiSelect && e.selected), false);
			}

			if(mParent->mMultiSelect)
			{
			  mMenu.addButton(_("SELECT ALL"), "select all", [this, checkboxes, color]
			  {
					for (unsigned int i = 0; i < mParent->mEntries.size(); i++)
					{
						mParent->mEntries[i].selected = true;
						checkboxes[i]->setImage(Path(CHECKED_PATH));
						checkboxes[i]->setColor(color);
					}
					mParent->onSelectedChanged();
				});

			  mMenu.addButton(_("SELECT NONE"), "select none", [this, checkboxes, color]
			  {
					for (unsigned int i = 0; i < mParent->mEntries.size(); i++)
					{
						mParent->mEntries[i].selected = false;
						checkboxes[i]->setImage(Path(UNCHECKED_PATH));
						checkboxes[i]->setColor(color);
					}
					mParent->onSelectedChanged();
				});
			}

      mMenu.refresh();
			mMenu.setPosition((Renderer::Instance().DisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::Instance().DisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
		}

		bool ProcessInput(const InputCompactEvent& event) override
		{
			if (event.CancelPressed())
			{
				Close();
				return true;
			}

			return Component::ProcessInput(event);
		}

		bool getHelpPrompts(Help& help) override
		{
			mMenu.getHelpPrompts(help);
			help.Set(Help::Cancel(), _("BACK"));
			return true;
		}
	};

public:
	OptionListComponent(WindowManager& window, const std::string& name, bool multiSelect, unsigned int font_size)
	  : Component(window)
    , mName(name)
    , mText(window)
    , mLeftArrow(window)
    , mRightArrow(window)
    , mOriginColor(0)
    , mId(0)
    , mInterface(nullptr)
    , mMultiInterface(nullptr)
    , mMultiSelect(multiSelect)
	{
		std::shared_ptr<Font> font = nullptr;
		auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
		if (font_size == FONT_SIZE_SMALL)
			font = menuTheme->menuTextSmall.font;
		else
			font = menuTheme->menuText.font;
    unsigned int color = menuTheme->menuText.color;
		mOriginColor = color;
			
		mText.setFont(font);
		mText.setColor(color);
		mText.setHorizontalAlignment(TextAlignment::Center);
		addChild(&mText);

		if(mMultiSelect)
		{
			mRightArrow.setImage(menuTheme->iconSet.arrow);
			mRightArrow.setColorShift(color);
			addChild(&mRightArrow);
		}else{
			mLeftArrow.setImage(menuTheme->iconSet.option_arrow);
			mLeftArrow.setColorShift(color);
			mLeftArrow.setFlipX(true);
			addChild(&mLeftArrow);

			mRightArrow.setImage(menuTheme->iconSet.option_arrow);
			mRightArrow.setColorShift(color);
			addChild(&mRightArrow);
		}

		setSize(mLeftArrow.getSize().x() + mRightArrow.getSize().x(), font->getHeight());
	}

  OptionListComponent(WindowManager&window, const std::string& name, bool multiSelect)
    : OptionListComponent(window, name, multiSelect, FONT_SIZE_MEDIUM)
  {
  }

  OptionListComponent(WindowManager&window, const std::string& name)
    : OptionListComponent(window, name, false, FONT_SIZE_MEDIUM)
  {
  }

  // handles positioning/resizing of text and arrows
	void onSizeChanged() override
	{
		mLeftArrow.setResize(0, mText.getFont()->getLetterHeight());
		mRightArrow.setResize(0, mText.getFont()->getLetterHeight());

        /*if(mSize.x() < (mLeftArrow.getSize().x() + mRightArrow.getSize().x())) {
			{ LOG(LogWarning) << "[OptionListComponent] too narrow!"; }
        }*/

		mText.setSize(mSize.x() - mLeftArrow.getSize().x() - mRightArrow.getSize().x(), mText.getFont()->getHeight());

		// position
		mLeftArrow.setPosition(0, (mSize.y() - mLeftArrow.getSize().y()) / 2);
		mText.setPosition(mLeftArrow.getPosition().x() + mLeftArrow.getSize().x(), (mSize.y() - mText.getSize().y()) / 2);
		mRightArrow.setPosition(mText.getPosition().x() + mText.getSize().x(), (mSize.y() - mRightArrow.getSize().y()) / 2);
	}

	bool ProcessInput(const InputCompactEvent& event) override
	{
    if (event.ValidPressed())
    {
      open();
      return true;
    }
    if(!mMultiSelect)
    {
      if (event.AnyLeftPressed())
      {
        // move selection to previous
        unsigned int i = getSelectedId();
        int next = (int)i - 1;
        if(next < 0)
          next += mEntries.size();

        mEntries[i].selected = false;
        mEntries[next].selected = true;
        onSelectedChanged();
        return true;

      }else if (event.AnyRightPressed())
      {
        // move selection to next
        unsigned int i = getSelectedId();
        int next = (i + 1) % mEntries.size();
        mEntries[i].selected = false;
        mEntries[next].selected = true;
        onSelectedChanged();
        return true;

      }
    }
		return Component::ProcessInput(event);
	}

	std::vector<T> getSelectedObjects()
	{
		std::vector<T> ret;
		for (auto& comp : mEntries)
		{
			if(comp.selected)
				ret.push_back(comp.object);
		}

		return ret;
	}

        
	T getSelected()
	{
		assert(!mMultiSelect);
		auto selected = getSelectedObjects();
		if(selected.size() == 1) return selected[0];
    return T();
	}
        
	std::string getSelectedName()
	{
    assert(!mMultiSelect);
		for (auto& entry : mEntries)
		{
			if(entry.selected)
				return entry.name;
		}
    return "";
	}

	void select(T object) {
	    bool hasChanged = false;
	    bool selectable = true;
		for (auto& entry : mEntries) {
		    const bool previous = entry.selected;
			entry.selected = selectable && (entry.object == object);
			if (entry.selected && !mMultiSelect) {
                selectable = false;
			}
            hasChanged = hasChanged || (entry.selected != previous);
		}
		if (hasChanged) {
            onSelectedChanged();
		}
	}
        
	void clear() { 
		mEntries.clear();
	}

    void add(const std::string& name, const T& obj, bool selected, const std::string& value = "")
        {
		OptionListData e;
		e.name = name;
		e.value = value;
		e.object = obj;
		e.selected = selected;
		if (selected) {
			if (!mMultiSelect) {
				for (auto& entry : mEntries) {
					entry.selected = false;
				}
			}
			firstSelected = obj;
		}

		mEntries.push_back(e);
		onSelectedChanged();
	}

	inline void invalidate() {
		onSelectedChanged();
	}

	inline void setSelectedChangedCallback(const std::function<void(const T&)>& callback) {
		mSelectedChangedCallback = callback;
	}

	inline void setChangedCallback(const std::function<void()>& callback) { mChangedCallback = callback; };

  inline void SetInterface(int id, IOptionListComponent<T>* interface)
  {
    mId = id;
    mInterface = interface;
  }

  inline void SetMultiInterface(int id, IOptionListMultiComponent<T>* interface)
  {
    mId = id;
    mMultiInterface = interface;
  }

  bool changed() {return firstSelected != getSelected();}

  unsigned int getSelectedIndex() const { return getSelectedId(); }

private:
	unsigned int getSelectedId() const
	{
		assert(!mMultiSelect);
		for (unsigned int i = 0; i < mEntries.size(); i++)
		{
			if(mEntries[i].selected)
				return i;
		}

    { LOG(LogWarning) << "[OptionListComponent] no selected element found, defaulting to 0"; }
		return 0;
	}

	void open()
	{
		mWindow.pushGui(new OptionListPopup(mWindow, this, mName));
	}

	void onSelectedChanged()
	{
		if(mMultiSelect)
		{
			// display # selected
		  	char strbuf[256];
			int x = getSelectedObjects().size();
		  	snprintf(strbuf, 256, _N("%i SELECTED", "%i SELECTED", x).c_str(), x);
			mText.setText(strbuf);
			mText.setSize(0, mText.getSize().y());
			setSize(mText.getSize().x() + mRightArrow.getSize().x() + 24, mText.getSize().y());
			if(mParent) // hack since theres no "on child size changed" callback atm...
				mParent->onSizeChanged();
		}else{
			// display currently selected + l/r cursors
			for (const auto& entry : mEntries)
			{
				if(entry.selected)
				{
					mText.setText(Strings::ToUpperUTF8(entry.name));
					mText.setSize(0, mText.getSize().y());
					setSize(mText.getSize().x() + mLeftArrow.getSize().x() + mRightArrow.getSize().x() + 24, mText.getSize().y());
					if(mParent) // hack since theres no "on child size changed" callback atm...
						mParent->onSizeChanged();
					break;
				}
			}
		}

		if (mInterface != nullptr)
      mInterface->OptionListComponentChanged(mId, getSelectedId(), mEntries[getSelectedId()].object);

    if (mMultiInterface != nullptr)
    {
      mMultiInterface->OptionListMultiComponentChanged(mId, getSelectedObjects());
    }

    if (mSelectedChangedCallback) {
			mSelectedChangedCallback(mEntries[getSelectedId()].object);
		}

		if (mChangedCallback) {
			mChangedCallback();
		}
	}

	bool getHelpPrompts(Help& help) override
	{
		if(!mMultiSelect)
		  help.Set(HelpType::LeftRight, _("CHANGE"));
		
		help.Set(Help::Valid(), _("SELECT"));
		return true;
	}

	void setColor(unsigned int color) override
	{
		mText.setColor(color);
		mRightArrow.setColorShift(color);
		mLeftArrow.setColorShift(color);
	}

	inline void setOriginColor(unsigned int color){mOriginColor = color;};
	inline unsigned int getOriginColor() override{return mOriginColor;};

    std::string getValue() const override {
        assert(!mMultiSelect);
        for (auto& entry : mEntries)
        {
            if(entry.selected) {
                return entry.value;
            }
        }
        return "";
    }

	std::string mName;
	T firstSelected;
	TextComponent mText;
	ImageComponent mLeftArrow;
	ImageComponent mRightArrow;
	unsigned int mOriginColor;
	int mId;

	std::vector<OptionListData> mEntries;
	std::function<void(const T&)> mSelectedChangedCallback;
	std::function<void()> mChangedCallback;
	IOptionListComponent<T>* mInterface;
  IOptionListMultiComponent<T>* mMultiInterface;

  bool mMultiSelect;
};
