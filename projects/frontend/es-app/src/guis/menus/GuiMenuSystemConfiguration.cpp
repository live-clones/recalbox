//
// Created by bkg2k on 04/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <guis/menus/GuiMenuSystemConfiguration.h>
#include <components/OptionListComponent.h>
#include <components/SwitchComponent.h>
#include <systems/SystemManager.h>
#include <guis/MenuMessages.h>
#include <LibretroRatio.h>
#include "GuiMenuTools.h"

GuiMenuSystemConfiguration::GuiMenuSystemConfiguration(WindowManager& window, SystemData& system, SystemManager& systemManager)
  : GuiMenuBase(window, system.getFullName())
  , mSystem(system)
{
  // Default emulator/core
  std::string currentEmulator(RecalboxConf::Instance().GetSystemEmulator(system));
  std::string currentCore    (RecalboxConf::Instance().GetSystemCore(system));
  GuiMenuTools::EmulatorAndCoreList list =
    GuiMenuTools::ListEmulatorAndCore(systemManager, system, mDefaultEmulator, mDefaultCore, currentEmulator, currentCore);
  if (!list.empty())
  {
    mEmulator = std::make_shared<OptionListComponent<std::string>>(window, _("Emulator"), false);
    mEmulator->setChangedCallback([this] { SetEmulatorCore(mEmulator->getSelected()); });
    for (const GuiMenuTools::EmulatorAndCore& emulator : list)
      mEmulator->add(emulator.Displayable, emulator.Identifier, emulator.Selected);
    mMenu.addWithLabel(mEmulator, _("Emulator"), _(MENUMESSAGE_ADVANCED_EMU_EMU_HELP_MSG));
  }

  // Screen ratio choice
  mRatio = std::make_shared<OptionListComponent<std::string> >(window, _("GAME RATIO"), false);
  mRatio->setChangedCallback([this] { SetRatio(mRatio->getSelected()); });
  std::string currentRatio = RecalboxConf::Instance().GetSystemRatio(system);
  for (const auto& ratio : LibretroRatio::GetRatio())
    mRatio->add(ratio.first, ratio.second, currentRatio == ratio.second);
  mMenu.addWithLabel(mRatio, _("GAME RATIO"), _(MENUMESSAGE_GAME_RATIO_HELP_MSG));

  // smoothing
  mSmooth = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetSystemSmooth(system));
  mSmooth->setChangedCallback([this](bool on) { SetSmooth(on); });
  mMenu.addWithLabel(mSmooth, _("SMOOTH GAMES"), _(MENUMESSAGE_GAME_SMOOTH_HELP_MSG));

  // rewind
  mRewind = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetSystemRewind(system));
  mRewind->setChangedCallback([this](bool on) { SetRewind(on); });
  mMenu.addWithLabel(mRewind, _("REWIND"), _(MENUMESSAGE_GAME_REWIND_HELP_MSG));

  // autosave
  mAutoSave = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().GetSystemAutoSave(system));
  mAutoSave->setChangedCallback([this](bool on) { SetAutoSave(on); });
  mMenu.addWithLabel(mAutoSave, _("AUTO SAVE/LOAD"), _(MENUMESSAGE_GAME_AUTOSAVELOAD_HELP_MSG));

  // Shaders
  mShaders = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SHADERS"), false);
  mShaders->setChangedCallback([this] { SetShaders(mShaders->getSelected()); });
  GuiMenuTools::ShaderList shaderList = GuiMenuTools::ListShaders();
  std::string currentShader = RecalboxConf::Instance().GetSystemShaders(system);
  mShaders->add(_("NONE"), "", currentShader.empty());
  for (const GuiMenuTools::Shader& shader : shaderList)
    mShaders->add(shader.Displayable, shader.ShaderPath.ToString(), currentShader == shader.ShaderPath.ToString());
  mMenu.addWithLabel(mShaders, _("SHADERS"), _(MENUMESSAGE_GAME_SHADERS_HELP_MSG));

  // Shaders preset
  mShaderSet = std::make_shared<OptionListComponent<std::string>>(mWindow,_("SHADERS SET"), false);
  mShaderSet->setChangedCallback([this] { SetShaderSet(mShaderSet->getSelected()); });
  currentShader = RecalboxConf::Instance().GetSystemShaderSet(system);
  if (currentShader.empty()) currentShader = "none";
  mShaderSet->add(_("NONE"), "none", currentShader == "none");
  mShaderSet->add(_("SCANLINES"), "scanlines", currentShader == "scanlines");
  mShaderSet->add(_("RETRO"), "retro", currentShader == "retro");
  mMenu.addWithLabel(mShaderSet, _("SHADERS SET"), _(MENUMESSAGE_GAME_SHADERSET_HELP_MSG));
}

void GuiMenuSystemConfiguration::SetEmulatorCore(const std::string& emulatorCore)
{
  // Split emulator & core
  std::string emulator, core;
  if (Strings::SplitAt(emulatorCore, ':', emulator, core, false))
  {
    if (emulator == mDefaultEmulator && core == mDefaultCore)
      RecalboxConf::Instance().SetSystemEmulator(mSystem, "")
                              .SetSystemCore(mSystem, "").Save();
    else
      RecalboxConf::Instance().SetSystemEmulator(mSystem, emulator)
                              .SetSystemCore(mSystem, core).Save();
  }
  else LOG(LogError) << "Error splitting emulator and core!";
}

void GuiMenuSystemConfiguration::SetRatio(const std::string& ratio)
{
  RecalboxConf::Instance().SetSystemRatio(mSystem, ratio).Save();
}

void GuiMenuSystemConfiguration::SetSmooth(bool on)
{
  RecalboxConf::Instance().SetSystemSmooth(mSystem, on).Save();
}

void GuiMenuSystemConfiguration::SetRewind(bool on)
{
  RecalboxConf::Instance().SetSystemRewind(mSystem, on).Save();
}

void GuiMenuSystemConfiguration::SetAutoSave(bool on)
{
  RecalboxConf::Instance().SetSystemAutoSave(mSystem, on).Save();
}

void GuiMenuSystemConfiguration::SetShaders(const std::string& shaders)
{
  RecalboxConf::Instance().SetSystemShaders(mSystem, shaders).Save();
}

void GuiMenuSystemConfiguration::SetShaderSet(const std::string& shaderset)
{
  RecalboxConf::Instance().SetSystemShaderSet(mSystem, shaderset).Save();
}
