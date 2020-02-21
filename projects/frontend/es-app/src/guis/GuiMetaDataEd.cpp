#include "guis/GuiMetaDataEd.h"
#include "Settings.h"
#include "guis/GuiMsgBox.h"
#include <RecalboxConf.h>
#include <components/SwitchComponent.h>
#include <LibretroRatio.h>

#include "components/DateTimeComponent.h"
#include "components/RatingComponent.h"
#include "guis/GuiTextEditPopup.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "MenuThemeData.h"
#include "components/OptionListComponent.h"
#include "recalbox/RecalboxSystem.h"

#include "games/MetadataDescriptor.h"
#include "games/MetadataFieldDescriptor.h"
#include "GuiScraperSingleGameRun.h"

GuiMetaDataEd::GuiMetaDataEd(Window& window,
                             SystemManager& systemManager,
                             MetadataDescriptor& md,
                             FileData& game,
                             const std::string& header,
                             std::function<void()> saveCallback,
                             std::function<void()> deleteFunc,
                             SystemData* system,
                             bool main)
  : Gui(window),
    mGame(game),
    mSystemManager(systemManager),
    mBackground(window, Path(":/frame.png")),
    mGrid(window, Vector2i(1, 3)),
    mMetaData(md),
    mSavedCallback(std::move(saveCallback)),
    mDeleteFunc(std::move(deleteFunc))
{
  addChild(&mBackground);
  addChild(&mGrid);

  mHeaderGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(1, 5));

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();

  mBackground.setImagePath(menuTheme->menuBackground.path);
  mBackground.setCenterColor(menuTheme->menuBackground.color);
  mBackground.setEdgeColor(menuTheme->menuBackground.color);

  mTitle = std::make_shared<TextComponent>(mWindow, _("EDIT METADATA"), menuTheme->menuTitle.font, menuTheme->menuTitle.color,
                                           TextAlignment::Center);
  mSubtitle = std::make_shared<TextComponent>(mWindow, Strings::ToUpperASCII(game.getPath().Filename()),
                                              menuTheme->menuFooter.font, menuTheme->menuFooter.color, TextAlignment::Center);
  float y = 0;
  y += mTitle->getFont()->getHeight() + mSubtitle->getFont()->getHeight();
  mHeaderGrid->setEntry(mTitle, Vector2i(0, 1), false, true);
  mHeaderGrid->setEntry(mSubtitle, Vector2i(0, 3), false, true);

  mGrid.setEntry(mHeaderGrid, Vector2i(0, 0), false, true);

  mList = std::make_shared<ComponentList>(mWindow);
  mGrid.setEntry(mList, Vector2i(0, 1), true, true);

  auto emu_choice = std::make_shared<OptionListComponent<std::string>>(mWindow, _("Emulator"), false, FONT_SIZE_MEDIUM);
  auto core_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, _("core"), false, FONT_SIZE_MEDIUM);

  // Get list
  int fieldCount = 0;
  const MetadataFieldDescriptor *fields = mMetaData.GetMetadataFieldDescriptors(fieldCount);

  // populate list
  for (; --fieldCount >= 0;)
  {
    const MetadataFieldDescriptor &field = fields[0];
    if (field.EditType() == MetadataFieldDescriptor::EditableType::None) continue;
    fields++;

    std::shared_ptr<Component> ed;

    // don't add statistics
    if (field.IsStatistic())
    {
      continue; // TODO: Still usefull (== EditableType::None?)
    }

    // filter on main or secondary entries depending on the requested type
    if (field.IsMain() != main)
    {
      continue;
    }

    // create ed and add it (and any related components) to mMenu
    // ed's value will be set below
    ComponentListRow row;
    auto lbl = std::make_shared<TextComponent>(mWindow, Strings::ToUpperUTF8(field.DisplayName()), menuTheme->menuText.font, menuTheme->menuText.color);

    row.addElement(lbl, true); // label
    y += lbl->getFont()->getHeight();

    switch (field.EditType())
    {
      case MetadataFieldDescriptor::EditableType::Rating:
      {
        ed = std::make_shared<RatingComponent>(window, menuTheme->menuText.color);

        ed->setSize(0, lbl->getSize().y() * 0.8f);
        row.addElement(ed, false, true);

        auto spacer = std::make_shared<Component>(mWindow);
        spacer->setSize(Renderer::getDisplayWidthAsFloat() * 0.0025f, 0);
        row.addElement(spacer, false);

        // pass input to the actual RatingComponent instead of the spacer
        row.input_handler = std::bind(&Component::ProcessInput, ed.get(), std::placeholders::_1/*, std::placeholders::_2*/);

        break;
      }
      case MetadataFieldDescriptor::EditableType::Date:
      {
        ed = std::make_shared<DateTimeComponent>(mWindow);
        row.addElement(ed, false);

        auto spacer = std::make_shared<Component>(mWindow);
        spacer->setSize(Renderer::getDisplayWidthAsFloat() * 0.0025f, 0);
        row.addElement(spacer, false);

        // pass input to the actual DateTimeComponent instead of the spacer
        row.input_handler = std::bind(&Component::ProcessInput, ed.get(), std::placeholders::_1/*, std::placeholders::_2*/);

        break;
      }
      case MetadataFieldDescriptor::EditableType::Switch:
      {
        auto switchComp = std::make_shared<SwitchComponent>(mWindow);
        GetValueMethodType method = field.GetValueMethod();
        switchComp->setState((mMetaData.*method)() == "true");
        ed = switchComp;
        row.addElement(ed, false);
        break;
      }
      case MetadataFieldDescriptor::EditableType::ListMulti:
      case MetadataFieldDescriptor::EditableType::List:
        if (field.Key() == "emulator")
        {
          row.addElement(emu_choice, false);

          std::string defaultEmulator;
          std::string defaultCore;
          mSystemManager.Emulators().GetSystemDefaultEmulator(*system, defaultEmulator, defaultCore);

          std::string currentEmulator = RecalboxConf::Instance().AsString(system->getName() + ".emulator");
          if (currentEmulator.empty()) currentEmulator = defaultEmulator;
          for (const std::string& emulatorName : mSystemManager.Emulators().GetEmulators(*system))
          {
            std::string displayName = emulatorName;
            if (displayName == defaultEmulator) displayName.append(" (").append(_("DEFAULT")).append(1, ')');
            emu_choice->add(displayName, emulatorName, emulatorName == currentEmulator);
          }

          // when emulator changes, load new core list
          emu_choice->setSelectedChangedCallback([this, system, defaultEmulator, defaultCore, core_choice](const std::string& emulatorName)
          {
            core_choice->clear();

            bool isDefaultEmulator = (emulatorName == defaultEmulator);
            std::string currentCore = RecalboxConf::Instance().AsString(system->getName() + ".core");
            if (currentCore.empty()) currentCore = defaultCore;
            for (const std::string& coreName : mSystemManager.Emulators().GetCores(*system, emulatorName))
            {
              std::string displayName = coreName;
              if (displayName == defaultCore && isDefaultEmulator) displayName.append(" (").append(_("DEFAULT")).append(1, ')');
              core_choice->add(displayName, coreName, coreName == currentCore);
            }
          });

          ed = emu_choice;
        }
        else if (field.Key() == "core")
        {
          row.addElement(core_choice, false);
          ed = core_choice;

          // force change event to load core list
          emu_choice->invalidate();
        }
        else if (field.Key() == "ratio")
        {
          auto ratio_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, "ratio", false,
                                                                                  FONT_SIZE_MEDIUM);
          row.addElement(ratio_choice, false);
          const std::map<std::string, std::string>& ratioMap = LibretroRatio::GetRatio();
          if (mMetaData.Ratio().empty())
          {
            mMetaData.SetRatio("auto");
          }
          for (auto& ratio : ratioMap)
          {
            ratio_choice->add(ratio.first, ratio.second, mMetaData.Ratio() == ratio.second);
          }
          ed = ratio_choice;
        }
        else if (field.Key() == "genreid")
        {
          auto genre_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, "genre", false,
                                                                                  FONT_SIZE_MEDIUM);
          row.addElement(genre_choice, false);
          Genres::GenreMap map = Genres::GetShortNameMap();
          for(auto & genre : Genres::GetOrderedList())
          {
            std::string genreString = Strings::ToString((int)genre);
            genre_choice->add((Genres::IsSubGenre(genre) ? "    " : "") + Genres::GetName(genre), genreString, mMetaData.GenreId() == genre);
          }
          ed = genre_choice;
        }
        break;
      case MetadataFieldDescriptor::EditableType::Text:
      {
        // MD_STRING
        ed = std::make_shared<TextComponent>(window, "", menuTheme->menuText.font, menuTheme->menuText.color, TextAlignment::Right);
        row.addElement(ed, true);

        auto spacer = std::make_shared<Component>(mWindow);
        spacer->setSize(Renderer::getDisplayWidthAsFloat() * 0.005f, 0);
        row.addElement(spacer, false);

        auto bracket = std::make_shared<ImageComponent>(mWindow);

        bracket->setImage(menuTheme->iconSet.arrow);
        bracket->setColorShift(menuTheme->menuText.color);
        bracket->setResize(Vector2f(0, lbl->getFont()->getLetterHeight()));
        row.addElement(bracket, false);

        bool multiLine = field.Type() == MetadataFieldDescriptor::DataType::Text;
        const std::string& title = field.DisplayPrompt();
        auto updateVal = [ed](const std::string &newVal)
        {
          ed->setValue(newVal);
        }; // ok callback (apply new value to ed)
        row.makeAcceptInputHandler([this, title, ed, updateVal, multiLine]
                                   {
                                     if (Settings::Instance().UseOSK() && (!multiLine))
                                     {
                                       mWindow.pushGui(new GuiTextEditPopupKeyboard(mWindow, title, ed->getValue(), updateVal, multiLine));
                                     }
                                     else
                                     {
                                       mWindow.pushGui(new GuiTextEditPopup(mWindow, title, ed->getValue(), updateVal, multiLine));
                                     }
                                   });
        break;
      }
      case MetadataFieldDescriptor::EditableType::None:
      default: break;
    }

    assert(ed);
    mList->addRow(row);
    GetValueMethodType method = field.GetValueMethod();
    ed->setValue((mMetaData.*method)());
    mEditors.push_back(ed);
    mMetaDataEditable.push_back(&field);
  }

  std::vector<std::shared_ptr<ButtonComponent> > buttons;

  if (main)
  {
    // append a shortcut to the secondary menu
    ComponentListRow row;
    auto lbl = std::make_shared<TextComponent>(mWindow, _("MORE DETAILS"), menuTheme->menuText.font, menuTheme->menuText.color);
    row.addElement(lbl, true);
    y += lbl->getFont()->getHeight();

    auto bracket = std::make_shared<ImageComponent>(mWindow);
    bracket->setImage(menuTheme->iconSet.arrow);
    bracket->setColorShift(menuTheme->menuText.color);

    bracket->setResize(Vector2f(0, lbl->getFont()->getLetterHeight()));
    row.addElement(bracket, false);

    row.makeAcceptInputHandler([this, header, system]
                               {
                                 // call the same Gui with "main" set to "false"
                                 mWindow.pushGui(new GuiMetaDataEd(mWindow, mSystemManager, mMetaData, mGame, header, nullptr, nullptr, system, false));
                               });

    mList->addRow(row);
  }


  if (main && !game.getSystem()->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
  {
    buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("SCRAPE"), _("SCRAPE"), std::bind(&GuiMetaDataEd::fetch, this)));
  }

  buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("SAVE"), _("SAVE"), [&]
  {
    save();
    Close();
  }));

  buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CANCEL"), _("CANCEL"), [&] { Close(); }));

  if (main && mDeleteFunc)
  {
    auto deleteFileAndSelf = [&]
    {
      mDeleteFunc();
      Close();
    };
    auto deleteBtnFunc = [this, deleteFileAndSelf]
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("THIS WILL DELETE A FILE!\nARE YOU SURE?"), _("YES"), deleteFileAndSelf, _("NO"), nullptr));
    };
    buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("DELETE"), _("DELETE"), deleteBtnFunc));
  }


  mButtons = makeButtonGrid(mWindow, buttons);
  mGrid.setEntry(mButtons, Vector2i(0, 2), true, false);

  mGrid.setUnhandledInputCallback([this](const InputCompactEvent& event) -> bool
                                  {
                                    if (event.AnyDownPressed())
                                    {
                                      mGrid.setCursorTo(mList);
                                      mList->setCursorIndex(0);
                                      return true;
                                    }
                                    if (event.AnyUpPressed())
                                    {
                                      mList->setCursorIndex(mList->size() - 1);
                                      mGrid.moveCursor(Vector2i(0, 1));
                                      return true;
                                    }
                                    return false;
                                  });

  // resize + center
  y /= Renderer::getDisplayHeightAsFloat();
  setSize(Renderer::getDisplayWidthAsFloat() * 0.95f, Renderer::getDisplayHeightAsFloat() * (y + 0.15f));
  setPosition((Renderer::getDisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::getDisplayHeightAsFloat() - mSize.y()) / 2);
}

void GuiMetaDataEd::onSizeChanged()
{
  mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

  mGrid.setSize(mSize);

  const float titleHeight = mTitle->getFont()->getLetterHeight();
  const float subtitleHeight = mSubtitle->getFont()->getLetterHeight();
  const float titleSubtitleSpacing = mSize.y() * 0.03f;

  mGrid.setRowHeightPerc(0, (titleHeight + titleSubtitleSpacing + subtitleHeight + TITLE_VERT_PADDING) / mSize.y());
  mGrid.setRowHeightPerc(2, mButtons->getSize().y() / mSize.y());

  mHeaderGrid->setRowHeightPerc(1, titleHeight / mHeaderGrid->getSize().y());
  mHeaderGrid->setRowHeightPerc(2, titleSubtitleSpacing / mHeaderGrid->getSize().y());
  mHeaderGrid->setRowHeightPerc(3, subtitleHeight / mHeaderGrid->getSize().y());
}

void GuiMetaDataEd::save()
{
  for (int i = 0; i < (int)mEditors.size(); i++)
  {
    if (mMetaDataEditable[i]->Type() != MetadataFieldDescriptor::DataType::PList)
    {
      (mMetaData.*(mMetaDataEditable[i]->SetValueMethod()))(mEditors[i]->getValue());
    }
    else
    {
      std::shared_ptr<Component> ed = mEditors[i];
      std::shared_ptr<OptionListComponent<std::string>> list = std::static_pointer_cast<OptionListComponent<std::string>>(ed);
      (mMetaData.*(mMetaDataEditable[i]->SetValueMethod()))(list->getSelected());
    }
  }

  if (mSavedCallback)
  {
    mSavedCallback();
  }
}

void GuiMetaDataEd::fetch()
{
  mWindow.pushGui(new GuiScraperSingleGameRun(mWindow, mGame, this));
}

void GuiMetaDataEd::ScrappingComplete(FileData& /*game*/)
{
  for (int i = 0; i < (int)mEditors.size(); i++)
  {
    GetValueMethodType method = mMetaDataEditable[i]->GetValueMethod();
    mEditors[i]->setValue((mMetaData.*method)());
  }
}

void GuiMetaDataEd::close(bool closeAllWindows)
{
  // find out if the user made any changes
  bool dirty = false;
  for (int i = 0; i < (int)mEditors.size(); i++)
  {
    GetValueMethodType method = mMetaDataEditable[i]->GetValueMethod();
    if ((mMetaData.*method)() != mEditors[i]->getValue())
    {
      dirty = true;
      break;
    }
  }

  std::function<void()> closeFunc;
  if (!closeAllWindows)
  {
    closeFunc = [this] { Close(); };
  }
  else
  {
    closeFunc = [this]
    {
      mWindow.CloseAll();
    };
  }


  if (dirty)
  {
    // changes were made, ask if the user wants to save them
    mWindow.pushGui(new GuiMsgBox(mWindow,
                                   _("SAVE CHANGES?"),
                                   _("YES"), [this, closeFunc]
                                   {
                                     save();
                                     closeFunc();
                                   },
                                   _("NO"), closeFunc
                                  ));
  }
  else
  {
    closeFunc();
  }
}

bool GuiMetaDataEd::ProcessInput(const InputCompactEvent& event)
{
  if (Component::ProcessInput(event))
  {
    return true;
  }

  if (event.APressed() || event.StartPressed())
  {
    close(event.StartPressed());
    return true;
  }

  return false;
}

bool GuiMetaDataEd::getHelpPrompts(Help& help)
{
  mGrid.getHelpPrompts(help);
  help.Set(HelpType::A, _("BACK"))
      .Set(HelpType::Start, _("CLOSE"));
  return true;
}
