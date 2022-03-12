//
// Created by bkg2k on 06/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GuiMenuGameSettings.h"
#include "GuiMenuTools.h"
#include "GuiMenuNetplay.h"
#include "GuiMenuRetroAchievements.h"
#include <components/OptionListComponent.h>
#include <systems/SystemManager.h>
#include <guis/MenuMessages.h>
#include <LibretroRatio.h>
#include <algorithm>

GuiMenuGameSettings::GuiMenuGameSettings(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("GAMES SETTINGS"), this)
  , mSystemManager(systemManager)
  , mOriginalPreinstalled(false)
{
  // Screen ratio choice
  bool isCrt = Board::Instance().CrtBoard().GetCrtAdapter() != CrtAdapterType::None;
  if (! isCrt && RecalboxConf::Instance().GetMenuType() != RecalboxConf::Menu::Bartop)
    mRatio = AddList<std::string>(_("GAME RATIO"), (int)Components::Ratio, this, GetRatioEntries(), _(MENUMESSAGE_GAME_RATIO_HELP_MSG));

  // RecalboxOverlays
  mRecalboxOverlays = AddSwitch(_("RECALBOX OVERLAYS"), RecalboxConf::Instance().GetGlobalRecalboxOverlays(), (int)Components::RecalboxOverlays, this, _(MENUMESSAGE_GAME_OVERLAYS_HELP_MSG));

  // smoothing
  mSmooth = AddSwitch(_("SMOOTH GAMES"), RecalboxConf::Instance().GetGlobalSmooth(), (int)Components::Smooth, this, _(MENUMESSAGE_GAME_SMOOTH_HELP_MSG));

  // rewind
  mRewind = AddSwitch(_("REWIND"), RecalboxConf::Instance().GetGlobalRewind(), (int)Components::Rewind, this,_(MENUMESSAGE_GAME_REWIND_HELP_MSG));

  // autosave
  mAutoSave = AddSwitch(_("AUTO SAVE/LOAD"), RecalboxConf::Instance().GetGlobalAutoSave(), (int)Components::AutoSave, this, _(MENUMESSAGE_GAME_AUTOSAVELOAD_HELP_MSG));

  // Press twice to quit
  mQuitTwice = AddSwitch(_("PRESS TWICE TO QUIT GAME"), RecalboxConf::Instance().GetGlobalQuitTwice(), (int)Components::QuitTwice, this, _(MENUMESSAGE_GAME_PRESS_TWICE_QUIT_HELP_MSG));

  // Integer Scale
  if(!isCrt)
    mIntegerScale = AddSwitch(_("INTEGER SCALE (PIXEL PERFECT)"), RecalboxConf::Instance().GetGlobalIntegerScale(), (int)Components::IntegerScale, this, _(MENUMESSAGE_GAME_INTEGER_SCALE_HELP_MSG));

  // Shaders
  if(!isCrt)
    mShaders = AddList<std::string>(_("SHADERS"), (int)Components::Shaders, this, GetShadersEntries(), _(MENUMESSAGE_GAME_SHADERS_HELP_MSG));

  // Shaders preset
  if(!isCrt)
    mShaderSet = AddList<std::string>(_("SHADERS SET"), (int)Components::ShaderSet, this, GetShaderPresetsEntries(), _(MENUMESSAGE_GAME_SHADERSET_HELP_MSG));

  // Retroachievements
  if (RecalboxConf::Instance().GetMenuType() != RecalboxConf::Menu::Bartop)
  {
    AddSubMenu(_("RETROACHIEVEMENTS SETTINGS"), (int)Components::RetroAchivements, _(MENUMESSAGE_RA_HELP_MSG));
    AddSubMenu(_("NETPLAY SETTINGS"), (int)Components::Netplay, _(MENUMESSAGE_NP_HELP_MSG));
  }

  // Hide preinstalled games
  mHidePreinstalled = AddSwitch(_("HIDE PREINSTALLED GAMES"), mOriginalPreinstalled = RecalboxConf::Instance().GetGlobalHidePreinstalled(), (int)Components::HidePreinstalled, this, _(MENUMESSAGE_GAME_HIDE_PREINSTALLED));
}

GuiMenuGameSettings::~GuiMenuGameSettings()
{
  if (mHidePreinstalled->getState() != mOriginalPreinstalled)
    RequestRelaunch();
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuGameSettings::GetRatioEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string currentRatio = RecalboxConf::Instance().GetGlobalRatio();
  for (const auto& ratio : LibretroRatio::GetRatio())
    list.push_back({ ratio.first, ratio.second, currentRatio == ratio.second });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuGameSettings::GetShadersEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  GuiMenuTools::ShaderList shaderList = GuiMenuTools::ListShaders();
  std::string currentShader = RecalboxConf::Instance().GetGlobalShaders();
  list.push_back({ _("NONE"), "", currentShader.empty() });
  for (const GuiMenuTools::Shader& shader : shaderList)
    list.push_back({ shader.Displayable, shader.ShaderPath.ToString(), currentShader == shader.ShaderPath.ToString() });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuGameSettings::GetShaderPresetsEntries()
{
  std::vector<GuiMenuBase::ListEntry<std::string>> list;

  std::string currentPreset = RecalboxConf::Instance().GetGlobalShaderSet();
  if (currentPreset != "scanlines" && currentPreset != "retro") currentPreset = "none";
  list.push_back({ _("NONE"), "none", currentPreset == "none" });
  list.push_back({ _("SCANLINES"), "scanlines", currentPreset == "scanlines" });
  list.push_back({ _("RETRO"), "retro", currentPreset == "retro" });

  return list;
}

void GuiMenuGameSettings::SubMenuSelected(int id)
{
  if ((Components)id == Components::RetroAchivements) mWindow.pushGui(new GuiMenuRetroAchievements(mWindow));
  else if ((Components)id == Components::Netplay) mWindow.pushGui(new GuiMenuNetplay(mWindow, mSystemManager));
}

void GuiMenuGameSettings::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  if ((Components)id == Components::Ratio) RecalboxConf::Instance().SetGlobalRatio(value).Save();
  else if ((Components)id == Components::Shaders) RecalboxConf::Instance().SetGlobalShaders(value).Save();
  else if ((Components)id == Components::ShaderSet) RecalboxConf::Instance().SetGlobalShaderSet(value).Save();
}

void GuiMenuGameSettings::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Smooth: RecalboxConf::Instance().SetGlobalSmooth(status).Save(); break;
    case Components::RecalboxOverlays: RecalboxConf::Instance().SetGlobalRecalboxOverlays(status).Save(); break;
    case Components::Rewind: RecalboxConf::Instance().SetGlobalRewind(status).Save(); break;
    case Components::AutoSave: RecalboxConf::Instance().SetGlobalAutoSave(status).Save(); break;
    case Components::QuitTwice: RecalboxConf::Instance().SetGlobalQuitTwice(status).Save(); break;
    case Components::IntegerScale: RecalboxConf::Instance().SetGlobalIntegerScale(status).Save(); break;
    case Components::HidePreinstalled: RecalboxConf::Instance().SetGlobalHidePreinstalled(status).Save(); break;
    case Components::Ratio:
    case Components::Shaders:
    case Components::ShaderSet:
    case Components::RetroAchivements:
    case Components::Netplay: break;
  }
}

