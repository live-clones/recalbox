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

GuiMenuSystemConfiguration::GuiMenuSystemConfiguration(WindowManager& window, SystemData& system, SystemManager& systemManager, AdvancedMenuOptions options)
  : GuiMenuBase(window, system.FullName(), nullptr)
  , mSystemManager(systemManager)
  , mSystem(system)
{
  // Default emulator/core
  if(options.emulator)
    mEmulator = AddList(_("Emulator"), (int)Components::Emulator, this, GetEmulatorEntries(), _(MENUMESSAGE_ADVANCED_EMU_EMU_HELP_MSG));

  // Screen ratio choice
  if(options.ratio)
    mRatio = AddList(_("GAME RATIO"), (int)Components::Ratio, this, GetRatioEntries(), _(MENUMESSAGE_GAME_RATIO_HELP_MSG));

  // smoothing
  if(options.ratio)
    mSmooth = AddSwitch(_("SMOOTH GAMES"), RecalboxConf::Instance().GetSystemSmooth(system), (int)Components::Smooth, this, _(MENUMESSAGE_GAME_SMOOTH_HELP_MSG));

  // rewind
  if(options.rewind)
    mRewind = AddSwitch(_("REWIND"), RecalboxConf::Instance().GetSystemRewind(system), (int)Components::Rewind, this, _(MENUMESSAGE_GAME_REWIND_HELP_MSG));

  // autosave
  if(options.autosave)
    mAutoSave = AddSwitch(_("AUTO SAVE/LOAD"), RecalboxConf::Instance().GetSystemAutoSave(system), (int)Components::AutoSave, this, _(MENUMESSAGE_GAME_AUTOSAVELOAD_HELP_MSG));

  // Shaders
  if(options.shaders)
    mShaders = AddList(_("SHADERS"), (int)Components::Shaders, this, GetShadersEntries(), _(MENUMESSAGE_GAME_SHADERS_HELP_MSG));

  // Shaders preset
  if(options.shaderSet)
    mShaderSet = AddList(_("SHADERS SET"), (int)Components::ShaderSet, this, GetShaderSetEntries(), _(MENUMESSAGE_GAME_SHADERSET_HELP_MSG));
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystemConfiguration::GetEmulatorEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentEmulator(RecalboxConf::Instance().GetSystemEmulator(mSystem));
  std::string currentCore    (RecalboxConf::Instance().GetSystemCore(mSystem));
  GuiMenuTools::EmulatorAndCoreList eList =
    GuiMenuTools::ListEmulatorAndCore(mSystemManager, mSystem, mDefaultEmulator, mDefaultCore, currentEmulator, currentCore);
  if (!eList.empty())
    for (const GuiMenuTools::EmulatorAndCore& emulator : eList)
      list.push_back({ emulator.Displayable, emulator.Identifier, emulator.Selected });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystemConfiguration::GetRatioEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentRatio = RecalboxConf::Instance().GetSystemRatio(mSystem);
  for (const auto& ratio : LibretroRatio::GetRatio())
    list.push_back({ ratio.first, ratio.second, currentRatio == ratio.second });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystemConfiguration::GetShadersEntries()
{
  std::vector<ListEntry<std::string>> list;

  GuiMenuTools::ShaderList shaderList = GuiMenuTools::ListShaders();
  std::string currentShader = RecalboxConf::Instance().GetSystemShaders(mSystem);
  list.push_back({ _("NONE"), "", currentShader.empty() });
  for (const GuiMenuTools::Shader& shader : shaderList)
    list.push_back({ shader.Displayable, shader.ShaderPath.ToString(), currentShader == shader.ShaderPath.ToString() });

  return list;
}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMenuSystemConfiguration::GetShaderSetEntries()
{
  std::vector<ListEntry<std::string>> list;

  std::string currentShader = RecalboxConf::Instance().GetSystemShaderSet(mSystem);
  if (currentShader.empty() || !Strings::Contains("|none|scanlines|retro|", std::string(1, '|').append(currentShader).append(1, '|'))) currentShader = "none";
  list.push_back({ _("NONE"), "none", currentShader == "none" });
  list.push_back({ _("SCANLINES"), "scanlines", currentShader == "scanlines" });
  list.push_back({ _("RETRO"), "retro", currentShader == "retro" });

  return list;
}

void GuiMenuSystemConfiguration::OptionListComponentChanged(int id, int index, const std::string& value)
{
  (void)index;
  switch((Components)id)
  {
    case Components::Emulator:
    {
      // Split emulator & core
      std::string emulator, core;
      if (Strings::SplitAt(value, ':', emulator, core, false))
      {
        if (emulator == mDefaultEmulator && core == mDefaultCore)
          RecalboxConf::Instance().SetSystemEmulator(mSystem, "")
                                  .SetSystemCore(mSystem, "").Save();
        else
          RecalboxConf::Instance().SetSystemEmulator(mSystem, emulator)
                                  .SetSystemCore(mSystem, core).Save();
      }
      else { LOG(LogError) << "[SystemConfigurationGui] Error splitting emulator and core!"; }
      break;
    }
    case Components::Ratio: RecalboxConf::Instance().SetSystemRatio(mSystem, value).Save(); break;
    case Components::Shaders: RecalboxConf::Instance().SetSystemShaders(mSystem, value).Save(); break;
    case Components::ShaderSet: RecalboxConf::Instance().SetSystemShaderSet(mSystem, value).Save(); break;
    case Components::Smooth:
    case Components::Rewind:
    case Components::AutoSave: break;
  }
}

void GuiMenuSystemConfiguration::SwitchComponentChanged(int id, bool status)
{
  switch((Components)id)
  {
    case Components::Smooth: RecalboxConf::Instance().SetSystemSmooth(mSystem, status).Save(); break;
    case Components::Rewind: RecalboxConf::Instance().SetSystemRewind(mSystem, status).Save(); break;
    case Components::AutoSave: RecalboxConf::Instance().SetSystemAutoSave(mSystem, status).Save(); break;
    case Components::Emulator:
    case Components::Ratio:
    case Components::Shaders:
    case Components::ShaderSet: break;
  }
}
