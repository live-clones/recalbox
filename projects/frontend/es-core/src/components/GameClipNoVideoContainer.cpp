//
// Created by guillaume on 07/12/2020.
//

#include <WindowManager.h>
#include <utils/locale/LocaleHelper.h>
#include "GameClipNoVideoContainer.h"
#include "Renderer.h"

GameClipNoVideoContainer::GameClipNoVideoContainer(WindowManager& window)
  : Gui(window),
    mWarningText(window),
    mMire(window)

{
  setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());
  
  mWarningText.setFont(Font::get(FONT_SIZE_MEDIUM));
  mWarningText.setPosition(mSize.x() / 2, mSize.y() / 2);
  mWarningText.setSize(0, 0);
  mWarningText.setHorizontalAlignment(TextAlignment::Center);
  mWarningText.setOrigin(0.5f, 0.5f);
  mWarningText.setZIndex(50);
  mWarningText.setDefaultZIndex(50);
  mWarningText.setColor(0xFFFFFFFF);
  mWarningText.setText(_("Gameclips cannot be played. No video availabe for your selection"));
  mWarningText.setBackgroundColor(0x000000FF);
  mWarningText.setRenderBackground(true);
  addChild(&mWarningText);

  mMire.setImage(Path(":/mire_recalbox.svg"));
  mMire.setOrigin(0.5f, 0.5f);
  mMire.setPosition(mSize.x() / 2, mSize.y() / 2);
  mMire.setSize(mSize.x(), mSize.y());
  mMire.setZIndex(0);
  mMire.setDefaultZIndex(0);
  addChild(&mMire);
  sortChildren();
}

void GameClipNoVideoContainer::Render(const Transform4x4f& parentTrans)
{
  renderChildren(parentTrans);
}

bool GameClipNoVideoContainer::getHelpPrompts(Help& help)
{
  help.Set(Help::Cancel(), _("QUIT"));
  return true;
}
