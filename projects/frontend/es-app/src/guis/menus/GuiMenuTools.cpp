//
// Created by bkg2k on 06/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <systems/SystemManager.h>
#include <guis/menus/GuiMenuTools.h>
#include <utils/locale/LocaleHelper.h>
#include <algorithm>

const Path GuiMenuTools::sShadersPath("/recalbox/share/shaders");

GuiMenuTools::EmulatorAndCoreList
GuiMenuTools::ListEmulatorAndCore(SystemManager& systemManager, SystemData& system, std::string& outDefaultEmulator,
                                  std::string& outDefaultCore, const std::string& currentEmulator,
                                  const std::string& currentCore)
{
  EmulatorAndCoreList result;
  std::string emulator = currentEmulator;
  std::string core = currentCore;

  if (systemManager.Emulators().GetDefaultEmulator(system, outDefaultEmulator, outDefaultCore))
  {
    if (emulator.empty()) emulator = outDefaultEmulator;
    if (core.empty()) core = outDefaultCore;
    for (const std::string& emulatorName : systemManager.Emulators().GetEmulators(system))
      for (const std::string& coreName : systemManager.Emulators().GetCores(system, emulatorName))
      {
        // Get display name, composed of "emulator core" or just "emulator" of both are the same (standalone)
        // Add "(default)" if this is the default emulator/core
        std::string displayName(emulatorName);
        if (displayName != coreName) displayName.append(1, ' ').append(coreName);
        if (outDefaultCore == coreName && outDefaultEmulator == emulatorName)
          displayName.append(" (").append(_("DEFAULT")).append(1, ')');

        // Build a key "emulator:core"
        std::string emulatorAndCore(emulatorName);
        emulatorAndCore.append(1, ':').append(coreName);
        bool match = emulatorName == emulator && coreName == core;
        if (match) { LOG(LogDebug) << "[GUI] Selected emulator/core: " << emulatorAndCore; }

        // Add the entry
        result.push_back({ emulatorAndCore, displayName, match });
      }
  }
  else { LOG(LogError) << "[GUI] Can't get default emulator/core for " << system.FullName(); }

  return result;
}


void GuiMenuTools::ReadShaderFolder(const Path& root, Path::PathList& glslp)
{
  Path::PathList files = root.GetDirectoryContent();
  for(const Path& path : files)
    if (path.Extension() == ".glslp")
      glslp.push_back(path);
    else if (path.IsDirectory())
      ReadShaderFolder(path, glslp);
}

Path::PathList GuiMenuTools::GetShaderList()
{
  Path::PathList glslp;
  ReadShaderFolder(sShadersPath, glslp);
  std::sort(glslp.begin(), glslp.end());
  return glslp;
}

GuiMenuTools::ShaderList GuiMenuTools::ListShaders()
{
  ShaderList result;

  for (const Path& path : GetShaderList())
  {
    bool ok = false;
    std::string shaderName = path.MakeRelative(sShadersPath, ok).ToString();
    Strings::ReplaceAllIn(shaderName, '/', " - ", 3);
    Strings::ReplaceAllIn(shaderName, '_', " ", 1);
    result.push_back({ path, shaderName });
  }

  return result;
}
