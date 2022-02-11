//
// Created by bkg2k on 28/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/MenuMessages.h>
#include <guis/GuiScraperSingleGameRun.h>
#include "GuiMenuGamelistGameOptions.h"
#include "GuiMenuTools.h"
#include <components/EditableComponent.h>
#include <components/RatingComponent.h>
#include <components/SwitchComponent.h>
#include <views/ViewController.h>
#include <LibretroRatio.h>

GuiMenuGamelistGameOptions::GuiMenuGamelistGameOptions(WindowManager& window, IGameListView& view, SystemManager& systemManager, SystemData& system, FileData& game)
  : GuiMenuBase(window, _("GAME OPTIONS"), this)
  , mView(view)
  , mSystemManager(systemManager)
  , mSystem(system)
  , mGame(game)
{
  if (mGame.IsGame())
  {
    std::string gameName(game.Name());
    gameName.append(" (").append(game.RomPath().Filename()).append(1, ')');
    SetFooter(Strings::Replace(_("GAME %s"), "%s", Strings::ToUpperUTF8(gameName)));
  }
  else if (mGame.IsFolder())
  {
    SetFooter(Strings::Replace(_("FOLDER %s"), "%s", Strings::ToUpperUTF8(mGame.Name())));
  }

  // Run width
  if (mGame.IsGame())
    mEmulator = AddList<std::string>(_("RUN WITH"), (int)Components::Emulator, this, GetEmulatorEntries(), _(MENUMESSAGE_ADVANCED_EMU_EMU_HELP_MSG));

  // Ratio
  if (mGame.IsGame())
    mRatio = AddList<std::string>(_("Ratio"), (int)Components::Ratio, this, GetRatioEntries(), _(MENUMESSAGE_GAME_RATIO_HELP_MSG));

  // Game name
  mName = AddEditable(_("Name"), mGame.Metadata().Name(), (int)Components::Name, this, false);

  // Rating
  if (mGame.IsGame())
    mRating = AddRating(_("Rating"), mGame.Metadata().Rating(), (int)Components::Rating, this);

  // Normalized genre
  if (mGame.IsGame())
    mGenre = AddList<GameGenres>(_("Genre"), (int)Components::Genre, this, GetGenreEntries());

  // Description
  mDescription = AddEditable(_("Description"), mGame.Metadata().Description(), (int)Components::Description, this, false);

  // Favorite
  if (mGame.IsGame())
    mFavorite = AddSwitch(_("Favorite"), mGame.Metadata().Favorite(), (int)Components::Favorite, this);

  // Hidden
  mHidden = AddSwitch(_("Hidden"), mGame.Metadata().Hidden(), (int)Components::Hidden, this);

  // Adult
  if (mGame.IsGame())
    mAdult = AddSwitch(_("Adult"), mGame.Metadata().Adult(), (int)Components::Adult, this);

  // Scrape
  if (mGame.IsGame())
    AddSubMenu(_("SCRAPE"), (int)Components::Scrape);
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuGamelistGameOptions::GetRatioEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentRatio = mGame.Metadata().Ratio();
  for (const auto& ratio : LibretroRatio::GetRatio())
    list.push_back({ ratio.first, ratio.second, currentRatio == ratio.second });

  return list;
}

std::vector<GuiMenuBase::ListEntry<GameGenres>> GuiMenuGamelistGameOptions::GetGenreEntries()
{
  std::vector<ListEntry<GameGenres>> list;

  for(GameGenres genre : Genres::GetOrderedList())
    list.push_back({ Genres::GetName(genre), genre, genre == mGame.Metadata().GenreId() });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuGamelistGameOptions::GetEmulatorEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentEmulator(mGame.Metadata().Emulator());
  std::string currentCore    (mGame.Metadata().Core());
  GuiMenuTools::EmulatorAndCoreList eList =
    GuiMenuTools::ListEmulatorAndCore(mSystemManager, mGame.System(), mDefaultEmulator, mDefaultCore, currentEmulator, currentCore);
  if (!eList.empty())
    for (const GuiMenuTools::EmulatorAndCore& emulator : eList)
      list.push_back({ emulator.Displayable, emulator.Identifier, emulator.Selected });

  return list;
}

void GuiMenuGamelistGameOptions::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Emulator)
  {
    mGame.Metadata().SetEmulator(Strings::Empty);
    mGame.Metadata().SetCore(Strings::Empty);
    // Split emulator & core
    std::string emulator, core;
    if (Strings::SplitAt(value, ':', emulator, core, false))
      if (emulator != mDefaultEmulator || core != mDefaultCore)
      {
        mGame.Metadata().SetEmulator(emulator);
        mGame.Metadata().SetCore(core);
      }
  }
  else if ((Components)id == Components::Ratio)
    mGame.Metadata().SetRatio(value);
}

void GuiMenuGamelistGameOptions::OptionListComponentChanged(int id, int index, const GameGenres& value)
{
  (void)index;
  if ((Components)id == Components::Genre)
    mGame.Metadata().SetGenreId(value);
}

void GuiMenuGamelistGameOptions::EditableComponentTextChanged(int id, const std::string& text)
{
  if ((Components)id == Components::Name)
    mGame.Metadata().SetName(text);
  else if ((Components)id == Components::Description)
    mGame.Metadata().SetDescription(text);
}

void GuiMenuGamelistGameOptions::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Favorite:
    {
      mGame.Metadata().SetFavorite(status);
      SystemData* favoriteSystem = mSystemManager.FavoriteSystem();
      if (favoriteSystem != nullptr)
      {
        if (mGame.Metadata().Favorite())
        {
          favoriteSystem->GetFavoriteRoot().AddChild(&mGame, false);
        }
        else
        {
          favoriteSystem->GetFavoriteRoot().RemoveChild(&mGame);
        }
        ViewController::Instance().setInvalidGamesList(&mGame.System());
        ViewController::Instance().setInvalidGamesList(favoriteSystem);
      }
      ViewController::Instance().getSystemListView().manageFavorite();
      break;
    }
    case Components::Hidden: mGame.Metadata().SetHidden(status); break;
    case Components::Adult: mGame.Metadata().SetAdult(status); break;
    case Components::Name:
    case Components::Description:
    case Components::Rating:
    case Components::Genre:
    case Components::Scrape:
    case Components::Ratio:
    case Components::Emulator: break;
  }
}

void GuiMenuGamelistGameOptions::RatingChanged(int id, float value)
{
  if ((Components)id == Components::Rating)
    mGame.Metadata().SetRating(value);
}

void GuiMenuGamelistGameOptions::SubMenuSelected(int id)
{
  if ((Components)id == Components::Scrape)
    mWindow.pushGui(new GuiScraperSingleGameRun(mWindow, mGame, this));
}

void GuiMenuGamelistGameOptions::ScrappingComplete(FileData& game)
{
  mName->setText(game.Metadata().Name());
  mRating->setValue(game.Metadata().Rating());
  mGenre->select(game.Metadata().GenreId());
  mDescription->setText(game.Metadata().Description());
  mFavorite->setState(game.Metadata().Favorite());
  mHidden->setState(game.Metadata().Hidden());
  mAdult->setState(game.Metadata().Adult());
  mMenu.onSizeChanged();
}
