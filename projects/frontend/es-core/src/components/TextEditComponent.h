#pragma once

#include "components/base/Component.h"
#include "components/NinePatchComponent.h"

class Font;
class TextCache;

// Used to enter text.
class TextEditComponent : public Component
{
public:
	explicit TextEditComponent(Window&window);
	
	void textInput(const char* text) override;
	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;

	void onFocusGained() override;
	void onFocusLost() final;

	void onSizeChanged() override;

	void setValue(const std::string& val) override;
	std::string getValue() const override { return mText; }

	inline bool isEditing() const { return mEditing; };
	inline const std::shared_ptr<Font>& getFont() const { return mFont; }

	void setCursor(size_t pos);

	bool getHelpPrompts(Help& help) override;

	void startEditing();
	void stopEditing();

private:
	void onTextChanged();
	void onCursorChanged();

	void updateCursorRepeat(int deltaTime);
	void moveCursor(int amt);

	bool isMultiline();
	static Vector2f getTextAreaPos() ;
	Vector2f getTextAreaSize() const;

	std::string mText;
	bool mFocused;
	bool mEditing;
	int mCursor; // cursor position in characters

	int mCursorRepeatTimer;
	int mCursorRepeatDir;

	Vector2f mScrollOffset;

	NinePatchComponent mBox;

	std::shared_ptr<Font> mFont;
	std::unique_ptr<TextCache> mTextCache;
};
