//
// Created by pre2010-02 on 08/02/2022.
//

#include "GuiMemuSmartCollection.h"
#include "systems/SystemManager.h"
#include "utils/Files.h"

GuiMemuSmartCollection::GuiMemuSmartCollection(WindowManager& window, SystemManager& systemManager, SmartCollection* smartCollection)
  : GuiMenuBase(window, _("SMART COLLECTION"), nullptr),
    mSystemManager(systemManager),
    mSmartCollection(smartCollection)


{
  AddEditable(_("COLLECTION NAME"), mSmartCollection == nullptr ? "" : mSmartCollection->FullName(), (int)Components::fullName, this, false);
  AddEditable(_("FILTER NAME"), mSmartCollection == nullptr ? "" : mSmartCollection->NameFilter(), (int)Components::nameFilter, this, false);

  AddMultiList(_("SYSTEMS"), (int)Components::platformFilter, this, GetSystemEntries());

  // Buttons
  if(smartCollection == nullptr)
    mMenu.addButton(_("CREATE"), "create ", [this] {Create();});
  else
  {
    mMenu.addButton(_("UPDATE"), "update ", [this] {});
    mMenu.addButton(_("DELETE"), "update ", [this] {});
  }


}

std::vector<GuiMenuBase::ListEntry<std::string>> GuiMemuSmartCollection::GetSystemEntries()
{
  std::vector<ListEntry<std::string>> list;

  for (auto* it : mSystemManager.GetAllSystemList())
    if (!it->IsVirtual() &&!it->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
      list.push_back({it->FullName(), it->Name(),
                      mSmartCollection != nullptr &&
                      Strings::Contains(mSmartCollection->PlatformFilter(), it->Name())
                     });

  return list;
}

void GuiMemuSmartCollection::Create()
{

  XmlDocument document;
  XmlNode root = document.append_child("collections");

  XmlNode collection = root.append_child("collection");
  collection.append_attribute("identifier") = mFullName.c_str();
  collection.append_attribute("fullname") = mFullName.c_str();

  XmlNode filters = collection.append_child("filters");

  Xml::AddAsString(filters, "nameFilter", mName.c_str());
  Xml::AddAsString(filters, "platformFilter", mPlatform.c_str());
  Close();
  RequestRelaunch();

  struct XmlWriter : public pugi::xml_writer
  {
    std::string mOutput;
    void write(const void* data, size_t size) override { mOutput.append((const char*)data, size); }
  }
    Writer;

  /*
   * Write the list.
   * At this point, we're sure at least one node has been updated (or added and updated).
   */
  Path path = Path("/recalbox/share/roms/collections/collections.xml");
  path.Directory().CreatePath();
  document.save(Writer);
  Files::SaveFile(path, Writer.mOutput);

}


void GuiMemuSmartCollection::EditableComponentTextChanged(int id, const std::string& text)
{
  if ((Components)id == Components::nameFilter)
    mName = text;
  if ((Components)id == Components::fullName)
    mFullName = text;
}

void GuiMemuSmartCollection::OptionListMultiComponentChanged(int id, const std::vector<std::string>& value)
{

  if ((Components)id == Components::platformFilter)
  {
    std::string platforms;

    for (auto& platform: value)
    {
      platforms.append(platform);

      if(value.back() != platform)
        platforms.append(",");
    }
    mPlatform = platforms;
  }
}

GuiMemuSmartCollection::~GuiMemuSmartCollection()
{
 // RequestRelaunch();
}

