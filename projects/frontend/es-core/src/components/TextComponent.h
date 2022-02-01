#pragma once

#include "components/base/Component.h"
#include "resources/Font.h"

class ThemeData;

// Used to display text.
// TextComponent::setSize(x, y) works a little differently than most components:
//  * (0, 0)                     - will automatically calculate a size that fits the text on one line (expand horizontally)
//  * (x != 0, 0)                - wrap text so that it does not reach beyond x. Will automatically calculate a vertical size (expand vertically).
//  * (x != 0, y <= fontHeight)  - will truncate text so it fits within this box.
class TextComponent : public Component
{
public:
	explicit TextComponent(WindowManager&window);
  TextComponent(WindowManager&window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color);
  TextComponent(WindowManager&window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, TextAlignment align);
	TextComponent(WindowManager&window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, TextAlignment align,
                Vector3f pos, Vector2f size, unsigned int bgcolor);

	void setFont(const std::shared_ptr<Font>& font);
	void setUppercase(bool uppercase);
	void onSizeChanged() override;
	void setText(const std::string& text);
	void setColor(unsigned int color) override;
	inline void setOriginColor(unsigned int color) { mOriginColor = color; }
	unsigned int getOriginColor() override { return mOriginColor; }

  void setHorizontalAlignment(TextAlignment align);
	void setVerticalAlignment(TextAlignment align) { mVerticalAlignment = align; }

	void setLineSpacing(float spacing);
	void setBackgroundColor(unsigned int color);
	void setRenderBackground(bool render) { mRenderBackground = render; }

	void Render(const Transform4x4f& parentTrans) override;

	std::string getValue() const override { return mText; }
	void setValue(const std::string& value) override { setText(value); }

	unsigned char getOpacity() const override {	return (unsigned char)(mColor & 0xFF); }

  void setOpacity(unsigned char opacity) override;
	
	inline std::shared_ptr<Font> getFont() const { return mFont; }

	void applyTheme(const ThemeData& theme, const std::string& view, const std::string& element, ThemeProperties properties) override;

private:
	void calculateExtent();

	void onTextChanged();
	void onColorChanged();

  std::shared_ptr<Font> mFont;
  std::shared_ptr<TextCache> mTextCache;
  std::string mText;
	unsigned int mColor;
	unsigned int mOriginColor;
	unsigned int mBgColor;
  float mLineSpacing;
	unsigned char mColorOpacity;
	unsigned char mBgColorOpacity;
  TextAlignment mHorizontalAlignment;
  TextAlignment mVerticalAlignment;
	bool mRenderBackground;
	bool mUppercase;
	bool mAutoCalcExtentX;
  bool mAutoCalcExtentY;
};
