//
// Created by bkg2k on 03/04/2020.
//

#include "SystemDeserializer.h"

void SystemDeserializer::DeserializeEmulatorTree(XmlNode emulators, EmulatorList& emulatorList)
{
  emulatorList.Clear();
  for (const XmlNode& emulator : emulators.children("emulator"))
  {
    const std::string& emulatorName = Xml::AttributeAsString(emulator, "name", "");
    EmulatorDescriptor emulatorDescriptor(emulatorName);
    for (const auto& coreNode : emulator.children("core"))
      emulatorDescriptor.AddCore(Xml::AttributeAsString(coreNode, "name", ""),
                                 Xml::AttributeAsInt(coreNode, "priority", 255),
                                 Xml::AttributeAsString(coreNode, "extensions", ""),
                                 Xml::AttributeAsBool(coreNode, "netplay", false),
                                 Xml::AttributeAsString(coreNode, "compatibility", ""),
                                 Xml::AttributeAsString(coreNode, "speed", ""));
    if (emulatorDescriptor.HasAny()) emulatorList.AddEmulator(emulatorDescriptor);
  }
}

bool SystemDeserializer::Deserialize(int index, SystemDescriptor& systemDescriptor)
{
  systemDescriptor.ClearEmulators();
  systemDescriptor.ClearPlatforms();

  XmlNode systemNode = mSystemList[index];
  // System Information
  systemDescriptor.SetSystemInformation(Xml::AttributeAsString(systemNode, "uuid", ""),
                                        Xml::AttributeAsString(systemNode, "name", ""),
                                        Xml::AttributeAsString(systemNode, "fullname", ""),
                                        Xml::AttributeAsString(systemNode, "platforms", ""));
  // System descriptor
  XmlNode descriptor = systemNode.child("descriptor");
  systemDescriptor.SetDescriptorInformation(Xml::AttributeAsString(descriptor, "path", ""),
                                            Xml::AttributeAsString(descriptor, "extensions", ""),
                                            Xml::AttributeAsString(descriptor, "theme", ""),
                                            Xml::AttributeAsString(descriptor, "command", ""),
                                            Xml::AttributeAsString(descriptor, "icon", "$0"),
                                            Xml::AttributeAsBool(descriptor, "port", false),
                                            Xml::AttributeAsBool(descriptor, "readonly", false));
  // Scraper information
  XmlNode scraper = systemNode.child("scraper");
  systemDescriptor.SetScraperInformation(Xml::AttributeAsInt(scraper, "screenscraper", 0));

  // System properties
  XmlNode properties = systemNode.child("properties");
  systemDescriptor.SetPropertiesInformation(Xml::AttributeAsString(properties, "type", ""),
                                            Xml::AttributeAsString(properties, "pad", ""),
                                            Xml::AttributeAsString(properties, "keyboard", ""),
                                            Xml::AttributeAsString(properties, "mouse", ""),
                                            Xml::AttributeAsString(properties, "releasedate", ""),
                                            Xml::AttributeAsBool(properties, "lightgun", false),
                                            Xml::AttributeAsBool(properties, "crt.multiresolution", false),
                                            Xml::AttributeAsBool(properties, "crt.multiregion", false));

  // Check
  if (systemDescriptor.IsValid())
  {
    // Emulator tree
    EmulatorList emulatorList;
    DeserializeEmulatorTree(systemNode.child("emulatorList"), emulatorList);
    systemDescriptor.SetEmulatorList(emulatorList);
    return true;
  }

  { LOG(LogError) << "[System] System \"" << systemDescriptor.Name() << "\" is missing name, path, extension, or GUID!"; }
  return false;
}

bool SystemDeserializer::LoadSystemXMLNodes(const XmlDocument& document)
{
  bool result = false;
  XmlNode systemList = document.child("systemList");
  if (systemList != nullptr)
  {
    // Defaults
    XmlNode defaults = systemList.child("defaults");
    SystemDescriptor::SetDefaultCommand(Xml::AttributeAsString(defaults, "command", ""));

    // Systems
    for (const XmlNode system : systemList.children("system"))
    {
      // Ignore favorite
      if (Xml::AttributeAsString(system, "name", "") == sFavoriteSystemShortName)
        continue;

      // At least one node found
      result = true;

      // Composite key: fullname + platform
      std::string key = Xml::AttributeAsString(system, "uuid", "");

      // Already exist in the store ?
      int index = (mSystemMap.find(key) != mSystemMap.end()) ? mSystemMap[key] : -1;

      // If the system does not exists, add it to the end of list.
      // Replace the existing entry otherwise.
      // This way, we keep the original ordering of first-in entries
      if (index < 0)
      {
        mSystemMap[key] = (int) mSystemList.size();
        mSystemList.push_back(system);
      }
      else mSystemList[index] = system;
    }
  }

  if (!result) { LOG(LogError) << "[System] missing or empty <systemList> tag!"; }

  return result;
}

bool SystemDeserializer::LoadXMLFile(XmlDocument& document, const Path& filepath)
{
  XmlResult result = document.load_file(filepath.ToChars());
  if (!result)
  {
    { LOG(LogError) << "[System] Could not parse " << filepath.ToString() << " file!"; }
    return false;
  }
  return true;
}

bool SystemDeserializer::LoadSystemList(XmlDocument &document, const Path &filepath)
{
  // Load user configuration
  if (!filepath.Exists())
  {
    { LOG(LogWarning) << "[System] " << filepath.ToString() << " file does not exist!"; }
    return false;
  }

  { LOG(LogInfo) << "[System] Loading system config files " << filepath.ToString() << "..."; }
  if (!LoadXMLFile(document, filepath)) return false;

  bool result = LoadSystemXMLNodes(document);
  if (!result) { LOG(LogWarning) << "[System] " << filepath.ToString() << " has no systems or systemList nodes"; }

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
