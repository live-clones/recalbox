//
// Created by gugue_u on 03/01/2021.
//

#include <guis/menus/GuiMenuCollections.h>
#include <guis/menus/GuiMemuSmartCollection.h>
#include <utils/locale/LocaleHelper.h>
#include <scraping/ScraperFactory.h>
#include <scraping/ScraperTools.h>
#include <guis/GuiScraperRun.h>
#include "GuiMenuNetwork.h"

GuiMenuCollections::GuiMenuCollections(WindowManager& window, SystemManager& systemManager)
  : GuiMenuBase(window, _("COLLECTIONS"), this),
  mSystemManager(systemManager)
{

  for (Path& file : Path(SmartCollection::sXmlPath).GetDirectoryContent())
  {
    XmlDocument document;
    std::string xlsPath = file.ToString();
    XmlResult result = document.load_file(xlsPath.c_str());
    if (!result)
    {
      { LOG(LogError) << "[SearchCollection] Could not parse " << xlsPath << " file!"; }
    }

    XmlNode collections = document.child("collections");

    if (collections != nullptr)
    {
      int i = 0;
      for (XmlNode& collectionNode : collections.children("collection"))
      {
        SmartCollection collection(collectionNode);
        AddSubMenu(collection.FullName(), i);
        existingCollections.insert(existingCollections.begin(), collection);
        i++;
      }
    }
  }

  mMenu.addButton(_("NEW COLLECTION"), "start", [this] {
    mWindow.pushGui(new GuiMemuSmartCollection(mWindow, mSystemManager, nullptr));
  });
}

void GuiMenuCollections::SubMenuSelected(int id)
{
  int i = 0;
  for(SmartCollection collection : existingCollections)
  {
    if(id == i)
      mWindow.pushGui(new GuiMemuSmartCollection(mWindow, mSystemManager, &collection));
    i++;
  }

}



