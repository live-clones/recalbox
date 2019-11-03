//
// Created by xizor on 01/06/18.
//

#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <systems/SystemManager.h>
#include "GuiHashStart.h"
#include "components/OptionListComponent.h"


GuiHashStart::GuiHashStart(Window* window)
  : GuiComponent(window),
    mBusyAnim(window),
    mMenu(window, _("HASH NOW").c_str()),
    mHandle(nullptr)
{
    addChild(&mMenu);

    mLoading = false;

    mState = 0;

	mBusyAnim.setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

    mFilter = std::make_shared< OptionListComponent<std::string> >(mWindow, _("FILTER"), false);
    mFilter->add(_("Only missing hashs"), "missing", true);
    mFilter->add(_("All Games"), "all", false);
    mMenu.addWithLabel(mFilter, _("FILTER"));

    // add systems (all with a platformid specified selected)
    mSystems = std::make_shared< OptionListComponent<SystemData*> >(mWindow, _("HASH THESE SYSTEMS"), true);
    for (auto it : SystemManager::Instance().GetVisibleSystemList())
    {
        if(RecalboxConf::getInstance()->isInList("global.netplay.systems", it->getName()))
            mSystems->add(it->getFullName(), it, true);
    }
    mMenu.addWithLabel(mSystems, _("SYSTEMS"));

    mMenu.addButton(_("START"), "start",[this] {mState = 1;});
    mMenu.addButton(_("BACK"), "back", [&] { delete this; });

    mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2, (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiHashStart::start()
{
    for (auto system : mSystems->getSelectedObjects()) {

    	std::string command = "/recalbox/scripts/recalbox-hash.sh -s \"" + system->getName() + "\" -t";

	    auto cmdResult = RecalboxSystem::execute(command);

	    //script output 1 if system is in fobidden list (like fba_libretro) so we skip to next system
	    if (cmdResult.second == 1) {
		    LOG(LogInfo) << "system \"" << system->getName() << "\"  can't be hashed";
		    continue;
	    }

        Path xmlpath = system->getGamelistPath(false);

        if(!xmlpath.Exists()) return;

        LOG(LogInfo) << "Hashing games from XML file \"" << xmlpath.ToString() << "\"...";

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(xmlpath.ToChars());

        if(!result)
        {
            LOG(LogError) << "Error parsing XML file \"" << xmlpath.ToString() << "\"!\n	" << result.description();
            return;
        }

        pugi::xml_node root = doc.child("gameList");
        if(!root)
        {
            LOG(LogError) << "Could not find <gameList> node in gamelist \"" << xmlpath.ToString() << "\"!";
            return;
        }

        const Path& relativeTo = system->getStartPath();

        const char* tag = "game";

        int totalRoms = 0;

	    for (pugi::xml_node fileNode = root.child(tag); fileNode != nullptr; fileNode = fileNode.next_sibling(tag))
	    {
	    	totalRoms++;
	    }

	    int currentRom = 0;

	    std::string busyText;
	    const std::string& systemName = system->getFullName();

        for (pugi::xml_node fileNode = root.child(tag); fileNode != nullptr; fileNode = fileNode.next_sibling(tag))
        {
			currentRom++;

            Path path = Path(fileNode.child("path").text().get()).ToAbsolute(relativeTo);

            if(!path.Exists())
            {
                LOG(LogWarning) << "File \"" << path.ToString() << "\" does not exist! Ignoring.";
                continue;
            }

	        busyText = systemName + " " + std::to_string(currentRom) + " / " + std::to_string(totalRoms);

	        mBusyAnim.setText(busyText);

	        // if missing only, don't bother calculating hash if tag is present
	        if ((fileNode.child("hash") != nullptr) && mFilter->getSelected() != "all") {
	        	continue;
	        }

            std::string cmd = "/recalbox/scripts/recalbox-hash.sh -f \"" + path.ToString() + "\"";

	        auto hashResult = RecalboxSystem::execute(cmd);

	        std::string hashString = hashResult.first;

	        //script output an endline after hash, need to remove it
	        hashString.erase(std::remove(hashString.begin(), hashString.end(), '\n'), hashString.end());

	        //if tag exist, update if not, add it
            if ((fileNode.child("hash") != nullptr) && mFilter->getSelected() == "all") {
	            fileNode.child("hash").text().set(hashString.c_str());
            } else {
                pugi::xml_node hash = fileNode.append_child("hash");
                hash.append_child(pugi::node_pcdata).set_value(hashString.c_str());
            }
        }
        doc.save_file(xmlpath.ToChars());

    }
    mLoading = false;
    mState = -1;
}

bool GuiHashStart::ProcessInput(const InputCompactEvent& event)
{
	if (mLoading)
		return false;

	if (event.APressed())
	{
		delete this;
	}
	return GuiComponent::ProcessInput(event);
}

void GuiHashStart::update(int deltaTime) {
    GuiComponent::update(deltaTime);
    mBusyAnim.update(deltaTime);

	if (mState == 1) {
		mWindow->pushGui(
				new GuiMsgBox(mWindow, _("THIS COULD TAKE A WHILE, CONFIRM?"), _("YES"),
				              [this] {
					              this->mLoading = true;
					              mHandle = new boost::thread(boost::bind(&GuiHashStart::start, this));
					              mState = 0;

				              }, _("NO"), [this] {
							mState = -1;
						})

		);
		mState = 0;
	}
	if (mState == -1) {
		delete this;
	}
}

void GuiHashStart::render(const Transform4x4f& parentTrans)
{
    Transform4x4f trans = parentTrans * getTransform();

    if (!mLoading)
        renderChildren(trans);

    Renderer::setMatrix(trans);
    Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

    if (mLoading)
        mBusyAnim.render(trans);

}


std::vector<HelpPrompt> GuiHashStart::getHelpPrompts()
{
    std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();
    prompts.push_back(HelpPrompt("a", _("BACK")));
    return prompts;
}