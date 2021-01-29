#include "components/ComponentGrid.h"
#include "Renderer.h"
#include <utils/locale/LocaleHelper.h>
#include <input/InputCompactEvent.h>
#include <help/Help.h>

ComponentGrid::ComponentGrid(WindowManager&window, const Vector2i& gridDimensions)
  : Component(window),
    mGridSize({ 0, 0}),
    mCursor(0, 0),
    mHighlightRowFrom(0),
    mHighlightRowTo(0),
    mHighlightColumnFrom(0),
    mHighlightColumnTo(0),
    mHighlightRows(false),
    mHighlightColumns(false),
    mUnhandledInputCallback(nullptr)
{
  SetGridDimensions(gridDimensions);
}

void ComponentGrid::SetGridDimensions(const Vector2i& gridDimensions)
{
  if (gridDimensions.x() == 0 || gridDimensions.y() == 0)
    return;

  mGridSize = gridDimensions;

  mCells.clear();
  mCells.reserve(gridDimensions.x() * gridDimensions.y());

  mColWidths.resize(gridDimensions.x(), 0.0f);
  mRowHeights.resize(gridDimensions.y(), 0.0f);
}

float ComponentGrid::getColWidth(int col)
{
    if(mColWidths[col] != 0)
        return mColWidths[col] * mSize.x();

    // calculate automatic width
    float freeWidthPerc = 1.0f;
    int between = 0;
    for (int x = mGridSize.x(); --x >= 0; )
    {
        freeWidthPerc -= mColWidths[x]; // if it's 0 it won't do anything
        if(mColWidths[x] == 0)
            between++;
    }
    
    return (freeWidthPerc * mSize.x()) / (float)between;
}

float ComponentGrid::getRowHeight(int row)
{
    if(mRowHeights[row] != 0)
        return mRowHeights[row] * mSize.y();

    // calculate automatic height
    float freeHeightPerc = 1.0f;
    int between = 0;
    for (int y = mGridSize.y(); --y >= 0; )
    {
      freeHeightPerc -= mRowHeights[y]; // if it's 0 it won't do anything
      if(mRowHeights[y] == 0) between++;
    }
    
    return (freeHeightPerc * mSize.y()) / (float)between;
}

float ComponentGrid::getColWidth(int col1, int col2)
{
  float result = 0;
  for(++col2; --col2 >= col1; )
    result += getColWidth(col2);
  return result;
}

float ComponentGrid::getRowHeight(int row1, int row2)
{
  float result = 0;
  for(++row2; --row2 >= row1; )
    result += getRowHeight(row2);
  return result;
}

void ComponentGrid::setColWidthPerc(int col, float width, bool update)
{
    assert(width >= 0 && width <= 1);
    assert(col >= 0 && col < mGridSize.x());
    mColWidths[col] = width;

    if(update)
        onSizeChanged();
}

void ComponentGrid::setRowHeightPerc(int row, float height, bool update)
{
    assert(height >= 0 && height <= 1);
    assert(row >= 0 && row < mGridSize.y());
    mRowHeights[row] = height;

    if(update)
        onSizeChanged();
}

void ComponentGrid::setEntry(const std::shared_ptr<Component>& comp, const Vector2i& pos, bool canFocus, bool resize, const Vector2i& size,
                             Borders border, UpdateType updateType)
{
    assert(pos.x() >= 0 && pos.x() < mGridSize.x() && pos.y() >= 0 && pos.y() < mGridSize.y());
    assert(comp != nullptr);
    assert(comp->getParent() == nullptr);

    GridEntry entry(pos, size, comp, canFocus, resize, updateType, border);
    mCells.push_back(entry);

    addChild(comp.get());

    if(!cursorValid() && canFocus)
    {
        auto origCursor = mCursor;
        mCursor = pos;
        onCursorMoved(origCursor, mCursor);
    }

    updateCellComponent(mCells.back());
    updateSeparators();
}

bool ComponentGrid::removeEntry(const std::shared_ptr<Component>& comp)
{
    for (auto it = mCells.begin(); it != mCells.end(); it++)
    {
        if(it->component == comp)
        {
            removeChild(comp.get());
            mCells.erase(it);
            return true;
        }
    }

    return false;
}

void ComponentGrid::updateCellComponent(const GridEntry& cell)
{
    // size
    Vector2f size(0, 0);
    for (int x = cell.pos.x(); x < cell.pos.x() + cell.dim.x(); x++)
        size[0] += getColWidth(x);
    for (int y = cell.pos.y(); y < cell.pos.y() + cell.dim.y(); y++)
        size[1] += getRowHeight(y);

    if(cell.resize)
        cell.component->setSize(size);

    // position
    // find top left corner
    Vector3f pos(0, 0, 0);
    for (int x = 0; x < cell.pos.x(); x++)
        pos[0] += getColWidth(x);
    for (int y = 0; y < cell.pos.y(); y++)
        pos[1] += getRowHeight(y);

    // center component
    pos[0] = pos.x() + (size.x() - cell.component->getSize().x()) / 2;
    pos[1] = pos.y() + (size.y() - cell.component->getSize().y()) / 2;
    
    cell.component->setPosition(pos);
}

void ComponentGrid::updateSeparators()
{
    mLines.clear();

    bool drawAll = false; //Settings::Instance().DebugGrid();

    Vector2f pos(0, 0);
    Vector2f size(0, 0);
    for (auto& mCell : mCells)
    {
        if((mCell.border == 0) && !drawAll)
            continue;

        // find component position + size
        for (int x = 0; x < mCell.pos.x(); x++)
            pos[0] += getColWidth(x);
        for (int y = 0; y < mCell.pos.y(); y++)
            pos[1] += getRowHeight(y);
        for (int x = mCell.pos.x(); x < mCell.pos.x() + mCell.dim.x(); x++)
            size[0] += getColWidth(x);
        for (int y = mCell.pos.y(); y < mCell.pos.y() + mCell.dim.y(); y++)
            size[1] += getRowHeight(y);

        if (hasFlag(mCell.border, Borders::Top) || drawAll)
        {
            mLines.push_back(Vert(pos.x(), pos.y()));
            mLines.push_back(Vert(pos.x() + size.x(), pos.y()));
        }
        if (hasFlag(mCell.border, Borders::Bottom) || drawAll)
        {
            mLines.push_back(Vert(pos.x(), pos.y() + size.y()));
            mLines.push_back(Vert(pos.x() + size.x(), mLines.back().y));
        }
        if (hasFlag(mCell.border, Borders::Left) || drawAll)
        {
            mLines.push_back(Vert(pos.x(), pos.y()));
            mLines.push_back(Vert(pos.x(), pos.y() + size.y()));
        }
        if (hasFlag(mCell.border, Borders::Right) || drawAll)
        {
            mLines.push_back(Vert(pos.x() + size.x(), pos.y()));
            mLines.push_back(Vert(mLines.back().x, pos.y() + size.y()));
        }
    }

    mLineColors.reserve(mLines.size());
    Renderer::BuildGLColorArray((GLubyte*)mLineColors.data(), 0xC6C7C6FF, mLines.size());
}

void ComponentGrid::onSizeChanged()
{
    for (auto& mCell : mCells)
        updateCellComponent(mCell);

    updateSeparators();
}

ComponentGrid::GridEntry* ComponentGrid::getCellAt(int x, int y)
{
    assert(x >= 0 && x < mGridSize.x() && y >= 0 && y < mGridSize.y());
    
    for (auto& mCell : mCells)
    {
        int xmin = mCell.pos.x();
        int xmax = xmin + mCell.dim.x();
        int ymin = mCell.pos.y();
        int ymax = ymin + mCell.dim.y();

        if(x >= xmin && y >= ymin && x < xmax && y < ymax)
            return &mCell;
    }

    return nullptr;
}

bool ComponentGrid::ProcessInput(const InputCompactEvent& event)
{
  GridEntry* cursorEntry = getCellAt(mCursor);

  if ((cursorEntry != nullptr) && cursorEntry->component->ProcessInput(event))
    return true;

  if (event.AnyDownPressed())       return moveCursor(Vector2i(0, 1));
  else if (event.AnyUpPressed())    return moveCursor(Vector2i(0, -1));
  else if (event.AnyLeftPressed())  return moveCursor(Vector2i(-1, 0));
  else if (event.AnyRightPressed()) return moveCursor(Vector2i(1, 0));
  else if (mUnhandledInputCallback) return mUnhandledInputCallback(event);

  return false;
}

void ComponentGrid::resetCursor()
{
    if(mCells.empty())
        return;

    for (auto& mCell : mCells)
    {
        if(mCell.canFocus)
        {
            Vector2i origCursor = mCursor;
            mCursor = mCell.pos;
            onCursorMoved(origCursor, mCursor);
            break;
        }
    }
}

bool ComponentGrid::moveCursor(Vector2i dir)
{
    assert(dir.x() || dir.y());

    const Vector2i origCursor = mCursor;

    GridEntry* currentCursorEntry = getCellAt(mCursor);

    Vector2i searchAxis(dir.x() == 0 ? 1 : 0, dir.y() == 0 ? 1 : 0);
    
    while(mCursor.x() >= 0 && mCursor.y() >= 0 && mCursor.x() < mGridSize.x() && mCursor.y() < mGridSize.y())
    {
        mCursor = mCursor + dir;

        Vector2i curDirPos = mCursor;

        GridEntry* cursorEntry = nullptr;
        //spread out on search axis+
        while(mCursor.x() < mGridSize.x() && mCursor.y() < mGridSize.y()
            && mCursor.x() >= 0 && mCursor.y() >= 0)
        {
            cursorEntry = getCellAt(mCursor);
            if((cursorEntry != nullptr) && cursorEntry->canFocus && cursorEntry != currentCursorEntry)
            {
                onCursorMoved(origCursor, mCursor);
                return true;
            }

            mCursor += searchAxis;
        }

        //now again on search axis-
        mCursor = curDirPos;
        while(mCursor.x() >= 0 && mCursor.y() >= 0
            && mCursor.x() < mGridSize.x() && mCursor.y() < mGridSize.y())
        {
            cursorEntry = getCellAt(mCursor);
            if((cursorEntry != nullptr) && cursorEntry->canFocus && cursorEntry != currentCursorEntry)
            {
                onCursorMoved(origCursor, mCursor);
                return true;
            }

            mCursor -= searchAxis;
        }

        mCursor = curDirPos;
    }

    //failed to find another focusable element in this direction
    mCursor = origCursor;
    return false;
}

void ComponentGrid::onFocusLost()
{
    GridEntry* cursorEntry = getCellAt(mCursor);
    if(cursorEntry != nullptr)
        cursorEntry->component->onFocusLost();
}

void ComponentGrid::onFocusGained()
{
    GridEntry* cursorEntry = getCellAt(mCursor);
    if(cursorEntry != nullptr)
        cursorEntry->component->onFocusGained();
}

bool ComponentGrid::cursorValid()
{
    GridEntry* e = getCellAt(mCursor);
    return (e != nullptr && e->canFocus);
}

void ComponentGrid::Update(int deltaTime)
{
    // update ALL THE THINGS
    GridEntry* cursorEntry = getCellAt(mCursor);
    for (auto& mCell : mCells)
    {
        if(mCell.updateType == UpdateType::Always || (mCell.updateType == UpdateType::WhenSelected && cursorEntry == &mCell))
          mCell.component->Update(deltaTime);
    }
}

void ComponentGrid::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();
  Renderer::SetMatrix(trans);

  // Render rows
  if (mHighlightRows)
  {
    float x = mPosition.x() + getColWidth(0, mHighlightColumnFrom - 1);
    float w = getColWidth(mHighlightColumnFrom, mHighlightColumnTo);
    float y = mPosition.y() + getRowHeight(0, mHighlightRowFrom - 1);
    for (int i = mHighlightRowFrom - 1; ++i <= mHighlightRowTo; )
    {
      float h = getRowHeight(i);
      Renderer::DrawRectangle(x, y, w, h, (i & 1) != 0 ? 0xFFFFFF18 : 0x00000018);
      y += h;
    }
  }

  if (mHighlightColumns)
  {
    float y = mPosition.y() + getRowHeight(0, mHighlightRowFrom - 1);
    float h = getRowHeight(mHighlightRowFrom, mHighlightRowTo);
    float x = mPosition.x() + getColWidth(0, mHighlightColumnFrom - 1);
    for (int i = mHighlightColumnFrom - 1; ++i < mHighlightColumnTo; )
    {
      float w = getColWidth(i);
      Renderer::DrawRectangle(x, y, w, h, (i & 1) != 0 ? 0xFFFFFF18 : 0x00000018);
      x += w;
    }
  }





  // Render rows
/*  if (mHighlightColumns)
  {
    float x = mPosition.x() + mSize.x();
    for (int i = mGridSize.x(); --i >= 0;)
    {
      float width = getColWidth(i);
      x -= width;
      Renderer::drawRect(x, mPosition.y(), width, mSize.y(), (i & 1) ? 0xFFFFFF18 : 0x00000018);
    }
  }*/
/*
  if (mHighlightRows)
  {
    float y = getRowHeight(0, mHighlightRowTo);
    for (int i = mHighlightRowTo; --i >= mHighlightRowFrom; )
    {
      float height = getRowHeight(i);
      y -= height;
      Renderer::drawRect(0.0f, y, mSize.x(), height, (i & 1) ? 0xFFFFFF18 : 0x00000018);
    }
  }

  // Render rows
  if (mHighlightColumns)
  {
    float x = getColWidth(0, mHighlightColumnTo);
    for (int i = mHighlightColumnTo; --i >= mHighlightColumnFrom;)
    {
      float width = getColWidth(i);
      x -= width;
      Renderer::drawRect(x, 0.0f, width, mSize.y(), (i & 1) ? 0xFFFFFF18 : 0x00000018);
    }
  }
*/
  renderChildren(trans);
    
    // draw cell separators
    if(!mLines.empty())
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, &mLines[0].x);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, mLineColors.data());

        glDrawArrays(GL_LINES, 0, mLines.size());

        glDisable(GL_BLEND);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
}

void ComponentGrid::textInput(const char* text)
{
    GridEntry* selectedEntry = getCellAt(mCursor);
    if(selectedEntry != nullptr && selectedEntry->canFocus)
        selectedEntry->component->textInput(text);
}

void ComponentGrid::onCursorMoved(Vector2i from, Vector2i to)
{
    GridEntry* cell = getCellAt(from);
    if(cell != nullptr)
        cell->component->onFocusLost();

    cell = getCellAt(to);
    if(cell != nullptr)
        cell->component->onFocusGained();

    updateHelpPrompts();
}

void ComponentGrid::setCursorTo(const std::shared_ptr<Component>& comp)
{
    for (auto& mCell : mCells)
    {
        if(mCell.component == comp)
        {
            Vector2i oldCursor = mCursor;
            mCursor = mCell.pos;
            onCursorMoved(oldCursor, mCursor);
            return;
        }
    }

    // component not found!!
    assert(false);
}

bool ComponentGrid::getHelpPrompts(Help& help)
{
  GridEntry* e = getCellAt(mCursor);
  if(e != nullptr) e->component->getHelpPrompts(help);

  bool canScrollVert = mGridSize.y() > 1;
  bool canScrollHoriz = mGridSize.x() > 1;
  if (!help.IsSet(HelpType::AllDirections)) canScrollHoriz = canScrollVert = false;
  if (!help.IsSet(HelpType::LeftRight)) canScrollHoriz = false;
  if (!help.IsSet(HelpType::UpDown)) canScrollVert = false;

  if(canScrollHoriz && canScrollVert) help.Set(HelpType::AllDirections, _("CHOOSE"));
  else if(canScrollHoriz) help.Set(HelpType::LeftRight, _("CHOOSE"));
  else if(canScrollVert) help.Set(HelpType::UpDown, _("CHOOSE"));

  return true;
}

