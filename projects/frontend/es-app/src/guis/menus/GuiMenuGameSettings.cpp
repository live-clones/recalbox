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
  : GuiMenuBase(window, _("GAMES SETTINGS"))
  , mSystemManager(systemManager)
  , mOriginalPreinstalled(false)
{
  // Screen ratio choice
  if (RecalboxConf::Instance().GetMenuType() != RecalboxConf::Menu::Bartop)
    mRatio = AddList<std::string>(_("GAME RATIO"), SetRatio, GetRatioEntries(), _(MENUMESSAGE_GAME_RATIO_HELP_MSG));

  // smoothing
  mSmooth = AddSwitch(_("SMOOTH GAMES"), RecalboxConf::Instance().GetGlobalSmooth(), SetSmooth, _(MENUMESSAGE_GAME_SMOOTH_HELP_MSG));

  // rewind
  mRewind = AddSwitch(_("REWIND"), RecalboxConf::Instance().GetGlobalRewind(), SetRewind,_(MENUMESSAGE_GAME_REWIND_HELP_MSG));

  // autosave
  mAutoSave = AddSwitch(_("AUTO SAVE/LOAD"), RecalboxConf::Instance().GetGlobalAutoSave(), SetAutoSave, _(MENUMESSAGE_GAME_AUTOSAVELOAD_HELP_MSG));

  // Press twice to quit
  mQuitTwice = AddSwitch(_("PRESS TWICE TO QUIT GAME"), RecalboxConf::Instance().GetGlobalQuitTwice(), SetQuitTwice, _(MENUMESSAGE_GAME_PRESS_TWICE_QUIT_HELP_MSG));

  // Integer Scale
  mIntegerScale = AddSwitch(_("INTEGER SCALE (PIXEL PERFECT)"), RecalboxConf::Instance().GetGlobalQuitTwice(), SetIntegerScale, _(MENUMESSAGE_GAME_INTEGER_SCALE_HELP_MSG));

  // Shaders
  mShaders = AddList<std::string>(_("SHADERS"), SetShaders, GetShadersEntries(), _(MENUMESSAGE_GAME_SHADERS_HELP_MSG));

  // Shaders preset
  mShaderSet = AddList<std::string>(_("SHADERS SET"), SetShaderSet, GetShaderPresetsEntries(), _(MENUMESSAGE_GAME_SHADERSET_HELP_MSG));

  // Retroachievements
  if (RecalboxConf::Instance().GetMenuType() != RecalboxConf::Menu::Bartop)
  {
    AddSubMenu(_("RETROACHIEVEMENTS SETTINGS"), [this] { mWindow.pushGui(new GuiMenuRetroAchievements(mWindow)); }, _(MENUMESSAGE_RA_HELP_MSG));
    AddSubMenu(_("NETPLAY SETTINGS"), [this] { mWindow.pushGui(new GuiMenuNetplay(mWindow, mSystemManager)); }, _(MENUMESSAGE_NP_HELP_MSG));
  }

  // Hide preinstalled games
  mHidePreinstalled = AddSwitch(_("HIDE PREINSTALLED GAMES"), mOriginalPreinstalled = RecalboxConf::Instance().GetGlobalHidePreinstalled(), SetPreinstalled, _(MENUMESSAGE_GAME_HIDE_PREINSTALLED));
}

GuiMenuGameSettings::~GuiMenuGameSettings()
{
  if (mHidePreinstalled->getState() != mOriginalPreinstalled)
    RequestReboot();
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

void GuiMenuGameSettings::SetRatio(const std::string& ratio)
{
  RecalboxConf::Instance().SetGlobalRatio(ratio).Save();
}

void GuiMenuGameSettings::SetQuitTwice(bool on)
{
  RecalboxConf::Instance().SetGlobalQuitTwice(on).Save();
}

void GuiMenuGameSettings::SetSmooth(bool on)
{
  RecalboxConf::Instance().SetGlobalSmooth(on).Save();
}

void GuiMenuGameSettings::SetRewind(bool on)
{
  RecalboxConf::Instance().SetGlobalRewind(on).Save();
}

void GuiMenuGameSettings::SetAutoSave(bool on)
{
  RecalboxConf::Instance().SetGlobalAutoSave(on).Save();
}

void GuiMenuGameSettings::SetShaders(const std::string& shaders)
{
  RecalboxConf::Instance().SetGlobalShaders(shaders).Save();
}

void GuiMenuGameSettings::SetShaderSet(const std::string& shaderset)
{
  RecalboxConf::Instance().SetGlobalShaderSet(shaderset).Save();
}

void GuiMenuGameSettings::SetIntegerScale(bool on)
{
  RecalboxConf::Instance().SetGlobalIntegerScale(on).Save();
}

void GuiMenuGameSettings::SetPreinstalled(bool on)
{
  RecalboxConf::Instance().SetGlobalHidePreinstalled(on).Save();
}

