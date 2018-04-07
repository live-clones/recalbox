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
	e.object = NULL;
	e.data = row;

	this->add(e);

	for(auto it = mEntries.back().data.elements.begin(); it != mEntries.back().data.elements.end(); it++)
		addChild(it->component.get());

	if (updateGeometry) {
		updateElementSize(mEntries.back().data);
		updateElementPosition(mEntries.back().data);
	}

	if(setCursorHere)
	{
		mCursor = mEntries.size() - 1;
		onCursorChanged(CURSOR_STOPPED);
	}
}

void ComponentList::onSizeChanged()
{
	for(auto it = mEntries.begin(); it != mEntries.end(); it++)
	{
		updateElementSize(it->data);
		updateElementPosition(it->data);
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
		if(row.elements.size())
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
		for(auto it = mEntries.at(mCursor).data.elements.begin(); it != mEntries.at(mCursor).data.elements.end(); it++)
			it->component->update(deltaTime);
	}
}

void ComponentList::onCursorChanged(const CursorState& state)
{
	// update the selector bar position
	// in the future this might be animated
	mSelectorBarOffset = 0;
	for(int i = 0; i < mCursor; i++)
	{
		mSelectorBarOffset += getRowHeight(mEntries.at(i).data);
	}

	updateCameraOffset();

	// this is terribly inefficient but we don't know what we came from so...
	if(size())
	{
		for(auto it = mEntries.begin(); it != mEntries.end(); it++)
			it->data.elements.back().component->onFocusLost();
		
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

void ComponentList::render(const Eigen::Affine3f& parentTrans)
{
	if(!size())
		return;

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	unsigned int selectorColor = menuTheme->menuText.selectorColor;
	unsigned int selectedColor = menuTheme->menuText.selectedColor;
	unsigned int bgColor = menuTheme->menuBackground.color;
	unsigned int separatorColor = menuTheme->menuText.separatorColor;
	unsigned int textColor = menuTheme->menuText.color;

	Eigen::Affine3f trans = roundMatrix(parentTrans * getTransform());

	// clip everything to be inside our bounds
	Eigen::Vector3f dim(mSize.x(), mSize.y(), 0);
	dim = trans * dim - trans.translation();
	Renderer::pushClipRect(Eigen::Vector2i((int)trans.translation().x(), (int)trans.translation().y()), 
		Eigen::Vector2i((int)round(dim.x()), (int)round(dim.y() + 1)));

	// scroll the camera
	trans.translate(Eigen::Vector3f(0, -round(mCameraOffset), 0));

	// draw our entries
	std::vector<GuiComponent*> drawAfterCursor;
	bool drawAll;
	for(unsigned int i = 0; i < mEntries.size(); i++)
	{
		auto& entry = mEntries.at(i);
		drawAll = !mFocused || i != mCursor;
		for(auto it = entry.data.elements.begin(); it != entry.data.elements.end(); it++)
		{
			if(drawAll || it->invert_when_selected)
			{
				it->component->setColor(textColor);
				it->component->render(trans);
			}else{
				drawAfterCursor.push_back(it->component.get());
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
		for(auto it = entry.data.elements.begin(); it != entry.data.elements.end(); it++) {

			it->component->setColor(selectedColor);
			drawAfterCursor.push_back(it->component.get());
		}

		for(auto it = drawAfterCursor.begin(); it != drawAfterCursor.end(); it++)
			(*it)->render(trans);
		
		// reset matrix if one of these components changed it
		if(drawAfterCursor.size())
			Renderer::setMatrix(trans);
	}

	// draw separators
	float y = 0;
	
	for(unsigned int i = 0; i < mEntries.size(); i++)
	{
		Renderer::drawRect(0.0f, y, mSize.x(), 1.0f, separatorColor);
		y += getRowHeight(mEntries.at(i).data);
	}
	Renderer::drawRect(0.0f, y, mSize.x(), 1.0f, separatorColor);

	Renderer::popClipRect();
}

float ComponentList::getRowHeight(const ComponentListRow& row) const
{
	// returns the highest component height found in the row
	float height = 0;
	for(unsigned int i = 0; i < row.elements.size(); i++)
	{
		if(row.elements.at(i).component->getSize().y() > height)
			height = row.elements.at(i).component->getSize().y();
	}

	return height;
}

float ComponentList::getTotalRowHeight() const
{
	float height = 0;
	for(auto it = mEntries.begin(); it != mEntries.end(); it++)
	{
		height += getRowHeight(it->data);
	}

	return height;
}

void ComponentList::updateElementPosition(const ComponentListRow& row)
{
	float yOffset = 0;
	for(auto it = mEntries.begin(); it != mEntries.end() && &it->data != &row; it++)
	{
		yOffset += getRowHeight(it->data);
	}

	// assumes updateElementSize has already been called
	float rowHeight = getRowHeight(row);

	float x = TOTAL_HORIZONTAL_PADDING_PX / 2;
	for(unsigned int i = 0; i < row.elements.size(); i++)
	{
		const auto comp = row.elements.at(i).component;

		// center vertically
		comp->setPosition(x, (rowHeight - comp->getSize().y()) / 2 + yOffset);
		x += comp->getSize().x();
	}
}

void ComponentList::updateElementSize(const ComponentListRow& row)
{
	float width = mSize.x() - TOTAL_HORIZONTAL_PADDING_PX;
	std::vector< std::shared_ptr<GuiComponent> > resizeVec;

	for(auto it = row.elements.begin(); it != row.elements.end(); it++)
	{
		if(it->resize_width)
			resizeVec.push_back(it->component);
		else
			width -= it->component->getSize().x();
	}

	// redistribute the "unused" width equally among the components with resize_width set to true
	width = width / resizeVec.size();
	for(auto it = resizeVec.begin(); it != resizeVec.end(); it++)
	{
		(*it)->setSize(width, (*it)->getSize().y());
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
		for(auto it = prompts.begin(); it != prompts.end(); it++)
		{
		  if(strcmp(it->first.c_str(), "up/down") == 0 || strcmp(it->first.c_str(), "up/down/left/right") == 0)
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
