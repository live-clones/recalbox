#pragma once

#include "components/base/Component.h"
#include <utils/math/Vector2i.h>
#include <memory>

enum class UpdateType : unsigned char // Take less memory in grid elements
{
    Always,
    WhenSelected,
    Never,
};

enum class Borders : unsigned char // Take less memory in grid elements
{
    None = 0,
    Top = (1 << 0),
    Bottom = (1 << 1),
    Left = (1 << 2),
    Right = (1 << 3),
};

DEFINE_BITFLAG_ENUM(Borders, unsigned int)

// Used to arrange a bunch of components in a spreadsheet-esque grid.
class ComponentGrid : public Component
{
  public:
    ComponentGrid(WindowManager& window, const Vector2i& gridDimensions);

    void SetGridDimensions(const Vector2i& gridDimensions);

    int EntryCount() const
    { return (int) mCells.size(); }

    void ClearEntries()
    { mCells.clear(); }

    bool removeEntry(const std::shared_ptr<Component>& comp);

    void setEntry(const std::shared_ptr<Component>& comp, const Vector2i& pos, bool canFocus, bool resize = true,
                  const Vector2i& size = Vector2i(1, 1), Borders border = Borders::None,
                  UpdateType updateType = UpdateType::Always);

    void textInput(const char* text) override;

    bool ProcessInput(const InputCompactEvent& event) override;

    void Update(int deltaTime) override;

    void Render(const Transform4x4f& parentTrans) override;

    void onSizeChanged() override;

    void resetCursor();

    bool cursorValid();

    float getColWidth(int col);

    float getRowHeight(int row);

    float getColWidth(int col1, int col2);

    float getRowHeight(int row1, int row2);

    void setColWidthPerc(int col, float width,
                         bool update = true); // if update is false, will not call an onSizeChanged() which triggers a (potentially costly) repositioning + resizing of every element
    void setRowHeightPerc(int row, float height,
                          bool update = true); // if update is false, will not call an onSizeChanged() which triggers a (potentially costly) repositioning + resizing of every element

    bool moveCursor(Vector2i dir);

    void setCursorTo(const std::shared_ptr<Component>& comp);

    inline void setCursor(const Vector2i cursor) { mCursor = cursor; }

    inline Vector2i getCursor() { return mCursor; }

    inline void setUnhandledInputCallback(const std::function<bool(const InputCompactEvent&)>& func)
    { mUnhandledInputCallback = func; }

    inline std::shared_ptr<Component> getSelectedComponent()
    {
      GridEntry* e = getCellAt(mCursor);
      if (e != nullptr)
        return e->component;
      else
        return nullptr;
    }

    void onFocusLost() override;

    void onFocusGained() override;

    bool getHelpPrompts(Help& help) override;

    void SetRowHighlight(bool active, int from = 0, int to = 0)
    {
      mHighlightRows = active;
      mHighlightRowFrom = from;
      mHighlightRowTo = to;
    }

    void SetColumnHighlight(bool active, int from = 0, int to = 0)
    {
      mHighlightColumns = active;
      mHighlightColumnFrom = from;
      mHighlightColumnTo = to;
    }

  private:
    class GridEntry
    {
      public:
        Vector2i pos;
        Vector2i dim;
        std::shared_ptr<Component> component;
        UpdateType updateType;
        Borders border;
        bool canFocus;
        bool resize;

        explicit GridEntry(const Vector2i& p = Vector2i::Zero(), const Vector2i& d = Vector2i::Zero(),
                           std::shared_ptr<Component> cmp = nullptr, bool f = false, bool r = true,
                           UpdateType u = UpdateType::Always, Borders b = Borders::None)
          : pos(p), dim(d), component(std::move(cmp)), updateType(u), border(b), canFocus(f), resize(r)
        {};

        explicit operator bool() const
        {
          return component != nullptr;
        }
    };

    std::vector<float> mRowHeights;
    std::vector<float> mColWidths;

    struct Vert
    {
      explicit Vert(float xi = 0, float yi = 0)
        : x(xi), y(yi)
      {};
      float x;
      float y;
    };

    std::vector<Vert> mLines;
    std::vector<unsigned int> mLineColors;

    std::vector<GridEntry> mCells;

    Vector2i mGridSize;
    Vector2i mCursor;

    int mHighlightRowFrom;
    int mHighlightRowTo;
    int mHighlightColumnFrom;
    int mHighlightColumnTo;

    bool mHighlightRows;
    bool mHighlightColumns;

    void onCursorMoved(Vector2i from, Vector2i to);

    std::function<bool(const InputCompactEvent&)> mUnhandledInputCallback;

    inline GridEntry* getCellAt(const Vector2i& pos) { return getCellAt(pos.x(), pos.y()); }

    // Update position & size
    void updateCellComponent(const GridEntry& cell);

    void updateSeparators();

  public:
    GridEntry* getCellAt(int x, int y);
};
