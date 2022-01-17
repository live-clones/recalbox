#include <components/ComponentList.h>
#include <themes/MenuThemeData.h>
#include <help/Help.h>
#include <utils/locale/LocaleHelper.h>

#define TOTAL_HORIZONTAL_PADDING_PX 20

ComponentList::ComponentList(WindowManager&window)
   : IList<ComponentListRow, void*>(window, LIST_SCROLL_STYLE_SLOW, LoopType::NeverLoop)
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
		onCursorChanged(CursorState::Stopped);
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

bool ComponentList::ProcessInput(const InputCompactEvent& event)
{
	if(size() == 0)
		return false;

	if (mEntries[mCursor].data.ProcessInput(event))
	  return true;

	// input handler didn't consume the input - try to scroll
	if (event.AnyUpPressed())	      return listInput(-1);
	else if (event.AnyDownPressed()) return listInput(1);
	else if (event.L1Pressed())   return listInput(-7);
	else if (event.R1Pressed())   return listInput(7);
	else if (event.AnyUpReleased() || event.AnyDownReleased() || event.L1Released() || event.R1Released())
	  listInput(0);

	return false;
}

void ComponentList::Update(int deltaTime)
{
	listUpdate(deltaTime);

	if(size() != 0)
	{
		// update our currently selected row
		for (auto& element : mEntries[mCursor].data.elements)
      element.component->Update(deltaTime);
	}
}

void ComponentList::onCursorChanged(const CursorState& state)
{
	// update the selector bar position
	// in the future this might be animated
	mSelectorBarOffset = 0;
	for (int i = 0; i < mCursor; i++)
	{
		mSelectorBarOffset += getRowHeight(mEntries[i].data);
	}

	updateCameraOffset();

	// this is terribly inefficient but we don't know what we came from so...
	if(size() != 0)
	{
		for (auto& mEntrie : mEntries)
			mEntrie.data.elements.back().component->onFocusLost();
		
		mEntries[mCursor].data.elements.back().component->onFocusGained();
	}

	if(mCursorChangedCallback)
		mCursorChangedCallback(state);

	updateHelpPrompts();
}

void ComponentList::updateCameraOffset()
{
	// move the camera to scroll
	const float totalHeight = getTotalRowHeight();
	if(totalHeight > mSize.y() && !mEntries.empty())
	{
		float target = mSelectorBarOffset + getRowHeight(mEntries[mCursor].data)/2 - (mSize.y() / 2);

		// clamp it
		mCameraOffset = 0;
		unsigned int i = 0;
		while(mCameraOffset < target && i < mEntries.size())
		{
			mCameraOffset += getRowHeight(mEntries[i].data);
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

void ComponentList::Render(const Transform4x4f& parentTrans)
{
	if(size() == 0)
		return;

	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	unsigned int selectorColor = menuTheme->menuText.selectorColor;
	unsigned int selectedColor = menuTheme->menuText.selectedColor;
	unsigned int bgColor = menuTheme->menuBackground.color;
	unsigned int separatorColor = menuTheme->menuText.separatorColor;

	Transform4x4f trans = (parentTrans * getTransform()).round();

	// clip everything to be inside our bounds
	Vector3f dim(mSize.x(), mSize.y(), 0);
	dim = trans * dim - trans.translation();
	Renderer::Instance().PushClippingRect(Vector2i((int)trans.translation().x(), (int)trans.translation().y()),
		Vector2i(Math::roundi(dim.x()), Math::roundi(dim.y() + 1)));

	// scroll the camera
	trans.translate(Vector3f(0, -Math::round(mCameraOffset), 0));

	// draw our entries
	std::vector<Component*> drawAfterCursor;
	for (int i = 0; i < (int)mEntries.size(); i++)
	{
		auto& entry = mEntries[i];
		bool drawAll = !mFocused || i != mCursor;
		for (auto& element : entry.data.elements)
		{
			if(drawAll || element.invert_when_selected)
			{
				element.component->setColor(element.component->getOriginColor());
        element.component->Render(trans);
			}else{
				drawAfterCursor.push_back(element.component.get());
			}
		}
	}

	// custom rendering
	Renderer::SetMatrix(trans);

	// draw selector bar	
	if(mFocused)
	{			
		const float selectedRowHeight = getRowHeight(mEntries[mCursor].data);
		//here we draw a bar then redraw the list entry
		if ((selectorColor != bgColor) && ((selectorColor & 0xFF) != 0x00)) {
			Renderer::DrawRectangle(0.0f, mSelectorBarOffset, mSize.x(), selectedRowHeight, bgColor, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			Renderer::DrawRectangle(0.0f, mSelectorBarOffset, mSize.x(), selectedRowHeight, selectorColor, GL_ONE, GL_ONE);
		}
		auto& entry = mEntries[mCursor];
		for (auto& element : entry.data.elements)
		{
			element.component->setColor(selectedColor);
			drawAfterCursor.push_back(element.component.get());
		}

		for (auto& it : drawAfterCursor)
      it->Render(trans);
		
		// reset matrix if one of these components changed it
		if (!drawAfterCursor.empty())
			Renderer::SetMatrix(trans);
	}

	// draw separators
	float y = 0;
	
	for (auto& mEntrie : mEntries)
	{
		Renderer::DrawRectangle(0.0f, y, mSize.x(), 1.0f, separatorColor);
		y += getRowHeight(mEntrie.data);
	}
	Renderer::DrawRectangle(0.0f, y, mSize.x(), 1.0f, separatorColor);

	Renderer::Instance().PopClippingRect();
}

float ComponentList::getRowHeight(const ComponentListRow& row)
{
	// returns the highest component height found in the row
	float height = 0;
	for (const auto& element : row.elements)
	{
		if(element.component->getSize().y() > height)
			height = element.component->getSize().y();
	}

	return height + 2;
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
	std::vector< std::shared_ptr<Component> > resizeVec;

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
	if(size() == 0)
		return;

	mEntries[mCursor].data.elements.back().component->textInput(text);
}

bool ComponentList::getHelpPrompts(Help& help)
{
	if (size() == 0) return false;
	mEntries[mCursor].data.elements.back().component->getHelpPrompts(help);

	if (size() > 1)
		if (help.IsSet(HelpType::UpDown) || help.IsSet(HelpType::AllDirections)) help.Set(HelpType::UpDown, _("CHOOSE"));

	if ((mEntries[mCursor].data.HasHelpHandler()) && RecalboxConf::Instance().GetPopupHelp() != 0)
		help.Set(HelpType::Y, _("HELP"));

	return true;
}

bool ComponentList::moveCursor(int amt)
{
	bool ret = listInput(amt); 
	listInput(0); 
	return ret;
}
