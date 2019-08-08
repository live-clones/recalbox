#include <guis/GuiMsgBoxScroll.h>
#include <Settings.h>
#include "components/ComponentList.h"
#include "Util.h"
#include "Log.h"
#include "Locale.h"
#include "MenuThemeData.h"

#define TOTAL_HORIZONTAL_PADDING_PX 20

ComponentList::ComponentList(Window* window) : IList<ComponentListRow, void*>(window, LIST_SCROLL_STYLE_SLOW, LIST_NEVER_LOOP)
{
	mSelectorBarOffset = 0;
	mCameraOffset = 0;
	mFocused = false;
}

void ComponentList::addRow(const ComponentListRow& row, bool setCursorHere, bool updateGeometry)
{
	IList<ComponentListRow, void*>::Entry e;
	e.name = row.name;
	e.object = nullptr;
	e.data = row;

	this->add(e);

	for (auto& element : mEntries.back().data.elements)
		addChild(element.component.get());

	if (updateGeometry) {
		updateElementSize(mEntries.back().data);
		updateElementPosition(mEntries.back().data);
	}

	if(setCursorHere)
	{
		mCursor = (int)mEntries.size() - 1;
		onCursorChanged(CURSOR_STOPPED);
	}
}

void ComponentList::onSizeChanged()
{
	for (auto& mEntrie : mEntries)
	{
		updateElementSize(mEntrie.data);
		updateElementPosition(mEntrie.data);
	}

	updateCameraOffset();
}

void ComponentList::onFocusLost()
{
	mFocused = false;

	if(mFocusLostCallback)
		mFocusLostCallback();
}

void ComponentList::onFocusGained()
{
	mFocused = true;

	if(mFocusGainedCallback)
		mFocusGainedCallback();
}

bool ComponentList::input(InputConfig* config, Input input)
{
	if(size() == 0)
		return false;

	if(mEntries.at(mCursor).data.help_handler){
		if(mEntries.at(mCursor).data.help_handler(config, input))
			return true;
	}
	// give it to the current row's input handler
	if(mEntries.at(mCursor).data.input_handler)
	{
		if(mEntries.at(mCursor).data.input_handler(config, input))
			return true;
	}else{
		// no input handler assigned, do the default, which is to give it to the rightmost element in the row
		auto& row = mEntries.at(mCursor).data;
		if(!row.elements.empty())
		{
			if(row.elements.back().component->input(config, input))
				return true;
		}
	}

	// input handler didn't consume the input - try to scroll
	if(config->isMappedTo("up", input))
	{
		return listInput(input.value != 0 ? -1 : 0);
	}else if(config->isMappedTo("down", input))
	{
		return listInput(input.value != 0 ? 1 : 0);
	}else if(config->isMappedTo("pageup", input))
	{
		return listInput(input.value != 0 ? -7 : 0);
	}else if(config->isMappedTo("pagedown", input)){
		return listInput(input.value != 0 ? 7 : 0);
	}

	return false;
}

void ComponentList::update(int deltaTime)
{
	listUpdate(deltaTime);

	if(size())
	{
		// update our currently selected row
		for (auto& element : mEntries.at(mCursor).data.elements)
			element.component->update(deltaTime);
	}
}

void ComponentList::onCursorChanged(const CursorState& state)
{
	// update the selector bar position
	// in the future this might be animated
	mSelectorBarOffset = 0;
	for (int i = 0; i < mCursor; i++)
	{
		mSelectorBarOffset += getRowHeight(mEntries.at(i).data);
	}

	updateCameraOffset();

	// this is terribly inefficient but we don't know what we came from so...
	if(size())
	{
		for (auto& mEntrie : mEntries)
			mEntrie.data.elements.back().component->onFocusLost();
		
		mEntries.at(mCursor).data.elements.back().component->onFocusGained();
	}

	if(mCursorChangedCallback)
		mCursorChangedCallback(state);

	updateHelpPrompts();
}

void ComponentList::updateCameraOffset()
{
	// move the camera to scroll
	const float totalHeight = getTotalRowHeight();
	if(totalHeight > mSize.y())
	{
		float target = mSelectorBarOffset + getRowHeight(mEntries.at(mCursor).data)/2 - (mSize.y() / 2);

		// clamp it
		mCameraOffset = 0;
		unsigned int i = 0;
		while(mCameraOffset < target && i < mEntries.size())
		{
			mCameraOffset += getRowHeight(mEntries.at(i).data);
			i++;
		}

		if(mCameraOffset < 0)
			mCameraOffset = 0;
		else if(mCameraOffset + mSize.y() > totalHeight)
			mCameraOffset = totalHeight - mSize.y();
	}else{
		mCameraOffset = 0;
	}
}

void ComponentList::render(const Transform4x4f& parentTrans)
{
	if(!size())
		return;

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	unsigned int selectorColor = menuTheme->menuText.selectorColor;
	unsigned int selectedColor = menuTheme->menuText.selectedColor;
	unsigned int bgColor = menuTheme->menuBackground.color;
	unsigned int separatorColor = menuTheme->menuText.separatorColor;

	Transform4x4f trans = roundMatrix(parentTrans * getTransform());

	// clip everything to be inside our bounds
	Vector3f dim(mSize.x(), mSize.y(), 0);
	dim = trans * dim - trans.translation();
	Renderer::pushClipRect(Vector2i((int)trans.translation().x(), (int)trans.translation().y()), 
		Vector2i((int)round(dim.x()), (int)round(dim.y() + 1)));

	// scroll the camera
	trans.translate(Vector3f(0, -round(mCameraOffset), 0));

	// draw our entries
	std::vector<GuiComponent*> drawAfterCursor;
	bool drawAll;
	for (int i = 0; i < (int)mEntries.size(); i++)
	{
		auto& entry = mEntries.at(i);
		drawAll = !mFocused || i != mCursor;
		for (auto& element : entry.data.elements)
		{
			if(drawAll || element.invert_when_selected)
			{
				element.component->setColor(element.component->getOriginColor());
				element.component->render(trans);
			}else{
				drawAfterCursor.push_back(element.component.get());
			}
		}
	}

	// custom rendering
	Renderer::setMatrix(trans);

	// draw selector bar	
	if(mFocused)
	{			
		const float selectedRowHeight = getRowHeight(mEntries.at(mCursor).data);
		//here we draw a bar then redraw the list entry
		if ((selectorColor != bgColor) && ((selectorColor & 0xFF) != 0x00)) {
			Renderer::drawRect(0.0f, mSelectorBarOffset, mSize.x(), selectedRowHeight, bgColor, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			Renderer::drawRect(0.0f, mSelectorBarOffset, mSize.x(), selectedRowHeight, selectorColor, GL_ONE, GL_ONE);
		}
		auto& entry = mEntries.at(mCursor);
		for (auto& element : entry.data.elements)
		{

			element.component->setColor(selectedColor);
			drawAfterCursor.push_back(element.component.get());
		}

		for (auto& it : drawAfterCursor)
			it->render(trans);
		
		// reset matrix if one of these components changed it
		if (!drawAfterCursor.empty())
			Renderer::setMatrix(trans);
	}

	// draw separators
	float y = 0;
	
	for (auto& mEntrie : mEntries)
	{
		Renderer::drawRect(0.0f, y, mSize.x(), 1.0f, separatorColor);
		y += getRowHeight(mEntrie.data);
	}
	Renderer::drawRect(0.0f, y, mSize.x(), 1.0f, separatorColor);

	Renderer::popClipRect();
}

float ComponentList::getRowHeight(const ComponentListRow& row) const
{
	// returns the highest component height found in the row
	float height = 0;
	for (const auto& element : row.elements)
	{
		if(element.component->getSize().y() > height)
			height = element.component->getSize().y();
	}

	return height;
}

float ComponentList::getTotalRowHeight() const
{
	float height = 0;
	for (const auto& mEntrie : mEntries)
	{
		height += getRowHeight(mEntrie.data);
	}

	return height;
}

void ComponentList::updateElementPosition(const ComponentListRow& row)
{
	float yOffset = 0;
	for (auto it = mEntries.begin(); it != mEntries.end() && &it->data != &row; it++)
	{
		yOffset += getRowHeight(it->data);
	}

	// assumes updateElementSize has already been called
	float rowHeight = getRowHeight(row);

	float x = (float)TOTAL_HORIZONTAL_PADDING_PX / 2;
	for (const auto& element : row.elements)
	{
		const auto comp = element.component;

		// center vertically
		comp->setPosition(x, (rowHeight - comp->getSize().y()) / 2 + yOffset);
		x += comp->getSize().x();
	}
}

void ComponentList::updateElementSize(const ComponentListRow& row)
{
	float width = mSize.x() - TOTAL_HORIZONTAL_PADDING_PX;
	std::vector< std::shared_ptr<GuiComponent> > resizeVec;

	for (const auto& element : row.elements)
	{
		if(element.resize_width)
			resizeVec.push_back(element.component);
		else
			width -= element.component->getSize().x();
	}

	// redistribute the "unused" width equally among the components with resize_width set to true
	width = width / resizeVec.size();
	for (auto& it : resizeVec)
	{
		it->setSize(width, it->getSize().y());
	}
}

void ComponentList::textInput(const char* text)
{
	if(!size())
		return;

	mEntries.at(mCursor).data.elements.back().component->textInput(text);
}

std::vector<HelpPrompt> ComponentList::getHelpPrompts()
{
	if(!size())
		return std::vector<HelpPrompt>();

	std::vector<HelpPrompt> prompts = mEntries.at(mCursor).data.elements.back().component->getHelpPrompts();

	if(size() > 1)
	{
		bool addMovePrompt = true;
		for (auto& prompt : prompts)
		{
		  if(strcmp(prompt.first.c_str(), "up/down") == 0 || strcmp(prompt.first.c_str(), "up/down/left/right") == 0)
			{
				addMovePrompt = false;
				break;
			}
		}

		if(addMovePrompt)
		  prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));
	}
	if((mEntries.at(mCursor).data.help_handler != nullptr) && Settings::getInstance()->getInt("HelpPopupTime") != 0){
		prompts.push_back(HelpPrompt("y", _("HELP")));
	}

	return prompts;
}

bool ComponentList::moveCursor(int amt)
{
	bool ret = listInput(amt); 
	listInput(0); 
	return ret;
}
