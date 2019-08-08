#include "components/ComponentGrid.h"
#include "Log.h"
#include "Renderer.h"
#include "Settings.h"
#include "Locale.h"

using namespace GridFlags;

ComponentGrid::ComponentGrid(Window* window, const Vector2i& gridDimensions) : GuiComponent(window), 
    mGridSize(gridDimensions), mCursor(0, 0), mUnhandledInputCallback(nullptr)
{
    assert(gridDimensions.x() > 0 && gridDimensions.y() > 0);

    mCells.reserve(gridDimensions.x() * gridDimensions.y());

    mColWidths = new float[gridDimensions.x()];
    mRowHeights = new float[gridDimensions.y()];
    for (int x = 0; x < gridDimensions.x(); x++)
        mColWidths[x] = 0;
    for (int y = 0; y < gridDimensions.y(); y++)
        mRowHeights[y] = 0;
}

ComponentGrid::~ComponentGrid()
{
    delete[] mRowHeights;
    delete[] mColWidths;
}

float ComponentGrid::getColWidth(int col)
{
    if(mColWidths[col] != 0)
        return mColWidths[col] * mSize.x();

    // calculate automatic width
    float freeWidthPerc = 1;
    int between = 0;
    for (int x = 0; x < mGridSize.x(); x++)
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
    float freeHeightPerc = 1;
    int between = 0;
    for (int y = 0; y < mGridSize.y(); y++)
    {
        freeHeightPerc -= mRowHeights[y]; // if it's 0 it won't do anything
        if(mRowHeights[y] == 0)
            between++;
    }
    
    return (freeHeightPerc * mSize.y()) / (float)between;
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

void ComponentGrid::setEntry(const std::shared_ptr<GuiComponent>& comp, const Vector2i& pos, bool canFocus, bool resize, const Vector2i& size,
    unsigned int border, GridFlags::UpdateType updateType)
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

bool ComponentGrid::removeEntry(const std::shared_ptr<GuiComponent>& comp)
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

    bool drawAll = Settings::getInstance()->getBool("DebugGrid");

    Vector2f pos(0, 0);
    Vector2f size(0, 0);
    for (auto& mCell : mCells)
    {
        if(!mCell.border && !drawAll)
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

        if(mCell.border & BORDER_TOP || drawAll)
        {
            mLines.push_back(Vert(pos.x(), pos.y()));
            mLines.push_back(Vert(pos.x() + size.x(), pos.y()));
        }
        if(mCell.border & BORDER_BOTTOM || drawAll)
        {
            mLines.push_back(Vert(pos.x(), pos.y() + size.y()));
            mLines.push_back(Vert(pos.x() + size.x(), mLines.back().y));
        }
        if(mCell.border & BORDER_LEFT || drawAll)
        {
            mLines.push_back(Vert(pos.x(), pos.y()));
            mLines.push_back(Vert(pos.x(), pos.y() + size.y()));
        }
        if(mCell.border & BORDER_RIGHT || drawAll)
        {
            mLines.push_back(Vert(pos.x() + size.x(), pos.y()));
            mLines.push_back(Vert(mLines.back().x, pos.y() + size.y()));
        }
    }

    mLineColors.reserve(mLines.size());
    Renderer::buildGLColorArray((GLubyte*)mLineColors.data(), 0xC6C7C6FF, mLines.size());
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

bool ComponentGrid::input(InputConfig* config, Input input) {
    GridEntry* cursorEntry = getCellAt(mCursor);

    if (cursorEntry && cursorEntry->component->input(config, input)) {
        return true;
    }

    if (!input.value) {
        return false;
    }

    bool result = false;

    if (config->isMappedTo("down", input)) {
        result = moveCursor(Vector2i(0, 1));
    } else if(config->isMappedTo("up", input)) {
        result = moveCursor(Vector2i(0, -1));
    } else if(config->isMappedTo("left", input)) {
        result = moveCursor(Vector2i(-1, 0));
    } else if(config->isMappedTo("right", input)) {
        result = moveCursor(Vector2i(1, 0));
    }

    if (!result && mUnhandledInputCallback) {
        return mUnhandledInputCallback(config, input);
    }

    return result;
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

    Vector2i searchAxis(dir.x() == 0, dir.y() == 0);
    
    while(mCursor.x() >= 0 && mCursor.y() >= 0 && mCursor.x() < mGridSize.x() && mCursor.y() < mGridSize.y())
    {
        mCursor = mCursor + dir;

        Vector2i curDirPos = mCursor;

        GridEntry* cursorEntry;
        //spread out on search axis+
        while(mCursor.x() < mGridSize.x() && mCursor.y() < mGridSize.y()
            && mCursor.x() >= 0 && mCursor.y() >= 0)
        {
            cursorEntry = getCellAt(mCursor);
            if(cursorEntry && cursorEntry->canFocus && cursorEntry != currentCursorEntry)
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
            if(cursorEntry && cursorEntry->canFocus && cursorEntry != currentCursorEntry)
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
    if(cursorEntry)
        cursorEntry->component->onFocusLost();
}

void ComponentGrid::onFocusGained()
{
    GridEntry* cursorEntry = getCellAt(mCursor);
    if(cursorEntry)
        cursorEntry->component->onFocusGained();
}

bool ComponentGrid::cursorValid()
{
    GridEntry* e = getCellAt(mCursor);
    return (e != nullptr && e->canFocus);
}

void ComponentGrid::update(int deltaTime)
{
    // update ALL THE THINGS
    GridEntry* cursorEntry = getCellAt(mCursor);
    for (auto& mCell : mCells)
    {
        if(mCell.updateType == UPDATE_ALWAYS || (mCell.updateType == UPDATE_WHEN_SELECTED && cursorEntry == &mCell))
            mCell.component->update(deltaTime);
    }
}

void ComponentGrid::render(const Transform4x4f& parentTrans)
{
    Transform4x4f trans = parentTrans * getTransform();

    renderChildren(trans);
    
    // draw cell separators
    if(!mLines.empty())
    {
        Renderer::setMatrix(trans);

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
    if(cell)
        cell->component->onFocusLost();

    cell = getCellAt(to);
    if(cell)
        cell->component->onFocusGained();

    updateHelpPrompts();
}

void ComponentGrid::setCursorTo(const std::shared_ptr<GuiComponent>& comp)
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

std::vector<HelpPrompt> ComponentGrid::getHelpPrompts()
{
    std::vector<HelpPrompt> prompts;
    GridEntry* e = getCellAt(mCursor);
    if(e)
        prompts = e->component->getHelpPrompts();
    
    bool canScrollVert = mGridSize.y() > 1;
    bool canScrollHoriz = mGridSize.x() > 1;
    for (auto& prompt : prompts)
    {
      if(strcmp(prompt.first.c_str(), "up/down/left/right") == 0)
        {
            canScrollHoriz = false;
            canScrollVert = false;
            break;
        }else if(strcmp(prompt.first.c_str(), "up/down") == 0)
        {
            canScrollVert = false;
        }else if(strcmp(prompt.first.c_str(), "left/right") == 0)
        {
            canScrollHoriz = false;
        }
    }

    if(canScrollHoriz && canScrollVert)
      prompts.push_back(HelpPrompt("up/down/left/right", _("CHOOSE")));
    else if(canScrollHoriz)
      prompts.push_back(HelpPrompt("left/right", _("CHOOSE")));
    else if(canScrollVert)
      prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));

    return prompts;
}
