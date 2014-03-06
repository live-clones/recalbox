#pragma once

#include "../GuiComponent.h"
#include "../resources/Font.h"
#include "../Renderer.h"
#include "../Window.h"
#include "TextComponent.h"
#include "ImageComponent.h"
#include "MenuComponent.h"
#include <sstream>
#include "../Log.h"

//Used to display a list of options.
//Can select one or multiple options.

// if !multiSelect
// * <- curEntry ->

// always
// * press a -> open full list

template<typename T>
class OptionListComponent : public GuiComponent
{
private:
	struct OptionListData
	{
		std::string name;
		T object;
		bool selected;
	};

	class OptionListPopup : public GuiComponent
	{
	private:
		MenuComponent mMenu;
		OptionListComponent<T>* mParent;

	public:
		OptionListPopup(Window* window, OptionListComponent<T>* parent) : GuiComponent(window),
			mMenu(window, ""), mParent(parent)
		{
			auto font = Font::get(FONT_SIZE_MEDIUM);
			ComponentListRow row;

			for(auto it = mParent->mEntries.begin(); it != mParent->mEntries.end(); it++)
			{
				row.elements.clear();
				row.addElement(std::make_shared<TextComponent>(mWindow, it->name, font, 0x777777FF), true);

				OptionListData& e = *it;

				if(mParent->mMultiSelect)
				{
					// add checkbox
					auto checkbox = std::make_shared<ImageComponent>(mWindow);
					checkbox->setImage(it->selected ? ":/checkbox_checked.png" : ":/checkbox_unchecked.png");

					if(checkbox->getTextureSize().y() > (int)FONT_SIZE_MEDIUM) // downscale if necessary to match text
						checkbox->setResize(0, (float)FONT_SIZE_MEDIUM);

					row.addElement(checkbox, false);

					// input handler
					// update checkbox state & selected value
					row.makeAcceptInputHandler([this, &e, checkbox]
					{
						e.selected = !e.selected;
						checkbox->setImage(e.selected ? ":/checkbox_checked.png" : ":/checkbox_unchecked.png");
						mParent->onSelectedChanged();
					});
				}else{
					// input handler for non-multiselect
					// update selected value and close
					row.makeAcceptInputHandler([this, &e]
					{
						mParent->mEntries.at(mParent->getSelectedId()).selected = false;
						e.selected = true;
						mParent->onSelectedChanged();
						delete this;
					});
				}

				// also set cursor to this row if we're not multi-select and this row is selected
				mMenu.addRow(row, (!mParent->mMultiSelect && it->selected));
			}

			mMenu.setPosition((Renderer::getScreenWidth() - mMenu.getSize().x()) / 2, (Renderer::getScreenHeight() - mMenu.getSize().y()) / 2);
			addChild(&mMenu);
		}

		bool input(InputConfig* config, Input input) override
		{
			if(config->isMappedTo("b", input) && input.value != 0)
			{
				delete this;
				return true;
			}

			return GuiComponent::input(config, input);
		}
	};

public:
	OptionListComponent(Window* window, bool multiSelect = false) : GuiComponent(window), mMultiSelect(multiSelect),
		 mText(window), mLeftArrow(window), mRightArrow(window)
	{
		auto font = Font::get(FONT_SIZE_MEDIUM);
		mText.setFont(font);
		mText.setColor(0x777777FF);
		mText.setCentered(true);
		addChild(&mText);

		if(mMultiSelect)
		{
			mRightArrow.setImage(":/sq_bracket.png");
			addChild(&mRightArrow);
		}else{
			mLeftArrow.setImage(":/arrow.png");
			mLeftArrow.setFlipX(true);
			addChild(&mLeftArrow);

			mRightArrow.setImage(":/arrow.png");
			addChild(&mRightArrow);
		}

		// handles positioning/resizing of text and arrows
		setSize(Renderer::getScreenWidth() * 0.2f, (float)font->getHeight());
	}

	void onSizeChanged() override
	{
		// size
		if(mLeftArrow.getTextureSize().y() > mSize.y())
			mLeftArrow.setResize(0, mSize.y());
		else
			mLeftArrow.setResize(0, 0);

		if(mRightArrow.getTextureSize().y() > mSize.y())
			mRightArrow.setResize(0, mSize.y());
		else
			mRightArrow.setResize(0, 0);

		if(mSize.x() < (mLeftArrow.getSize().x() + mRightArrow.getSize().x()))
			LOG(LogWarning) << "OptionListComponent too narrow!";

		mText.setSize(mSize.x() - mLeftArrow.getSize().x() - mRightArrow.getSize().x(), (float)mText.getFont()->getHeight());

		// position
		mLeftArrow.setPosition(0, (mSize.y() - mLeftArrow.getSize().y()) / 2);
		mText.setPosition(mLeftArrow.getPosition().x() + mLeftArrow.getSize().x(), (mSize.y() - mText.getSize().y()) / 2);
		mRightArrow.setPosition(mText.getPosition().x() + mText.getSize().x(), (mSize.y() - mRightArrow.getSize().y()) / 2);
	}

	bool input(InputConfig* config, Input input) override
	{
		if(input.value != 0)
		{
			if(config->isMappedTo("a", input))
			{
				open();
				return true;
			}
			if(!mMultiSelect)
			{
				if(config->isMappedTo("left", input))
				{
					// move selection to previous
					unsigned int i = getSelectedId();
					int next = (int)i - 1;
					if(next < 0)
						next += mEntries.size();

					mEntries.at(i).selected = false;
					mEntries.at(next).selected = true;
					onSelectedChanged();
					return true;

				}else if(config->isMappedTo("right", input))
				{
					// move selection to next
					unsigned int i = getSelectedId();
					int next = (i + 1) % mEntries.size();
					mEntries.at(i).selected = false;
					mEntries.at(next).selected = true;
					onSelectedChanged();
					return true;

				}
			}
		}
		return GuiComponent::input(config, input);
	}

	std::vector<T> getSelectedObjects()
	{
		std::vector<T> ret;
		for(auto it = mEntries.begin(); it != mEntries.end(); it++)
		{
			if(it->selected)
				ret.push_back(it->object);
		}

		return ret;
	}

	T getSelected()
	{
		assert(mMultiSelect == false);
		auto selected = getSelectedObjects();
		assert(selected.size() == 1);
		return selected.at(0);
	}

	void add(const std::string& name, const T& obj, bool selected)
	{
		OptionListData e;
		e.name = name;
		e.object = obj;
		e.selected = selected;

		mEntries.push_back(e);
		onSelectedChanged();
	}

private:
	unsigned int getSelectedId()
	{
		assert(mMultiSelect == false);
		for(unsigned int i = 0; i < mEntries.size(); i++)
		{
			if(mEntries.at(i).selected)
				return i;
		}

		LOG(LogWarning) << "OptionListComponent::getSelectedId() - no selected element found, defaulting to 0";
		return 0;
	}

	void open()
	{
		mWindow->pushGui(new OptionListPopup(mWindow, this));
	}

	void onSelectedChanged()
	{
		if(mMultiSelect)
		{
			// display # selected
			std::stringstream ss;
			ss << getSelectedObjects().size() << " selected";
			mText.setText(ss.str());
		}else{
			// display currently selected + l/r cursors
			for(auto it = mEntries.begin(); it != mEntries.end(); it++)
			{
				if(it->selected)
				{
					mText.setText(it->name);
					break;
				}
			}
		}
	}

	bool mMultiSelect;

	TextComponent mText;
	ImageComponent mLeftArrow;
	ImageComponent mRightArrow;

	std::vector<OptionListData> mEntries;
};

