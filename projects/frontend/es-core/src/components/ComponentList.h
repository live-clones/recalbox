#pragma once

#include "IList.h"
#include <functional>
#include <input/InputCompactEvent.h>

struct ComponentListElement
{
	explicit ComponentListElement(std::shared_ptr<Component> cmp, bool resize_w, bool inv)
		: component(std::move(cmp)),
		  resize_width(resize_w),
		  invert_when_selected(inv)
  {
  };

	std::shared_ptr<Component> component;
	bool resize_width;
	bool invert_when_selected;
};

class IComponentListRow
{
  public:
    virtual void ComponentListRowSelected(int id) = 0;
};

class IComponentListRowEventInterceptor
{
  public:
    virtual bool EventReceived(int id, const InputCompactEvent& event) = 0;
};

struct ComponentListRow
{
  public:

    std::vector<ComponentListElement> elements;
    std::string name;

    explicit ComponentListRow(/*std::string n = std::string()*/)
      : name(/*n*/)
      , mInterceptor(nullptr)
      , mInterface(nullptr)
      , mIdentifier(0)
    {
    }

    inline void addElement(const std::shared_ptr<Component>& component, bool resize_width, bool invert_when_selected = true)
    {
      elements.push_back(ComponentListElement(component, resize_width, invert_when_selected));
    }

    bool ProcessInput(const InputCompactEvent& event)
    {
      if (mInterceptor != nullptr)
        return mInterceptor->EventReceived(mIdentifier, event);

      if(event.ValidPressed())
      {
        if (mInterface != nullptr) { mInterface->ComponentListRowSelected(mIdentifier); return true; }
        if (input_handler) { input_handler(); return true; }
      }
      if(event.YPressed())
        if (help_handler) { help_handler(); return true; }

      // No input handler assigned, do the default, which is to give it to the rightmost element in the row
      if(!elements.empty())
        if(elements.back().component->ProcessInput(event))
          return true;

      return false;
    }

    inline void SetEventInterceptor(int id, IComponentListRowEventInterceptor* interceptor)
    {
      mIdentifier =id;
      mInterceptor = interceptor;
    }

    inline void SetCallbackInterface(int id, IComponentListRow* interface)
    {
      mIdentifier = id;
      mInterface = interface;
    }

    // Utility method for making an input handler for "when the users presses B on this, do func."
    inline void makeAcceptInputHandler(const std::function<void()>& func)
    {
      input_handler = func;
    }

    inline void makeHelpInputHandler(const std::function<void()> &func)
    {
      help_handler = func;
    }

    inline bool HasHelpHandler() { return help_handler != nullptr; }

  private:
    // The input handler is called when the user enters any input while this row is highlighted (including up/down).
    // Return false to let the list try to use it or true if the input has been consumed.
    // If no input handler is supplied (input_handler == nullptr), the default behavior is to forward the input to
    // the rightmost element in the currently selected row.

    std::function<void()> input_handler;
    std::function<void()> help_handler;
    IComponentListRowEventInterceptor* mInterceptor;
    IComponentListRow* mInterface;
    int mIdentifier;
};

class ComponentList : public IList<ComponentListRow, void*>
{
public:
	explicit ComponentList(WindowManager&window);

	void addRow(const ComponentListRow& row, bool setCursorHere = false, bool updateGeometry = true);

	void textInput(const char* text) override;
	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;
	bool getHelpPrompts(Help& help) override;

	void onSizeChanged() override;
	void onFocusGained() override;
	void onFocusLost() override;

	bool moveCursor(int amt);
	inline int getCursorId() const { return mCursor; }

	float getTotalRowHeight() const;
	inline float getRowHeight(int row) const { return getRowHeight(mEntries[row].data); }

	inline void setCursorChangedCallback(const std::function<void(CursorState state)>& callback) { mCursorChangedCallback = callback; };
	inline void setFocusGainedCallback(const std::function<void()>& callback) { mFocusGainedCallback = callback; };
	inline void setFocusLostCallback(const std::function<void()>& callback) { mFocusLostCallback = callback; };
	inline const std::function<void(CursorState state)>& getCursorChangedCallback() const { return mCursorChangedCallback; };

protected:
	void onCursorChanged(const CursorState& state) override;

private:
	bool mFocused;

	void updateCameraOffset();
	void updateElementPosition(const ComponentListRow& row);
	void updateElementSize(const ComponentListRow& row);
	
	static float getRowHeight(const ComponentListRow& row) ;

	float mSelectorBarOffset;
	float mCameraOffset;

	std::function<void(CursorState state)> mCursorChangedCallback;
	std::function<void()> mFocusGainedCallback;
	std::function<void()> mFocusLostCallback;
};
