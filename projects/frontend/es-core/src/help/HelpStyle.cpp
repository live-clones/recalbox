#include <help/HelpStyle.h>
#include <themes/ThemeData.h>
#include <themes/Properties.h>
#include <Renderer.h>
#include <utils/math/Misc.h>
#include <resources/Font.h>

HelpStyle::HelpStyle()
  : mFont(Font::get(FONT_SIZE_SMALL)),
    mPosition(Renderer::Instance().DisplayWidthAsFloat() * 0.012f, Renderer::Instance().DisplayHeightAsFloat() * 0.9515f),
    mIconColor(0x777777FF),
    mTextColor(0x777777FF)
{
}

void HelpStyle::FromTheme(const ThemeData& theme, const std::string& view)
{
	const auto *elem = theme.getElement(view, "help", "helpsystem");
	if(elem == nullptr)
		return;

	for(int i=Help::TypeCount(); --i>=0; )
	  mImagesPath[i] = Path::Empty;

	if(elem->HasProperty("pos"))
		mPosition = elem->AsVector("pos") * Vector2f(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());

	if(elem->HasProperty("textColor"))
		mTextColor = (unsigned int)elem->AsInt("textColor");

	if(elem->HasProperty("iconColor"))
		mIconColor = (unsigned int)elem->AsInt("iconColor");

	if(elem->HasProperty("fontPath") || elem->HasProperty("fontSize"))
		mFont = Font::getFromTheme(elem, ThemeProperties::All, mFont);
	
	if(elem->HasProperty("iconUpDown"))
    mImagesPath[(int)HelpType::UpDown] = Path(elem->AsString("iconUpDown"));
	
	if(elem->HasProperty("iconLeftRight"))
    mImagesPath[(int)HelpType::LeftRight] = Path(elem->AsString("iconLeftRight"));
	
	if(elem->HasProperty("iconUpDownLeftRight"))
    mImagesPath[(int)HelpType::AllDirections] = Path(elem->AsString("iconUpDownLeftRight"));
	
	if(elem->HasProperty("iconA"))
    mImagesPath[(int)HelpType::A] = Path(elem->AsString("iconA"));
	
	if(elem->HasProperty("iconB"))
    mImagesPath[(int)HelpType::B] = Path(elem->AsString("iconB"));
	
	if(elem->HasProperty("iconX"))
    mImagesPath[(int)HelpType::X] = Path(elem->AsString("iconX"));
	
	if(elem->HasProperty("iconY"))
    mImagesPath[(int)HelpType::Y] = Path(elem->AsString("iconY"));
	
	if(elem->HasProperty("iconL"))
    mImagesPath[(int)HelpType::L] = Path(elem->AsString("iconL"));
	
	if(elem->HasProperty("iconR"))
    mImagesPath[(int)HelpType::R] = Path(elem->AsString("iconR"));
	
	if(elem->HasProperty("iconStart"))
    mImagesPath[(int)HelpType::Start] = Path(elem->AsString("iconStart"));
	
	if(elem->HasProperty("iconSelect"))
    mImagesPath[(int)HelpType::Select] = Path(elem->AsString("iconSelect"));
}
