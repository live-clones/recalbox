//
// Created by gugue_u on 04/01/2023.
//

#include <components/ButtonComponent.h>
#include <components/MenuComponent.h>
#include <guis/GuiTextEditPopupKeyboard.h>
#include <views/ViewController.h>
#include "GuiSaveStates.h"
#include <games/GameFilesUtils.h>
#include <games/SaveState.h>


#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + Renderer::Instance().DisplayHeightAsFloat()*0.0437f )

GuiSaveStates::GuiSaveStates(WindowManager& window, SystemManager& systemManager, FileData& game, const std::function<void(const std::string& slot)>& func, bool fromMenu)
  : Gui(window),
    mSystemManager(systemManager),
    mBackground(window, Path(":/frame.png")),
    mGrid(window, Vector2i(3, 2)),
    mList(nullptr),
    mGame(game),
    mFromMenu(fromMenu),
    mCurrentState(Path("")),
    mSort(Sort::SLOT_DESC)
{


  mFunc = std::move(func);
  mIsLibretro = mSystemManager.Emulators().GetGameEmulator(mGame).IsLibretro();
  addChild(&mBackground);
  addChild(&mGrid);

  mMenuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  mBackground.setImagePath(mMenuTheme->menuBackground.path);
  mBackground.setCenterColor(mMenuTheme->menuBackground.color);
  mBackground.setEdgeColor(mMenuTheme->menuBackground.color);

  initGridsNStuff();

  const float height = Renderer::Instance().DisplayHeightAsFloat() * 0.75f;
  const float width = Renderer::Instance().DisplayWidthAsFloat() * 0.85f;
  setSize(width, height);

  setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2,
              (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);

  PopulateGrid();
}

void GuiSaveStates::initGridsNStuff()
{
  //init Title
  mTitle = std::make_shared<TextComponent>(mWindow, _("SAVE STATES"), mMenuTheme->menuText.font,
                                           mMenuTheme->menuText.color, TextAlignment::Right
                                          );
  mGrid.setEntry(mTitle, Vector2i(0, 0), false, true, Vector2i(1, 1));

  mList = std::make_shared<ComponentList>(mWindow);

  mList->setCursorChangedCallback([this](CursorState state)
                                  {
                                    (void) state;
                                    if (mList->getChildCount()>0)
                                      updateInformations(mList->getCursor());
                                  });

  mGridBody = std::make_shared<ComponentGrid>(mWindow, Vector2i(3, 4));

  mGridBody->setEntry(mList, Vector2i(0, 0), true, true, Vector2i(1, 4));

  mGrid.setEntry(mGridBody, Vector2i(0, 1), true, true, Vector2i(2, 1));


  mGameName = std::make_shared<TextComponent>(mWindow, mGame.Name() + " [" + mGame.RomPath().Filename() +"]", mMenuTheme->menuText.font,
                                              mMenuTheme->menuText.color, TextAlignment::Right);
  mGridBody->setEntry(mGameName, Vector2i(2, 0), false, true, Vector2i(1, 1));

  mStateField = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Right);
  mGridBody->setEntry(mStateField, Vector2i(2, 1), false, true, Vector2i(1, 1));

  mModificationTimeField = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Right);
  mGridBody->setEntry(mModificationTimeField, Vector2i(2, 2), false, true, Vector2i(1, 1));

  mThumbnail = std::make_shared<ImageComponent>(mWindow);
  mGridBody->setEntry(mThumbnail, Vector2i(2, 3), false, true, Vector2i(1, 1));


  //col with mList
  mGridBody->setColWidthPerc(0, 0.2f);
  mGridBody->setColWidthPerc(1, 0.1f);
  mGridBody->setColWidthPerc(2, 0.7f);
  //cols with metadata
  //spacer row
  mGridBody->setRowHeightPerc(0, 0.0825f);
  mGridBody->setRowHeightPerc(1, 0.0825f);
  mGridBody->setRowHeightPerc(2, 0.0825f);
  mGridBody->setRowHeightPerc(3, 0.7f);
}

bool GuiSaveStates::ProcessInput(const class InputCompactEvent & event)
{
  if (event.CancelPressed())
  {
    Close();
    return true;
  }
  else if (event.YPressed())
  {
    launch(Strings::Empty);

    return true;
  }
  else if (event.ValidPressed() && mIsLibretro && mCurrentState.GetPath().Extension() != ".auto" && mList->size() != 0)
  {
    std::string slot = Strings::ToString(mCurrentState.GetSlotNumber());
    launch(slot);

    return true;
  }
  else if (event.SelectPressed())
  {
    if(mSort == Sort::SLOT_ASC)
      mSort = Sort::SLOT_DESC;
    else if(mSort == Sort::SLOT_DESC)
      mSort = Sort::SLOT_ASC;

    PopulateGrid();

  }
  else if (event.XPressed())
  {
    std::string message = _("Game").append(": ").append(mGame.Name()).append("\n");
    message.append(_("You are about to delete this state, confirm ?"));
    message.append("\n\n");
    message.append(mCurrentState.GetPath().Filename());

    auto* msg = new GuiMsgBoxScroll(mWindow, _("DELETE STATE, CONFIRM?"), message,
                                   _("YES"), [this]
                                   {
                                     Delete();
                                     PopulateGrid();
                                   },
                                   _("BACK"), {}, "", nullptr, TextAlignment::Left);

    msg->SetDefaultButton(1);

    mWindow.pushGui(msg);
    return true;
  }

  return Component::ProcessInput(event);
}

void GuiSaveStates::onSizeChanged()
{
  mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

  // update grid row/col sizes
  mGrid.setColWidthPerc(0, 0.5f);
  mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());

  mGrid.setSize(mSize);
}

bool GuiSaveStates::getHelpPrompts(Help& help)
{
  help.Clear();
  std::string sort = mSort == Sort::SLOT_DESC ? "ASC": "DESC";

  help.Set(HelpType::UpDown, _("CHOOSE"))
      .Set(Help::Cancel(), _("BACK"))
      .Set(HelpType::Y, _("LAUNCH WITHOUT STATE"))
      .Set(HelpType::Select, _("CHANGE ORDER") + " " + sort);

    if (!mList->isEmpty())
    {
      if (mIsLibretro && mCurrentState.GetPath().Extension() != ".auto")
        help.Set(Help::Valid(), _("LAUNCH GAME FROM STATE"));
      help.Set(HelpType::X, _("DELETE STATE SLOT"));
    }
    return true;
}

void GuiSaveStates::Update(int deltaTime)
{
  Component::Update(deltaTime);
}

void GuiSaveStates::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  renderChildren(trans);

  Renderer::SetMatrix(trans);
  Renderer::DrawRectangle(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);
}

void GuiSaveStates::GetSaveStates()
{
  mSaveStates.clear();
  for (auto& state: GameFilesUtils::GetGameSaveStateFiles(mGame))
  {
    mSaveStates.push_back(state);
  }
}

void GuiSaveStates::PopulateGrid()
{
  GetSaveStates();

  mSort == Sort::SLOT_ASC ? mSaveStates.sort(GuiSaveStates::asc) : mSaveStates.sort(GuiSaveStates::desc);


  if (mList)
    mList->clear();

  ComponentListRow row;
  std::shared_ptr<Component> ed;

  for (auto& state : mSaveStates)
  {
    row.elements.clear();


    ed = std::make_shared<TextComponent>(mWindow, state.GetIsAuto() ? "slot: auto" : "slot: " + Strings::ToString(state.GetSlotNumber()),
                                         mMenuTheme->menuText.font, mMenuTheme->menuText.color,TextAlignment::Left);
    row.addElement(ed, true);
    mList->addRow(row, false, true);
  }

  clear();
  updateInformations(0);
}

void GuiSaveStates::clear()
{
  mThumbnail->setImage(Path::Empty);
  mModificationTimeField->setText(Strings::Empty);
  mStateField->setText(Strings::Empty);
}

//called when changing cursor in mList to populate MD
void GuiSaveStates::updateInformations(int i)
{
  clear();

  mCurrentState = GetStateAt(i);
  //screenshot & video
  mThumbnail->setImage(mCurrentState.GetThrumbnail());
  mThumbnail->setSize(mGridBody->getColWidth(2) * .75, mGridBody->getColWidth(2) * .75 * 3 / 4);
  mThumbnail->setPosition(mGridBody->getPosition().x() + mGridBody->getColWidth(0) + mGridBody->getColWidth(1) + mGridBody->getColWidth(2),
                          mGridBody->getPosition().y() + mGridBody->getRowHeight(0) * 2);
  mThumbnail->setOrigin(1,0);
  mModificationTimeField->setText(mCurrentState.GetModificationDate());
  mStateField->setText(mCurrentState.GetPath().Filename());

  updateHelpPrompts();
}


SaveState GuiSaveStates::GetStateAt(int index)
{
  int i = 0;
  for (auto& saveState : mSaveStates)
  {
    if (index == i)
    {
      return saveState;
    }
    i++;
  }

  return *new SaveState(Path(""));
}

void GuiSaveStates::launch(const std::string slot)
{
  if (mFromMenu)
  {
    Vector3f target(Renderer::Instance().DisplayWidthAsFloat() / 2.0f,
                      Renderer::Instance().DisplayHeightAsFloat() / 2.0f, 0);
    GameLinkedData gameLinkedData = GameLinkedData();
    gameLinkedData.ConfigurableSaveState().SetSlotNumber(slot);
    ViewController::Instance().Launch(&mGame, gameLinkedData, target);
  }
  else
  {
    mFunc(slot);
    Close();
  }
}

void GuiSaveStates::Delete()
{
  mCurrentState.GetPath().Delete();
  mCurrentState.GetThrumbnail().Delete();
  updateHelpPrompts();
  { LOG(LogDebug) << "[SAVESTATE] " << mCurrentState.GetPath().Filename() << " slot has been deleted"; }
}

//non compatibl libretro core pi4
//libretro-atari800
//libretro-easyrpg
//libretro-gw
//libretro-lowresnx
//libretro-lutro
//libretro-mame2010
//libretro-minivmac
//libretro-px68k
//libretro-retro8
//libretro-virtualjaguar
