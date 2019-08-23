#pragma once

#include "GuiComponent.h"

enum class UpdateType : unsigned char // Take less memory in grid elements
{
  Always,
  WhenSelected,
  Never,
};

enum class Borders : unsigned char // Take less memory in grid elements
{
  None   = 0,
  Top    = (1 << 0),
  Bottom = (1 << 1),
  Left   = (1 << 2),
  Right  = (1 << 3),
};
DEFINE_BITFLAG_ENUM(Borders, unsigned int)

// Used to arrange a bunch of components in a spreadsheet-esque grid.
class ComponentGrid : public GuiComponent
{
public:
	ComponentGrid(Window* window, const Vector2i& gridDimensions);
	~ComponentGrid() override;

	bool removeEntry(const std::shared_ptr<GuiComponent>& comp);

	void setEntry(const std::shared_ptr<GuiComponent>& comp, const Vector2i& pos, bool canFocus, bool resize = true,
		const Vector2i& size = Vector2i(1, 1), Borders border = Borders::None, UpdateType updateType = UpdateType::Always);

	void textInput(const char* text) override;
	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void render(const Transform4x4f& parentTrans) override;
	void onSizeChanged() override;

	void resetCursor();
	bool cursorValid();

	float getColWidth(int col);
	float getRowHeight(int row);

	void setColWidthPerc(int col, float width, bool update = true); // if update is false, will not call an onSizeChanged() which triggers a (potentially costly) repositioning + resizing of every element
	void setRowHeightPerc(int row, float height, bool update = true); // if update is false, will not call an onSizeChanged() which triggers a (potentially costly) repositioning + resizing of every element

	bool moveCursor(Vector2i dir);
	void setCursorTo(const std::shared_ptr<GuiComponent>& comp);

	inline void setUnhandledInputCallback(const std::function<bool(InputConfig* config, Input input)>& func) { mUnhandledInputCallback = func; }

	inline std::shared_ptr<GuiComponent> getSelectedComponent()
	{
		GridEntry* e = getCellAt(mCursor);
		if(e != nullptr)
			return e->component;
		else
			return nullptr;
	}

	void onFocusLost() override;
	void onFocusGained() override;

	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	class GridEntry
	{
    public:
      Vector2i pos;
      Vector2i dim;
      std::shared_ptr<GuiComponent> component;
      UpdateType updateType;
      Borders border;
      bool canFocus;
      bool resize;

      explicit GridEntry(const Vector2i& p = Vector2i::Zero(), const Vector2i& d = Vector2i::Zero(),
        std::shared_ptr<GuiComponent> cmp = nullptr, bool f = false, bool r = true,
        UpdateType u = UpdateType::Always, Borders b = Borders::None) :
        pos(p), dim(d), component(std::move(cmp)), updateType(u), border(b), canFocus(f), resize(r)
      {};

      explicit operator bool() const
      {
        return component != nullptr;
      }
	};

	float* mRowHeights;
	float* mColWidths;
	
	struct Vert
	{
		explicit Vert(float xi = 0, float yi = 0) : x(xi), y(yi) {};
		float x;
		float y;
	};

	std::vector<Vert> mLines;
	std::vector<unsigned int> mLineColors;

	// Update position & size
	void updateCellComponent(const GridEntry& cell);
	void updateSeparators();

	GridEntry* getCellAt(int x, int y);
	inline GridEntry* getCellAt(const Vector2i& pos) { return getCellAt(pos.x(), pos.y()); }
	
	Vector2i mGridSize;

	std::vector<GridEntry> mCells;

	void onCursorMoved(Vector2i from, Vector2i to);
	Vector2i mCursor;

	std::function<bool(InputConfig* config, Input input)> mUnhandledInputCallback;
};
