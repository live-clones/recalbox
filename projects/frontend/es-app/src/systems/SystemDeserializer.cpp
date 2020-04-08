//
// Created by bkg2k on 03/04/2020.
//

#include "SystemDeserializer.h"

void SystemDeserializer::DeserializeEmulatorTree(XmlNode emulators, EmulatorList& emulatorList)
{
  emulatorList.Clear();
  for (const auto& emulator : emulators.children("emulator"))
  {
    const std::string& emulatorName = Xml::AttributeAsString(emulator, "name", "");
    EmulatorDescriptor emulatorDescriptor(emulatorName);
    XmlNode cores = emulator.child("cores");
    if (cores != nullptr)
      for (const auto& coreNode : cores.children("core"))
        emulatorDescriptor.AddCore(coreNode.child_value(), coreNode.attribute("priority").as_int(255));
    if (emulatorDescriptor.HasAny()) emulatorList.AddEmulator(emulatorDescriptor);
  }
}

bool SystemDeserializer::Deserialize(int index, SystemDescriptor& systemDescriptor)
{
  systemDescriptor.ClearEmulators();
  systemDescriptor.ClearPlatforms();

  XmlNode systemNode = mSystemList[index];
  // Information
  systemDescriptor.SetInformation(Xml::AsString(systemNode, "path", ""),
                                  Xml::AsString(systemNode, "name", ""),
                                  Xml::AsString(systemNode, "fullname", ""),
                                  Xml::AsString(systemNode, "command", ""),
                                  Xml::AsString(systemNode, "extension", ""),
                                  Xml::AsString(systemNode, "theme", ""));

  // Check
  if (systemDescriptor.IsValid())
  {
    // Emulator tree
    EmulatorList emulatorList;
    DeserializeEmulatorTree(systemNode.child("emulators"), emulatorList);
    systemDescriptor.SetEmulatorList(emulatorList);

    // Platform list
    std::vector<std::string> platforms = Strings::Split(Xml::AsString(systemNode, "platform", ""), ' ');
    for (const auto &platform : platforms)
    {
      PlatformIds::PlatformId platformId = PlatformIds::getPlatformId(platform);
      if (platformId == PlatformIds::PlatformId::PLATFORM_IGNORE)
      {
        systemDescriptor.ClearPlatforms();
        systemDescriptor.AddPlatformIdentifiers(platformId);
        break;
      }
      if (!systemDescriptor.AddPlatformIdentifiers(platformId))
        LOG(LogError) << "Platform count for system " << systemDescriptor.Name() << " full. " << platform << " ignored!";
    }
    return true;
  }

  LOG(LogError) << "System \"" << systemDescriptor.Name() << "\" is missing name, path, extension, or command!";
  return false;
}

bool SystemDeserializer::LoadSystemXMLNodes(const XmlDocument& document)
{
  bool result = false;
  XmlNode systemList = document.child("systemList");
  if (systemList != nullptr)
    for (const XmlNode system : systemList.children("system"))
    {
      // Ignore favorite
      if (Xml::AsString(system, "name", "") == sFavoriteSystemShortName)
        continue;

      // At least one node found
      result = true;

      // Composite key: fullname + platform
      std::string key = Xml::AsString(system, "fullname", "");
      key += Xml::AsString(system, "platform", "");

      // Already exist in the store ?
      int index = (mSystemMap.find(key) != mSystemMap.end()) ? mSystemMap[key] : -1;

      // If the system does not exists, add it to the end of list.
      // Replace the existing entry otherwise.
      // This way, we keep the original ordering of first-in entries
      if (index < 0)
      {
        mSystemMap[key] = (int) mSystemList.size();
        mSystemList.push_back(system);
      } else mSystemList[index] = system;
    }

  if (!result)
  {
    LOG(LogError) << "missing or empty <systemList> tag!";
  }

  return result;
}

bool SystemDeserializer::LoadXMLFile(XmlDocument& document, const Path& filepath)
{
  XmlResult result = document.load_file(filepath.ToChars());
  if (!result)
  {
    LOG(LogError) << "Could not parse " << filepath.ToString() << " file!";
    return false;
  }
  return true;
}

bool SystemDeserializer::LoadSystemList(XmlDocument &document, const Path &filepath)
{
  // Load user configuration
  if (!filepath.Exists())
  {
    LOG(LogWarning) << filepath.ToString() << " file does not exist!";
    return false;
  }

  LOG(LogInfo) << "Loading system config files " << filepath.ToString() << "...";
  if (!LoadXMLFile(document, filepath)) return false;

  bool result = LoadSystemXMLNodes(document);
  if (!result)
  {
    LOG(LogWarning) << filepath.ToString() << " has no systems or systemList nodes";
  }

  return result;
}

bool SystemDeserializer::LoadSystems()
{
  // Load user systems
  bool userValid = LoadSystemList(mUserDocument, UserConfigurationPath());
  // Load template systems
  bool templateValid = LoadSystemList(mTemplateDocument, TemplateConfigurationPath());

  return userValid || templateValid;
}
