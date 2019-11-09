#include "HelpStyle.h"
#include "themes/ThemeData.h"
#include "themes/Properties.h"
#include "Renderer.h"
#include "resources/Font.h"

HelpStyle::HelpStyle()
  : position(Renderer::getDisplayWidthAsFloat() * 0.012f, Renderer::getDisplayHeightAsFloat() * 0.9515f),
    iconColor(0x777777FF),
    textColor(0x777777FF),
    font(Font::get(FONT_SIZE_SMALL))
{
	iconMap.clear();
}

void HelpStyle::applyTheme(const ThemeData& theme, const std::string& view)
{
	auto elem = theme.getElement(view, "help", "helpsystem");
	if(elem == nullptr)
		return;

	if(elem->HasProperty("pos"))
		position = elem->AsVector("pos") * Vector2f(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

	if(elem->HasProperty("textColor"))
		textColor = (unsigned int)elem->AsInt("textColor");

	if(elem->HasProperty("iconColor"))
		iconColor = (unsigned int)elem->AsInt("iconColor");

	if(elem->HasProperty("fontPath") || elem->HasProperty("fontSize"))
		font = Font::getFromTheme(elem, ThemeProperties::All, font);
	
	if(elem->HasProperty("iconUpDown"))
		iconMap["up/down"] = elem->AsString("iconUpDown");
	
	if(elem->HasProperty("iconLeftRight"))
		iconMap["left/right"] = elem->AsString("iconLeftRight");
	
	if(elem->HasProperty("iconUpDownLeftRight"))
		iconMap["up/down/left/right"] = elem->AsString("iconUpDownLeftRight");
	
	if(elem->HasProperty("iconA"))
		iconMap["a"] = elem->AsString("iconA");
	
	if(elem->HasProperty("iconB"))
		iconMap["b"] = elem->AsString("iconB");
	
	if(elem->HasProperty("iconX"))
		iconMap["x"] = elem->AsString("iconX");
	
	if(elem->HasProperty("iconY"))
		iconMap["y"] = elem->AsString("iconY");
	
	if(elem->HasProperty("iconL"))
		iconMap["l"] = elem->AsString("iconL");
	
	if(elem->HasProperty("iconR"))
		iconMap["r"] = elem->AsString("iconR");
	
	if(elem->HasProperty("iconStart"))
		iconMap["start"] = elem->AsString("iconStart");
	
	if(elem->HasProperty("iconSelect"))
		iconMap["select"] = elem->AsString("iconSelect");
}
