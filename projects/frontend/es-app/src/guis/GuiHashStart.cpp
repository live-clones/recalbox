//
// Created by xizor on 01/06/18.
//

#include <RecalboxConf.h>
#include <boost/filesystem/path.hpp>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include "GuiHashStart.h"
#include "components/OptionListComponent.h"


GuiHashStart::GuiHashStart(Window* window)
  : GuiComponent(window),
    mBusyAnim(window),
    mMenu(window, _("HASH NOW").c_str())
{
    addChild(&mMenu);

    mLoading = false;

    mState = 0;

	mBusyAnim.setSize((float) Renderer::getScreenWidth(), (float) Renderer::getScreenHeight());

    mFilter = std::make_shared< OptionListComponent<std::string> >(mWindow, _("FILTER"), false);
    mFilter->add(_("Only missing hashs"), "missing", true);
    mFilter->add(_("All Games"), "all", false);
    mMenu.addWithLabel(mFilter, _("FILTER"));

    // add systems (all with a platformid specified selected)
    mSystems = std::make_shared< OptionListComponent<SystemData*> >(mWindow, _("HASH THESE SYSTEMS"), true);
    for (auto& it : SystemData::sSystemVector)
    {
        if(RecalboxConf::getInstance()->isInList("global.netplay.systems", it->getName()))
            mSystems->add(it->getFullName(), it, true);
    }
    mMenu.addWithLabel(mSystems, _("SYSTEMS"));

    mMenu.addButton(_("START"), "start",[this] {mState = 1;});
    mMenu.addButton(_("BACK"), "back", [&] { delete this; });

    mMenu.setPosition(((float)Renderer::getScreenWidth() - mMenu.getSize().x()) / 2, ((float)Renderer::getScreenHeight() - mMenu.getSize().y()) / 2);
}

void GuiHashStart::start()
{
    for (auto system : mSystems->getSelectedObjects()) {

    	std::string command = "/recalbox/scripts/recalbox-hash.sh -s \"" + system->getName() + "\" -t";

	    auto cmdResult = RecalboxSystem::getInstance()->execute(command);

	    //script output 1 if system is in fobidden list (like fba_libretro) so we skip to next system
	    if (cmdResult.second == 1) {
		    LOG(LogInfo) << "system \"" << system->getName() << "\"  can't be hashed";
		    continue;
	    }

        std::string xmlpath = system->getGamelistPath(false);

        if(!boost::filesystem::exists(xmlpath))
            return;

        LOG(LogInfo) << "Hashing games from XML file \"" << xmlpath << "\"...";

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(xmlpath.c_str());

        if(!result)
        {
            LOG(LogError) << "Error parsing XML file \"" << xmlpath << "\"!\n	" << result.description();
            return;
        }

        pugi::xml_node root = doc.child("gameList");
        if(!root)
        {
            LOG(LogError) << "Could not find <gameList> node in gamelist \"" << xmlpath << "\"!";
            return;
        }

        boost::filesystem::path relativeTo = system->getStartPath();


        const char* tag = "game";

        int totalRoms = 0;

	    for (pugi::xml_node fileNode = root.child(tag); fileNode; fileNode = fileNode.next_sibling(tag))
	    {
	    	totalRoms++;
	    }

	    int currentRom = 0;

	    std::string busyText;
	    const std::string& systemName = system->getFullName();

        for (pugi::xml_node fileNode = root.child(tag); fileNode; fileNode = fileNode.next_sibling(tag))
        {
			currentRom++;

            boost::filesystem::path path = resolvePath(fileNode.child("path").text().get(), relativeTo, false);

            if(!boost::filesystem::exists(path))
            {
                LOG(LogWarning) << "File \"" << path << "\" does not exist! Ignoring.";
                continue;
            }

	        busyText = systemName + " " + std::to_string(currentRom) + " / " + std::to_string(totalRoms);

	        mBusyAnim.setText(busyText);

	        // if missing only, don't bother calculating hash if tag is present
	        if (fileNode.child("hash") && mFilter->getSelected() != "all") {
	        	continue;
	        }

            std::string cmd = "/recalbox/scripts/recalbox-hash.sh -f \"" + path.string() + "\"";

	        auto hashResult = RecalboxSystem::getInstance()->execute(cmd);

	        std::string hashString = hashResult.first;

	        //script output an endline after hash, need to remove it
	        hashString.erase(std::remove(hashString.begin(), hashString.end(), '\n'), hashString.end());

	        //if tag exist, update if not, add it
            if (fileNode.child("hash") && mFilter->getSelected() == "all") {
	            fileNode.child("hash").text().set(hashString.c_str());
            } else {
                pugi::xml_node hash = fileNode.append_child("hash");
                hash.append_child(pugi::node_pcdata).set_value(hashString.c_str());
            }
        }
        doc.save_file(xmlpath.c_str());

    }
    mLoading = false;
    mState = -1;
}

bool GuiHashStart::input(InputConfig* config, Input input)
{
	if (mLoading)
		return false;

	if (config->isMappedTo("a", input) && input.value != 0)
	{
		delete this;
	}
	return GuiComponent::input(config, input);
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