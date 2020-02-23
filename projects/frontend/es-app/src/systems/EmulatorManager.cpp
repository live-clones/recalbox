//
// Created by Bkg2k on 18/02/2020.
//

#include <utils/locale/LocaleHelper.h>
#include "EmulatorManager.h"

bool EmulatorManager::GetGameEmulator(const FileData& game, std::string& emulator, std::string& core) const
{
  // Get default emulator first
  bool Ok = GetSystemDefaultEmulator(*game.getSystem(), emulator, core);

  // Then from the general config file
  GetEmulatorFromConfigFile(*game.getSystem(), emulator, core);

  // Then from the gamelist.cfg file
  GetEmulatorFromGamelist(game, emulator, core);

  // Then from file overrides
  GetemulatorFromOverride(game, emulator, core);

  return Ok;
}

EmulatorData EmulatorManager::GetGameEmulator(const FileData& game) const
{
  std::string emulator;
  std::string core;

  if (GetGameEmulator(game, emulator, core))
    return EmulatorData(emulator, core);

  return EmulatorData("", "");
}

bool EmulatorManager::GetSystemDefaultEmulator(const SystemData& system, std::string& emulator,
                                                     std::string& core) const
{
  emulator.clear();
  core.clear();
  const EmulatorList** tryList = mSystemEmulators.try_get(KeyFrom(system));
  if (tryList != nullptr)
  {
    const EmulatorList& list = **tryList;
    if (list.HasAny())
    {
      int priority = 255;
      // Search for lowest priority core/emulator
      for(int i = list.Count(); --i >= 0; )
        for(int j = list.EmulatorAt(i).CoreCount(); --j >= 0; )
          if (list.EmulatorAt(i).CorePriorityAt(j) < priority)
          {
            priority = list.EmulatorAt(i).CorePriorityAt(j);
            core = list.EmulatorAt(i).CoreAt(j);
            emulator = list.EmulatorAt(i).Name();
          }

      LOG(LogDebug) << "Default emulator/core" << emulator << '/' << core;
      return true;
    }
  }
  return false;
}

void EmulatorManager::GetEmulatorFromConfigFile(const SystemData& system, std::string& emulator, std::string& core) const
{
  std::string rawemulator = RecalboxConf::Instance().AsString(system.getName() + ".emulator");
  std::string rawcore = RecalboxConf::Instance().AsString(system.getName() + ".core");

  // At least one not empty?
  if (!emulator.empty() || !core.empty())
  {
    if (CheckEmulatorAndCore(system, rawemulator, rawcore))
    {
      emulator = rawemulator;
      core = rawcore;
      LOG(LogDebug) << "Emulator/core from configuration file" << emulator << '/' << core;
    }
    else
    {
      if (GuessEmulatorAndCore(system, rawemulator, rawcore))
      {
        emulator = rawemulator;
        core = rawcore;
        LOG(LogDebug) << "Emulator/core guessed from configuration file" << emulator << '/' << core;
      }
    }
  }
}

void EmulatorManager::GetEmulatorFromGamelist(const FileData& game, std::string& emulator, std::string& core) const
{
  // Get configured emulator/core iif they are both not empty
  if (!game.Metadata().Core().empty() && !game.Metadata().Emulator().empty())
  {
    std::string rawemulator = game.Metadata().Emulator();
    std::string rawcore = game.Metadata().Core();

    if (CheckEmulatorAndCore(*game.getSystem(), rawemulator, rawcore))
    {
      emulator = rawemulator;
      core = rawcore;
      LOG(LogDebug) << "Emulator/core from Gamelist.xml" << emulator << '/' << core;
    }
    else
    {
      if (GuessEmulatorAndCore(*game.getSystem(), rawemulator, rawcore))
      {
        emulator = rawemulator;
        core = rawcore;
        LOG(LogDebug) << "Emulator/core guessed from Gamelist.xml" << emulator << '/' << core;
      }
    }
  }
}

void EmulatorManager::GetemulatorFromOverride(const FileData& game, std::string& emulator, std::string& core) const
{
  std::string rawGlobalEmulator;
  std::string rawSystemEmulator;
  std::string rawGlobalCore;
  std::string rawSystemCore;

  std::string keyEmulator = game.getSystem()->getName() + ".emulator";
  std::string keyCore = game.getSystem()->getName() + ".core";

  // Get game directory
  Path path = game.getPath().Directory();
  // Run through all folder starting from root
  int count = path.ItemCount();
  for (int i = 0; i < count; ++i)
  {
    IniFile configuration(Path(path.UptoItem(i)) / ".recalbox.conf");
    if (configuration.IsValid())
    {
      // Get values
      std::string globalEmulator = configuration.AsString("global.emulator");
      std::string systemEmulator = configuration.AsString(keyEmulator);
      std::string globalCore = configuration.AsString("global.core");
      std::string systemCore = configuration.AsString(keyCore);

      // Record non empty values
      if (globalEmulator.empty()) rawGlobalEmulator = globalEmulator;
      if (globalCore.empty()    ) rawGlobalCore     = globalCore;
      if (systemEmulator.empty()) rawSystemEmulator = systemEmulator;
      if (systemCore.empty()    ) rawSystemCore     = systemCore;
    }
  }

  // Get final tupple
  std::string finalEmulator = rawSystemEmulator.empty() ? rawGlobalEmulator : rawSystemEmulator;
  std::string finalCore     = rawSystemCore.empty()     ? rawGlobalCore     : rawSystemCore;

  if (!finalEmulator.empty() || !finalCore.empty())
  {
    if (CheckEmulatorAndCore(*game.getSystem(), finalEmulator, finalCore))
    {
      emulator = finalEmulator;
      core = finalCore;
      LOG(LogDebug) << "Emulator/core from override files" << emulator << '/' << core;
    }
    else
    {
      if (GuessEmulatorAndCore(*game.getSystem(), finalEmulator, finalCore))
      {
        emulator = finalEmulator;
        core = finalCore;
        LOG(LogDebug) << "Emulator/core guessed from override files" << emulator << '/' << core;
      }
    }
  }
}

bool EmulatorManager::CheckEmulatorAndCore(const SystemData& system, const std::string& emulator, const std::string& core) const
{
  const EmulatorList** tryList = mSystemEmulators.try_get(KeyFrom(system));
  if (tryList != nullptr)
  {
    const EmulatorList& list = **tryList;
    if (list.HasNamed(emulator))
      if (list.Named(emulator).HasCore(core))
        return true;
  }
  return false;
}

bool EmulatorManager::GuessEmulatorAndCore(const SystemData& system, std::string& emulator, std::string& core) const
{
  const EmulatorList** tryList = mSystemEmulators.try_get(KeyFrom(system));
  if (tryList != nullptr)
  {
    const EmulatorList& list = **tryList;
    // Emulator without core
    if (!emulator.empty() && core.empty())
    {
      if (list.HasNamed(emulator))
        if (list.Named(emulator).CoreCount() == 1)
        {
          core = list.Named(emulator).CoreAt(0);
          LOG(LogDebug) << "Core " << core << " guessed from emulator " << emulator << " whish has only one core";
          return true;
        }
    }
    // Core w/o emulator
    if (emulator.empty() && !core.empty())
    {
      for(int i = list.Count(); --i >= 0; )
        if (list.EmulatorAt(i).HasCore(core))
        {
          emulator = list.EmulatorAt(i).Name();
          LOG(LogDebug) << "Emulator " << emulator << " guessed from core " << core;
          return true;
        }
    }
  }
  return false;
}

Strings::Vector EmulatorManager::GetEmulators(const SystemData& system) const
{

  const EmulatorList** tryList = mSystemEmulators.try_get(KeyFrom(system));
  if (tryList != nullptr)
  {
    const EmulatorList& list = **tryList;

    // Get priorities
    unsigned char emulatorPriorities[EmulatorList::sMaximumEmulators];
    for(int i = EmulatorList::sMaximumEmulators; --i >= 0; ) emulatorPriorities[i] = 255;
    for(int i = list.Count(); --i >= 0; )
      for(int j = list.EmulatorAt(i).CoreCount(); --j >= 0; )
        if (list.EmulatorAt(i).CorePriorityAt(j) < emulatorPriorities[i])
          emulatorPriorities[i] = list.EmulatorAt(i).CorePriorityAt(j);

    // Build a sorted output list
    Strings::Vector result;
    for(int round = list.Count(); --round >= 0; )
    {
      int lowestPriority = emulatorPriorities[0];
      int index = 0;
      for(int i = 0; i < list.Count(); ++i)
        if (emulatorPriorities[i] < lowestPriority)
        {
          lowestPriority = emulatorPriorities[i];
          index = i;
        }
      result.push_back(list.EmulatorAt(index).Name());
      emulatorPriorities[index] = 255;
    }

    return result;
  }

  return Strings::Vector();
}

Strings::Vector EmulatorManager::GetCores(const SystemData& system, const std::string& emulator) const
{
  const EmulatorList** tryList = mSystemEmulators.try_get(KeyFrom(system));
  if (tryList != nullptr)
  {
    const EmulatorList& list = **tryList;
    if (list.HasNamed(emulator))
    {
      const EmulatorDescriptor& descriptor = list.Named(emulator);
      // Get priorities
      unsigned char corePriorities[EmulatorDescriptor::sMaximumCores];
      for(int i = EmulatorDescriptor::sMaximumCores; --i >= 0; ) corePriorities[i] = 255;
      for(int i = descriptor.CoreCount(); --i >= 0; ) corePriorities[i] = descriptor.CorePriorityAt(i);

      // Build a sorted output list
      Strings::Vector result;
      for(int round = descriptor.CoreCount(); --round >= 0; )
      {
        int lowestPriority = corePriorities[0];
        int index = 0;
        for(int i = 0; i < descriptor.CoreCount(); ++i)
          if (corePriorities[i] < lowestPriority)
          {
            lowestPriority = corePriorities[i];
            index = i;
          }
        result.push_back(descriptor.CoreAt(index));
        corePriorities[index] = 255;
      }

      return result;
    }
  }

  return Strings::Vector();
}

std::string EmulatorManager::KeyFrom(const SystemData& system)
{
  std::string result(system.getFullName());
  for(int i = system.PlatformCount(); --i >= 0; )
    result.append(Strings::ToString((int)system.PlatformIds(i)));
  return result;
}

