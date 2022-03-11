//
// Created by xizor on 05/10/2019.
//

#include <components/ButtonComponent.h>
#include <components/MenuComponent.h>
#include <guis/GuiTextEditPopupKeyboard.h>
#include <guis/GuiArcadeVirtualKeyboard.h>
#include <views/ViewController.h>
#include <RecalboxConf.h>
#include "components/ScrollableContainer.h"
#include "GuiSearch.h"
#include "GuiNetPlayHostPasswords.h"
#include <VideoEngine.h>


#define BUTTON_GRID_VERT_PADDING Renderer::Instance().DisplayHeightAsFloat() * 0.025f
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + Renderer::Instance().DisplayHeightAsFloat()*0.0437f )

GuiSearch::GuiSearch(WindowManager& window, SystemManager& systemManager)
		: Gui(window),
		  mSystemManager(systemManager),
		  mBackground(window, Path(":/frame.png")),
		  mGrid(window, Vector2i(3, 3)),
		  mList(nullptr),
		  mJustOpen(true)
{
	addChild(&mBackground);
	addChild(&mGrid);

	mMenuTheme = MenuThemeData::getInstance()->getCurrentTheme();

	mBackground.setImagePath(mMenuTheme->menuBackground.path);
	mBackground.setCenterColor(mMenuTheme->menuBackground.color);
	mBackground.setEdgeColor(mMenuTheme->menuBackground.color);

	initGridsNStuff();

	updateSize();
	setPosition((Renderer::Instance().DisplayWidthAsFloat() - mSize.x()) / 2,
	            (Renderer::Instance().DisplayHeightAsFloat() - mSize.y()) / 2);

	PopulateGrid("");
}

void GuiSearch::initGridsNStuff()
{
	//init Title
	mTitle = std::make_shared<TextComponent>(mWindow, _("SEARCH") + " : ", mMenuTheme->menuText.font,
	                                         mMenuTheme->menuText.color, TextAlignment::Right);
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true, Vector2i(1, 1));

	//init search textfield
	mSearch = std::make_shared<TextEditComponent>(mWindow);
	mGrid.setEntry(mSearch, Vector2i(0, 1), false, false, Vector2i(3, 1));

	//init search option selector
	mSearchChoices = std::make_shared<OptionListComponent<FolderData::FastSearchContext> >(mWindow,
	                                                                                       _("SEARCH BY"), false);
	FolderData::FastSearchContext currentSearch = FolderData::FastSearchContext::Name;

	mSearchChoices->add(_("Name"), FolderData::FastSearchContext::Name, currentSearch == FolderData::FastSearchContext::Name);
	mSearchChoices->add(_("Description"), FolderData::FastSearchContext::Description, currentSearch == FolderData::FastSearchContext::Description);
	mSearchChoices->add(_("DEVELOPER"), FolderData::FastSearchContext::Developer, currentSearch == FolderData::FastSearchContext::Developer);
	mSearchChoices->add(_("PUBLISHER"), FolderData::FastSearchContext::Publisher, currentSearch == FolderData::FastSearchContext::Publisher);
	mSearchChoices->add(_("FILENAME"), FolderData::FastSearchContext::Path, currentSearch == FolderData::FastSearchContext::Path);
	mSearchChoices->add(_("ALL"), FolderData::FastSearchContext::All, currentSearch == FolderData::FastSearchContext::All);

	mSearchChoices->setChangedCallback([this]{
		mGrid.setColWidthPerc(1, mSearchChoices->getSize().x() / mSize.x());
	});
	mGrid.setEntry(mSearchChoices, Vector2i(1, 0), false, false, Vector2i(1, 1));

	//init big center grid with List and Meta
	mGridMeta = std::make_shared<ComponentGrid>(mWindow, Vector2i(4, 3));
	mGridMeta->setEntry(std::make_shared<Component>(mWindow), Vector2i(1, 0), false, false, Vector2i(1, 3));
	mGridMeta->setEntry(std::make_shared<Component>(mWindow), Vector2i(2, 0), false, false, Vector2i(2, 1));
	mText = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuText.font,
	                                        mMenuTheme->menuText.color, TextAlignment::Center);
	mList = std::make_shared<ComponentList>(mWindow);
	mGridMeta->setEntry(mList, Vector2i(0, 0), true, true, Vector2i(1, 3));

	mList->setCursorChangedCallback([this](CursorState state)
	                                {
		                                (void) state;
		                                if (mList->getChildCount()>0)
			                                populateGridMeta(mList->getCursor());
	                                });
	mList->setFocusLostCallback([this]
	                            {
		                            clear();
	                            });
	mList->setFocusGainedCallback([this]
	                              {
		                              if (mList->getChildCount()>0)
			                              populateGridMeta(mList->getCursor());
	                              });

	mGrid.setEntry(mGridMeta, Vector2i(0, 2), true, true, Vector2i(3, 1));
	mGridMeta->setEntry(mText, Vector2i(0, 0), false, true, Vector2i(4, 3));


	//init grid for Meta with logo, image etc
	mGridLogoAndMD = std::make_shared<ComponentGrid>(mWindow, Vector2i(2, 6));

	// 3x3 grid to center logo
	mGridLogo = std::make_shared<ComponentGrid>(mWindow, Vector2i(3, 3));

	mGridLogoAndMD->setEntry(mGridLogo, Vector2i(0, 0), false, true, Vector2i(2, 1));

	mMDDeveloper = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
	                                        mMenuTheme->menuTextSmall.color, TextAlignment::Left);
	mMDDeveloperLabel = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
	                                                    mMenuTheme->menuTextSmall.color, TextAlignment::Left);
	mMDPublisher = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
	                                               mMenuTheme->menuTextSmall.color, TextAlignment::Left);
	mMDPublisherLabel = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuTextSmall.font,
	                                                    mMenuTheme->menuTextSmall.color, TextAlignment::Left);

	mGridLogoAndMD->setEntry(mMDDeveloperLabel, Vector2i(0, 2), false, false, Vector2i(1, 1));
	mGridLogoAndMD->setEntry(mMDDeveloper, Vector2i(1, 2), false, true, Vector2i(1, 1));
	mGridLogoAndMD->setEntry(mMDPublisherLabel, Vector2i(0, 4), false, false, Vector2i(1, 1));
	mGridLogoAndMD->setEntry(mMDPublisher, Vector2i(1, 4), false, true, Vector2i(1, 1));
	mGridLogoAndMD->setEntry(std::make_shared<Component>(mWindow), Vector2i(0, 1), false, false, Vector2i(2, 1));
	mGridLogoAndMD->setEntry(std::make_shared<Component>(mWindow), Vector2i(0, 3), false, false, Vector2i(2, 1));
	mGridLogoAndMD->setEntry(std::make_shared<Component>(mWindow), Vector2i(0, 5), false, false, Vector2i(2, 1));


	mGridMeta->setEntry(mGridLogoAndMD, Vector2i(2, 1), false, true, Vector2i(1, 1));

	mResultSystemLogo = std::make_shared<ImageComponent>(mWindow);
	mGridLogo->setEntry(mResultSystemLogo, Vector2i(1, 1), false, false, Vector2i(1, 1));

	// selected result thumbnail + video
	mResultThumbnail = std::make_shared<ImageComponent>(mWindow);
	mGridMeta->setEntry(mResultThumbnail, Vector2i(3, 1), false, true, Vector2i(1, 1));
	mResultVideo = std::make_shared<VideoComponent>(mWindow);
	mGridMeta->setEntry(mResultVideo, Vector2i(3, 1), false, true, Vector2i(1, 1));

	// selected result desc + container
	mDescContainer = std::make_shared<ScrollableContainer>(mWindow);
	mResultDesc = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuText.font, mMenuTheme->menuText.color);
	mDescContainer->addChild(mResultDesc.get());
	mDescContainer->setAutoScroll(true);
	mGridMeta->setEntry(mDescContainer, Vector2i(2, 2), false, true, Vector2i(2, 1));

	//col with mList
	mGridMeta->setColWidthPerc(0, 0.39f);
	//spacer col
	mGridMeta->setColWidthPerc(1, 0.01f);
	//cols with metadata
	mGridMeta->setColWidthPerc(2, 0.30f);
	mGridMeta->setColWidthPerc(3, 0.30f);
	//spacer row
	mGridMeta->setRowHeightPerc(0, 0.0825f);
}

GuiSearch::~GuiSearch()
{
	if (mList)
	{
		mList->clear();
	}
}

float GuiSearch::getButtonGridHeight() const
{
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	return (mButtonGrid ? mButtonGrid->getSize().y() : menuTheme->menuText.font->getHeight() + BUTTON_GRID_VERT_PADDING);
}

bool GuiSearch::ProcessInput(const class InputCompactEvent & event)
{
  if (event.CancelPressed())
	{
    clear();
		Close();
		return true;
	}
  else if (event.R1Pressed())
  {
	  mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("SEARCH"), mSearch->getValue(), this));
	  return true;
  }
  else if (event.ValidPressed())
  {
    clear();
	  launch();
	  return true;
  }
  else if (event.SelectPressed())
  {
    clear();
    GoToGame();
    Close();
    mWindow.CloseAll();
    return true;
  }
  if (!mSearchResults.empty())
  {
    FileData* cursor = mSearchResults[mList->getCursor()];

    // NETPLAY
    if ((event.XPressed()) && (RecalboxConf::Instance().GetNetplayEnabled())
        && cursor->System().Descriptor().HasNetPlayCores())
    {
      clear();
      mWindow.pushGui(new GuiNetPlayHostPasswords(mWindow, *cursor));
      return true;
    }
    if (event.YPressed())
    {
      if (cursor->IsGame() && cursor->System().HasFavoritesInTheme())
      {
        MetadataDescriptor& md = cursor->Metadata();
        SystemData* favoriteSystem = mSystemManager.FavoriteSystem();

        md.SetFavorite(!md.Favorite());

        if (favoriteSystem != nullptr)
        {
          if (md.Favorite()) favoriteSystem->GetFavoriteRoot().AddChild(cursor, false);
          else favoriteSystem->GetFavoriteRoot().RemoveChild(cursor);

          ViewController::Instance().setInvalidGamesList(&cursor->System());
          ViewController::Instance().setInvalidGamesList(favoriteSystem);
        }
        ViewController::Instance().getSystemListView().manageFavorite();

        populateGridMeta(mList->getCursor());
      }
      return true;
    }
  }

  if (event.AnyLeftPressed() || event.AnyRightPressed()) return mSearchChoices->ProcessInput(event);

	return Component::ProcessInput(event);
}

void GuiSearch::updateSize()
{
	const float height = Renderer::Instance().DisplayHeightAsFloat() * 0.85f;
	const float width = Renderer::Instance().DisplayWidthAsFloat() * 0.95f;
	setSize(width, height);
}

void GuiSearch::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

  mSearch->setSize(mSize.x() - 12, mSearch->getSize().y());

	// update grid row/col sizes
  mGrid.setColWidthPerc(0, 0.5f);
	mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y());
	mGrid.setColWidthPerc(1, mSearchChoices->getSize().x() / mSize.x());
	mGrid.setRowHeightPerc(1, (mSearch->getSize().y() + 8) / mSize.y());

	mGrid.setSize(mSize);
}

bool GuiSearch::getHelpPrompts(Help& help)
{
  help.Clear();
  if (AmIOnTopOfScreen())
  {
    help.Set(HelpType::LeftRight, _("SEARCH IN..."))
        .Set(HelpType::UpDown, _("SELECT"))
        .Set(Help::Cancel(), _("BACK"))
        .Set(HelpType::R, _("KEYBOARD"));
    if (!mList->isEmpty())
      help.Set(Help::Valid(), _("LAUNCH"))
          .Set(HelpType::Select, _("GO TO GAME"))
          .Set(HelpType::X, _("NETPLAY"))
          .Set(HelpType::Y,
               mSearchResults[mList->getCursor()]->Metadata().Favorite() ? _("Remove from favorite") : _("Favorite"));
    return true;
  }
  return false;
}

void GuiSearch::Update(int deltaTime)
{
	Component::Update(deltaTime);

	if (mJustOpen)
  {
    mWindow.pushGui(new GuiArcadeVirtualKeyboard(mWindow, _("SEARCH"), mSearch->getValue(), this));
	  mJustOpen = false;
  }
}

void GuiSearch::Render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	renderChildren(trans);

	Renderer::SetMatrix(trans);
	Renderer::DrawRectangle(0.f, 0.f, mSize.x(), mSize.y(), 0x00000011);
}

void GuiSearch::PopulateGrid(const std::string& search)
{
	if (mList) {
		mList->clear();
	}

	if (search.length()>2)
	{
    ViewController::State viewControllerState = ViewController::Instance().getState();
    SystemData* systemData = viewControllerState.viewing == ViewController::ViewMode::GameList ? viewControllerState.getSystem() : nullptr;
		mSearchResults =  mSystemManager.searchTextInGames(mSearchChoices->getSelected(), search, 100, 500, systemData);
		if (!mSearchResults.empty())
		{
			mText->setValue("");
			ComponentListRow row;
			std::shared_ptr<Component> ed;
			for (auto *game : mSearchResults) {
				row.elements.clear();
				std::string gameName;

        gameName.append(game->System().Descriptor().IconPrefix());
				gameName.append(game->Metadata().Name());

				ed = std::make_shared<TextComponent>(mWindow, gameName, mMenuTheme->menuText.font,
				                                     mMenuTheme->menuText.color,
				                                     TextAlignment::Left);
				row.addElement(ed, true);
				row.makeAcceptInputHandler([this] { launch(); });
				mList->addRow(row, false, true);
			}

			clear();
			mGrid.getCellAt(0, 2)->canFocus = true;
			populateGridMeta(0);
		}
		else
		{
			mText->setValue(_("NO RESULTS"));
			clear();
			if (mList) mList->clear();
			//so we can jump to button if list is empty
			mGrid.getCellAt(0, 2)->canFocus = false;
		}
	}
	else
	{
		mText->setValue(_("TYPE AT LEAST 3 CHARACTERS"));
		clear();
		if (mList) mList->clear();
		mGrid.getCellAt(0, 2)->canFocus = false;
	}
}

void GuiSearch::clear()
{
	mResultThumbnail->setImage(Path::Empty);
	mResultVideo->setVideo(Path::Empty, 0, 0);
	mResultSystemLogo->setImage(Path::Empty);
	mResultDesc->setText("");
	mMDPublisherLabel->setText("");
	mMDPublisher->setText("");
	mMDDeveloperLabel->setText("");
	mMDDeveloper->setText("");
}

//called when changing cursor in mList to populate MD
void GuiSearch::populateGridMeta(int i)
{
	//screenshot & video
	mResultThumbnail->setImage(mSearchResults[i]->Metadata().Image());
	mResultThumbnail->setMaxSize(mGridMeta->getColWidth(2) * 0.9f, mGridMeta->getRowHeight(1)*0.9f);
	mResultVideo->setVideo(mSearchResults[i]->Metadata().Video(), 2000, 1);
	mResultVideo->setMaxSize(mGridMeta->getColWidth(2) * 0.9f, mGridMeta->getRowHeight(1)*0.9f);

	//system logo retieved from theme
	mResultSystemLogo->applyTheme(mSearchResults[i]->System().Theme(), "system", "logo", ThemeProperties::Path);
	mGridLogoAndMD->setRowHeightPerc(0, 0.5f);
	mResultSystemLogo->setMaxSize(mGridLogo->getSize().x() * 0.8f, mGridLogo->getSize().y() * 0.8f);
	ResizeGridLogo();

	//Metadata
	mResultDesc->setText(mSearchResults[i]->Metadata().Description());
	mDescContainer->setSize(mGridMeta->getColWidth(2) + mGridMeta->getColWidth(3), mGridMeta->getRowHeight(2)*0.9f);
	mResultDesc->setSize(mDescContainer->getSize().x(), 0); // make desc text wrap at edge of container
	mMDPublisherLabel->setText(_("Publisher") + " : ");
	mMDDeveloperLabel->setText(_("Developer") + " : ");
	mMDPublisher->setText(mSearchResults[i]->Metadata().Publisher());
	mMDDeveloper->setText(mSearchResults[i]->Metadata().Developer());
	float height = (mMDDeveloperLabel->getFont()->getLetterHeight() + 2) / mGridLogoAndMD->getSize().y();
	mGridLogoAndMD->setRowHeightPerc(2, height);
	mGridLogoAndMD->setRowHeightPerc(4, height);
	int width = (int)Math::max(mMDDeveloperLabel->getSize().x(), mMDPublisherLabel->getSize().x());
	mGridLogoAndMD->setColWidthPerc(0, (float)width / mGridLogoAndMD->getSize().x());
	mGridLogoAndMD->getCellAt(0, 2)->resize = true;
	mGridLogoAndMD->getCellAt(0, 4)->resize = true;

  updateHelpPrompts();
}

void GuiSearch::ResizeGridLogo()
{
	//trying to center the logo in a 3x3 grid by adjusting col and row size
	int height = (int)mResultSystemLogo->getSize().y();
	int width = (int)mResultSystemLogo->getSize().x();

	float spacer2 = (mGridLogo->getSize().x() - (float)width) / 2 / mGridLogo->getSize().x();

	mGridLogo->setColWidthPerc(0, spacer2);
	mGridLogo->setColWidthPerc(1, (float)width / mGridLogo->getSize().x());
	mGridLogo->setColWidthPerc(2, spacer2);

	float spacer = (mGridLogo->getSize().y() - (float)height) / 2 / mGridLogo->getSize().y();

	mGridLogo->setRowHeightPerc(0, spacer);
	mGridLogo->setRowHeightPerc(1, (float)height / mGridLogo->getSize().y());
	mGridLogo->setRowHeightPerc(2, spacer);
}

void GuiSearch::launch()
{
  if (mList->size() != 0)
  {
    VideoEngine::Instance().StopVideo(true);
    mResultVideo->setVideo(Path::Empty, 0, 0);

    int index = mList->getCursor();
    Vector3f target(Renderer::Instance().DisplayWidthAsFloat() / 2.0f, Renderer::Instance().DisplayHeightAsFloat() / 2.0f, 0);
    ViewController::Instance().Launch(mSearchResults[index], GameLinkedData(), target);
  }
}

void GuiSearch::GoToGame()
{
  if (mList->size() != 0)
  {
    VideoEngine::Instance().StopVideo(true);
    mResultVideo->setVideo(Path::Empty, 0, 0);

    int index = mList->getCursor();
    ViewController::Instance().selectGamelistAndCursor(mSearchResults[index]);
  }
}

void GuiSearch::ArcadeVirtualKeyboardTextChange(GuiArcadeVirtualKeyboard& /*vk*/, const std::string& text)
{
  mSearch->setValue(text);
  PopulateGrid(text);
}

void GuiSearch::ArcadeVirtualKeyboardValidated(GuiArcadeVirtualKeyboard& /*vk*/, const std::string& /*text*/)
{
}

void GuiSearch::ArcadeVirtualKeyboardCanceled(GuiArcadeVirtualKeyboard& /*vk*/)
{
}
