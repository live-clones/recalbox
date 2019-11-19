//
// Created by xizor on 01/06/18.
//

#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/GuiMsgBox.h>
#include <systems/SystemManager.h>
#include "GuiHashStart.h"
#include "components/OptionListComponent.h"

GuiHashStart::GuiHashStart(Window& window, SystemManager& systemManager)
  : Gui(window),
    mSystemManager(systemManager),
    mBusyAnim(window),
    mMenu(window, _("HASH NOW").c_str()),
    mState(State::Wait)
{
  addChild(&mMenu);

  mBusyAnim.setSize(Renderer::getDisplayWidthAsFloat(), Renderer::getDisplayHeightAsFloat());

  mFilter = std::make_shared<OptionListComponent<std::string> >(mWindow, _("FILTER"), false);
  mFilter->add(_("Only missing hashs"), "missing", true);
  mFilter->add(_("All Games"), "all", false);
  mMenu.addWithLabel(mFilter, _("FILTER"));

  // add systems (all with a platformid specified selected)
  mSystems = std::make_shared<OptionListComponent<SystemData*> >(mWindow, _("HASH THESE SYSTEMS"), true);
  for (auto it : mSystemManager.GetVisibleSystemList())
  {
    if (RecalboxConf::Instance().isInList("global.netplay.systems", it->getName()))
      mSystems->add(it->getFullName(), it, true);
  }
  mMenu.addWithLabel(mSystems, _("SYSTEMS"));

  mMenu.addButton(_("START"), "start", [this]
  {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("THIS COULD TAKE A WHILE, CONFIRM?"), _("YES"), [this]
    {
      Start("HashThread");
      mState = State::Hashing;
    }, _("NO"), [this] { mState = State::Exit; }));
  });
  mMenu.addButton(_("BACK"), "back", [&]
  { Close(); });

  mMenu.setPosition((Renderer::getDisplayWidthAsFloat() - mMenu.getSize().x()) / 2,
                    (Renderer::getDisplayHeightAsFloat() - mMenu.getSize().y()) / 2);
}

void GuiHashStart::Run()
{
  bool forceAll = mFilter->getSelected() == "all";

  // Run through systems...
  for (auto system : mSystems->getSelectedObjects())
  {
    // Check if the system should be hashed
    auto cmdResult = RecalboxSystem::execute("/recalbox/scripts/recalbox-hash.sh -s \"" + system->getName() + "\" -t");
    if (cmdResult.second == 1)
    {
      LOG(LogInfo) << "system \"" << system->getName() << "\"  can't be hashed";
      continue;
    }
    LOG(LogInfo) << "Hashing games from system \"" << system->getName() << "\"...";

    // Run through games
    int romCount = 0;
    FileData::List games = system->getRootFolder()->getAllItems(true);
    for (FileData* game : games)
    {
      // Check path
      const Path& path = Path(game->getPath());
      if (!path.Exists())
      {
        LOG(LogWarning) << "File \"" << path.ToString() << "\" does not exist! Ignoring.";
        continue;
      }

      // Update GUI text
      std::string busyText = system->getFullName() + " " + std::to_string(++romCount) + " / " + std::to_string(games.size());
      mMutex.Lock();
      mBusyAnim.setText(busyText);
      mMutex.UnLock();

      // Hash already exists?
      if (game->Metadata().RomCrc32() == 0 && !forceAll)
        continue;

      // Get hash
      auto hashResult = RecalboxSystem::execute("/recalbox/scripts/recalbox-hash.sh -f \"" + path.ToString() + "\"");
      game->Metadata().SetRomCrc32AsString(hashResult.first);
    }
    system->UpdateGamelistXml();
  }
  mState = State::Exit;
}

bool GuiHashStart::ProcessInput(const InputCompactEvent& event)
{
  if (mState == State::Hashing)
    return false;

  if (event.APressed())
    Close();

  return GuiComponent::ProcessInput(event);
}

void GuiHashStart::update(int deltaTime)
{
  GuiComponent::update(deltaTime);
  mMutex.Lock();
  mBusyAnim.update(deltaTime);
  mMutex.UnLock();

  if (mState == State::Exit)
    Close();
}

void GuiHashStart::render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = parentTrans * getTransform();

  if (mState != State::Hashing)
    renderChildren(trans);

  Renderer::setMatrix(trans);
  Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);

  if (mState == State::Hashing)
    mBusyAnim.render(trans);
}


std::vector<HelpPrompt> GuiHashStart::getHelpPrompts()
{
  std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();
  prompts.push_back(HelpPrompt("a", _("BACK")));
  return prompts;
}