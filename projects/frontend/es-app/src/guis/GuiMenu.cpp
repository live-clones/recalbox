#include <list>
#include <algorithm>
#include <string>

#include <functional>
#include <LibretroRatio.h>
#include <systems/SystemManager.h>
#include <MainRunner.h>

#include "EmulationStation.h"
#include "guis/GuiMenu.h"
#include "Window.h"
#include "Sound.h"
#include "Settings.h"
#include "recalbox/RecalboxSystem.h"
#include "recalbox/RecalboxUpgrade.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiScraperStart.h"
#include "guis/GuiDetectDevice.h"
#include "guis/GuiUpdate.h"
#include "views/ViewController.h"
#include "AudioManager.h"
#include "platform.h"

#include "components/SwitchComponent.h"
#include "components/SliderComponent.h"
#include "components/TextComponent.h"
#include "components/OptionListComponent.h"
#include "components/MenuComponent.h"
#include "VolumeControl.h"

#include "guis/GuiTextEditPopup.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "GuiLoading.h"

#include "RecalboxConf.h"
#include "MenuMessages.h"

#include "MenuThemeData.h"
#include "animations/LambdaAnimation.h"
#include "GuiHashStart.h"

GuiMenu::GuiMenu(Window& window, SystemManager& systemManager)
  : Gui(window),
    mSystemManager(systemManager),
    mMenu(window, _("MAIN MENU").c_str()),
    mVersion(window)
{
  // MAIN MENU
  // KODI >
  // SYSTEM >
  // GAMES >
  // CONTROLLERS >
  // UI SETTINGS >
  // SOUND SETTINGS >
  // NETWORK >
  // SCRAPER >
  // ADVANCED SETTINGS >
  // ABOUT >
  // QUIT >

  mMenuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  bool bartop =  RecalboxConf::Instance().AsString("emulationstation.menu") == "bartop";

  //KODI
    if (RecalboxConf::Instance().AsBool("kodi.enabled")) {
        addEntryWithHelp(_("KODI MEDIA CENTER").c_str(), _(MenuMessages::START_KODI_HELP_MSG), mMenuTheme->menuText.color, true,
                 [this] {
                     if (!RecalboxSystem::launchKodi(mWindow)) {
                         LOG(LogWarning) << "Shutdown terminated with non-zero result!";
                     }
                 }, mMenuTheme->menuIconSet.kodi);
    }

  //SYSTEM
    if (!bartop) {
        addEntryWithHelp(_("SYSTEM SETTINGS").c_str(), _(MenuMessages::SYSTEM_HELP_MSG), mMenuTheme->menuText.color, true,
             [this] { menuSystem(); }, mMenuTheme->menuIconSet.system);
    }

  //UPDATES
    if (!bartop) {
        addEntryWithHelp(_("UPDATES").c_str(), _(MenuMessages::UPDATE_HELP_MSG), mMenuTheme->menuText.color, true,
             [this] { menuUpdates(); }, mMenuTheme->menuIconSet.updates);
    }

    //GAME SETTINGS
    addEntryWithHelp(_("GAMES SETTINGS").c_str(), _(MenuMessages::GAME_SETTINGS_HELP_MSG), mMenuTheme->menuText.color, true,
           [this] { menuGameSettings(); }, mMenuTheme->menuIconSet.games);

    //CONTROLLERS
    if (!bartop) {
        addEntryWithHelp(_("CONTROLLERS SETTINGS").c_str(), _(MenuMessages::CONTROLLER_HELP_MSG), mMenuTheme->menuText.color, true,
             [this] { menuControllers(); }, mMenuTheme->menuIconSet.controllers);
    }

  //UI SETTINGS
    if (!bartop) {

        addEntryWithHelp(_("UI SETTINGS").c_str(), _(MenuMessages::UI_HELP_MSG), mMenuTheme->menuText.color, true,
             [this] { menuUISettings(); }, mMenuTheme->menuIconSet.ui);
    }

    //SOUND SETTINGS
    addEntryWithHelp(_("SOUND SETTINGS").c_str(), _(MenuMessages::SOUND_HELP_MSG), mMenuTheme->menuText.color, true,
                     [this] { menuSoundSettings(); }, mMenuTheme->menuIconSet.sound);

  //NETWORK SETTINGS
    if (!bartop) {
        addEntryWithHelp(_("NETWORK SETTINGS").c_str(), _(MenuMessages::NETWORK_HELP_MSG), mMenuTheme->menuText.color, true,
                 [this] { menuNetworkSettings(); }, mMenuTheme->menuIconSet.network);
    }

    //SCRAPPER
    if (!bartop) {
        addEntryWithHelp(_("SCRAPER").c_str(), _(MenuMessages::SCRAPER_HELP_MSG), mMenuTheme->menuText.color, true,
                 [this] { menuScrapper(); }, mMenuTheme->menuIconSet.scraper);
    }

    //ADVANCED SETTINGS
    if (!bartop) {
        addEntryWithHelp(_("ADVANCED SETTINGS").c_str(), _(MenuMessages::ADVANCED_HELP_MSG), mMenuTheme->menuText.color, true,
                 [this] { menuAdvancedSettings(); }, mMenuTheme->menuIconSet.advanced);
    }

	//ABOUT
	addEntry(_("LICENSE").c_str(), mMenuTheme->menuText.color, true,
	         [this] {
		         mWindow.pushGui(new GuiMsgBoxScroll(mWindow, "RECALBOX", Strings::ScrambleSymetric2(std::string(MenuMessages::LICENCE_MSG, MenuMessages::LICENCE_MSG_SIZE), __MESSAGE_DECORATOR), _("OK"), nullptr, "", nullptr, "", nullptr, TextAlignment::Left));
		 }, Path(":/question.svg"));

    //QUIT
    addEntry(_("QUIT").c_str(), mMenuTheme->menuText.color, true,
             [this] { menuQuit(); }, mMenuTheme->menuIconSet.quit);

  mVersion.setFont(mMenuTheme->menuFooter.font);
  mVersion.setColor(mMenuTheme->menuFooter.color);

    mVersion.setText("RB EMULATIONSTATION V" + Strings::ToUpperASCII(PROGRAM_VERSION_STRING));
    mVersion.setHorizontalAlignment(TextAlignment::Center);

    addChild(&mMenu);
    addChild(&mVersion);

    setSize(mMenu.getSize());
    setPosition((Renderer::getDisplayWidthAsFloat() - mSize.x()) / 2, (Renderer::getDisplayHeightAsFloat() - mSize.y()) / 2);

  // Animation
  auto fadeFunc = [this](float t) {
    setOpacity(lerp<float>(0, 255, t));
    setPosition(getPosition().x(), lerp<float>(getPosition().y(), (Renderer::getDisplayHeightAsFloat() - mSize.y()) / 2, t));
  };

  setOpacity(0);

  setAnimation(new LambdaAnimation(fadeFunc, 200), 0);
}

GuiMenu::~GuiMenu() {
    clearLoadedInput();
}

void GuiMenu::createInputTextRow(GuiSettings *gui, const std::string& title, const char *settingsID, bool password, const std::string& help = "") {
  // LABEL
  ComponentListRow row;

  auto lbl = std::make_shared<TextComponent>(mWindow, title, mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  row.addElement(lbl, true); // label

  std::shared_ptr<GuiComponent> ed;

  ed = std::make_shared<TextComponent>(mWindow, ((password && !RecalboxConf::Instance().AsString(settingsID).empty()) ?
      "*********" : RecalboxConf::Instance().AsString(settingsID)),
                                       mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Right);
  row.addElement(ed, true);

  auto spacer = std::make_shared<GuiComponent>(mWindow);
  spacer->setSize(Renderer::getDisplayWidthAsFloat() * 0.005f, 0);
  row.addElement(spacer, false);

  auto bracket = std::make_shared<ImageComponent>(mWindow);
  bracket->setImage(mMenuTheme->iconSet.arrow);
  bracket->setColorShift(mMenuTheme->menuText.color);
  bracket->setResize(Vector2f(0, Math::round(mMenuTheme->menuText.font->getLetterHeight())));


  row.addElement(bracket, false);

  auto updateVal = [ed, settingsID, password](const std::string &newVal) {
    if (!password)
      ed->setValue(newVal);
    else {
      ed->setValue("*********");
    }
    RecalboxConf::Instance().SetString(settingsID, newVal);
  }; // ok callback (apply new value to ed)

  row.makeAcceptInputHandler([this, title, updateVal, settingsID] {
    if (Settings::Instance().UseOSK()) {
      mWindow.pushGui(
          new GuiTextEditPopupKeyboard(mWindow, title, RecalboxConf::Instance().AsString(settingsID),
                         updateVal, false));
    } else {
      mWindow.pushGui(
          new GuiTextEditPopup(mWindow, title, RecalboxConf::Instance().AsString(settingsID),
                     updateVal, false));
    }
  });
  gui->addRowWithHelp(row, title, help);
}

void GuiMenu::menuSystem(){

  auto s = new GuiSettings(mWindow, _("SYSTEM SETTINGS").c_str());

  auto version = std::make_shared<TextComponent>(mWindow,
      RecalboxUpgrade::getVersion(),
      mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  s->addWithLabel(version, _("VERSION"), _(MenuMessages::VERSION_HELP_MSG));
  bool warning = RecalboxSystem::isFreeSpaceLimit();
  auto space = std::make_shared<TextComponent>(mWindow,
      RecalboxSystem::getFreeSpaceInfo(),
      mMenuTheme->menuText.font,
      warning ? 0xFF0000FF : mMenuTheme->menuText.color);
  s->addWithLabel(space, _("DISK USAGE"), _(MenuMessages::DISK_USAGE_HELP_MSG));

  std::vector<std::string> availableStorage = RecalboxSystem::getAvailableStorageDevices();
  std::string selectedStorage = RecalboxSystem::getCurrentStorage();

  // Storage device
  auto optionsStorage = std::make_shared<OptionListComponent<std::string> >(mWindow,
      _("STORAGE DEVICE"), false);
  if (selectedStorage == "NETWORK") {
    optionsStorage->add("NETWORK", "NETWORK", true);
  }
  for (auto& it : availableStorage)
  {
    if (it != "RAM") {
      if (Strings::StartsWith(it, "DEV")) {
        std::vector<std::string> tokens = Strings::Split(it, ' ');
        if (tokens.size() >= 3) {
          optionsStorage->add(tokens.at(2), it,
              selectedStorage == std::string("DEV " + tokens.at(1)));
        }
      } else {
        optionsStorage->add(it, it, selectedStorage == it);
      }
    }
  }

  s->addWithLabel(optionsStorage, _("STORAGE DEVICE"), _(MenuMessages::STORAGE_DEVICE_HELP_MSG));

  // language choice
  auto language_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, _("LANGUAGE"), false);
  std::string language = RecalboxConf::Instance().AsString("system.language");
  if (language.empty()) language = "en_US";
  language_choice->add("EUSKARA", "eu_ES", language == "eu_ES");
  language_choice->add("正體中文", "zh_TW", language == "zh_TW");
  language_choice->add("简体中文", "zh_CN", language == "zh_CN");
  language_choice->add("DEUTSCH", "de_DE", language == "de_DE");
  language_choice->add("ENGLISH", "en_US", language == "en_US");
  language_choice->add("ESPAÑOL", "es_ES", language == "es_ES");
  language_choice->add("FRANÇAIS", "fr_FR", language == "fr_FR");
  language_choice->add("ITALIANO", "it_IT", language == "it_IT");
  language_choice->add("PORTUGUES", "pt_BR", language == "pt_BR");
  language_choice->add("SVENSKA", "sv_SE", language == "sv_SE");
  language_choice->add("TÜRKÇE", "tr_TR", language == "tr_TR");
  language_choice->add("CATALÀ", "ca_ES", language == "ca_ES");
  language_choice->add("اللغة العربية", "ar_YE", language == "ar_YE");
  language_choice->add("NEDERLANDS", "nl_NL", language == "nl_NL");
  language_choice->add("ελληνικά", "el_GR", language == "el_GR");
  language_choice->add("한국어", "ko_KR", language == "ko_KR");
  language_choice->add("NORSK", "nn_NO", language == "nn_NO");
  language_choice->add("BOKMAL", "nb_NO", language == "nb_NO");
  language_choice->add("POLSKI", "pl_PL", language == "pl_PL");
  language_choice->add("日本語", "ja_JP", language == "ja_JP");
  language_choice->add("Русский", "ru_RU", language == "ru_RU");
  language_choice->add("MAGYAR", "hu_HU", language == "hu_HU");
  language_choice->add("čeština", "cs_CZ", language == "cs_CZ");
  language_choice->add("latviešu", "lv_LV", language == "lv_LV");
  language_choice->add("Lëtzebuergesch", "lb_LU", language == "lb_LU");

  s->addWithLabel(language_choice, _("LANGUAGE"), _(MenuMessages::LANGUAGE_HELP_MSG));

  auto keyboard_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, _("KEYBOARD"), false);
  std::string keyboard = RecalboxConf::Instance().AsString("system.kblayout");
  if (keyboard.empty()) keyboard = "us";

  // linux loadkeys value
  keyboard_choice->add("DE", "de", keyboard == "de");
  keyboard_choice->add("DK", "dk", keyboard == "dk");
  keyboard_choice->add("ES", "es", keyboard == "es");
  keyboard_choice->add("FR", "fr", keyboard == "fr");
  keyboard_choice->add("IT", "it", keyboard == "it");
  keyboard_choice->add("US", "us", keyboard == "us");

  s->addWithLabel(keyboard_choice, _("KEYBOARD"), _(MenuMessages::KEYBOARD_HELP_MSG));

  s->addSaveFunc([this, language_choice, language, keyboard_choice, keyboard, optionsStorage, selectedStorage] {
    bool reboot = false;
    if (optionsStorage->changed()) {
      RecalboxSystem::setStorage(optionsStorage->getSelected());
      reboot = true;
    }

    if (language != language_choice->getSelected()) {
      RecalboxConf::Instance().SetString("system.language", language_choice->getSelected());
      RecalboxConf::Instance().SaveRecalboxConf();
      reboot = true;
    }

    if (keyboard != keyboard_choice->getSelected()) {
      RecalboxConf::Instance().SetString("system.kblayout", keyboard_choice->getSelected());
      RecalboxConf::Instance().SaveRecalboxConf();
      reboot = true;
    }

    if (reboot) {
      mWindow.pushGui(
          new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [] {
            if (runRestartCommand() != 0) {
              LOG(LogWarning) << "Reboot terminated with non-zero result!";
            }
          })
      );
    }

  });
  mWindow.pushGui(s);
}

void GuiMenu::menuUpdates(){
  GuiSettings *updateGui = new GuiSettings(mWindow, _("UPDATES").c_str());
  // Enable updates
  auto updates_enabled = std::make_shared<SwitchComponent>(mWindow);
  updates_enabled->setState(RecalboxConf::Instance().AsBool("updates.enabled"));
  updateGui->addWithLabel(updates_enabled, _("CHECK UPDATES"), _(MenuMessages::UPDATE_CHECK_HELP_MSG));
  // Display available update version
  if (RecalboxUpgrade::canUpdate()) {
    auto updateVersion = std::make_shared<TextComponent>(mWindow,
        _("YES"), mMenuTheme->menuText.font, mMenuTheme->menuText.color);
    updateGui->addWithLabel(updateVersion, _("AVAILABLE UPDATE"), _(MenuMessages::UPDATE_VERSION_HELP_MSG));

    // Display available update changelog
    updateGui->addSubMenu(_("UPDATE CHANGELOG"), [this] {
      std::string changelog = RecalboxUpgrade::getUpdateChangelog();
      if (!changelog.empty()) {
        const std::string& message = changelog;
        std::string updateVersion = RecalboxUpgrade::getUpdateVersion();
        mWindow.displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"),
            _("UPDATE VERSION:") + ' ' + updateVersion + "\n" +
            _("UPDATE CHANGELOG:") + "\n" + message);
      } else {
        mWindow.displayMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"));
      }
    }, _(MenuMessages::UPDATE_CHANGELOG_HELP_MSG));
    // Start update
    updateGui->addSubMenu(_("START UPDATE"), [this] {
      mWindow.pushGui(new GuiUpdate(mWindow));
    }, _(MenuMessages::START_UPDATE_HELP_MSG));
  }

  // Enable updates

  auto updatesTypeComp = std::make_shared<OptionListComponent<std::string> >(mWindow,
      _("UPDATE TYPE"), false);
  std::string updatesType = RecalboxConf::Instance().AsString("updates.type");
  // Stable or nothing
  if (updatesType.empty() || updatesType == "beta" || updatesType == "unstable") {
    updatesType = "stable";
  }
  updatesTypeComp->add("stable", "stable", updatesType == "stable");
  updatesTypeComp->add("custom", "custom", updatesType != "stable");

  updateGui->addWithLabel(updatesTypeComp, _("UPDATE TYPE"), _(MenuMessages::UPDATE_TYPE_HELP_MSG));
  updateGui->addSaveFunc([updates_enabled, updatesTypeComp] {
    RecalboxConf::Instance().SetBool("updates.enabled", updates_enabled->getState());
    if(updatesTypeComp->getSelected() == "stable"){
      RecalboxConf::Instance().SetString("updates.type", updatesTypeComp->getSelected());
    }
    RecalboxConf::Instance().SaveRecalboxConf();
  });
  mWindow.pushGui(updateGui);
}

void GuiMenu::menuGameSettings(){
  auto s = new GuiSettings(mWindow, _("GAMES SETTINGS").c_str());
  if (RecalboxConf::Instance().AsString("emulationstation.menu") != "bartop") {
    // Screen ratio choice
    auto ratio_choice = createRatioOptionList(mWindow, "global");
    s->addWithLabel(ratio_choice, _("GAME RATIO"), _(MenuMessages::GAME_RATIO_HELP_MSG));
    s->addSaveFunc([ratio_choice] {
      RecalboxConf::Instance().SetString("global.ratio", ratio_choice->getSelected());
      RecalboxConf::Instance().SaveRecalboxConf();
    });
  }
  // smoothing
  auto smoothing_enabled = std::make_shared<SwitchComponent>(mWindow);
  smoothing_enabled->setState(RecalboxConf::Instance().AsBool("global.smooth"));
  s->addWithLabel(smoothing_enabled, _("SMOOTH GAMES"), _(MenuMessages::GAME_SMOOTH_HELP_MSG));

  // rewind
  auto rewind_enabled = std::make_shared<SwitchComponent>(mWindow);
  rewind_enabled->setState(RecalboxConf::Instance().AsBool("global.rewind"));
  s->addWithLabel(rewind_enabled, _("REWIND"), _(MenuMessages::GAME_REWIND_HELP_MSG));


  // autosave/load
  auto autosave_enabled = std::make_shared<SwitchComponent>(mWindow);
  autosave_enabled->setState(RecalboxConf::Instance().AsBool("global.autosave"));
  s->addWithLabel(autosave_enabled, _("AUTO SAVE/LOAD"), _(MenuMessages::GAME_AUTOSAVELOAD_HELP_MSG));

  // press twice to quit emulator
  auto quit_press_twice_enabled = std::make_shared<SwitchComponent>(mWindow);
  quit_press_twice_enabled->setState(RecalboxConf::Instance().AsBool("global.quitpresstwice"));
  s->addWithLabel(quit_press_twice_enabled, _("PRESS TWICE TO QUIT GAME"), _(MenuMessages::GAME_PRESS_TWICE_QUIT_HELP_MSG));
  s->addSaveFunc([quit_press_twice_enabled] {
      RecalboxConf::Instance().SetBool("global.quitpresstwice", quit_press_twice_enabled->getState());
      RecalboxConf::Instance().SaveRecalboxConf();
    });

  // Shaders preset

  auto shaders_choices = std::make_shared<OptionListComponent<std::string> >(mWindow,
      _("SHADERS SET"), false);
  std::string currentShader = RecalboxConf::Instance().AsString("global.shaderset");
  if (currentShader.empty()) {
    currentShader = std::string("none");
  }

  shaders_choices->add(_("NONE"), "none", currentShader == "none");
  shaders_choices->add(_("SCANLINES"), "scanlines", currentShader == "scanlines");
  shaders_choices->add(_("RETRO"), "retro", currentShader == "retro");
  s->addWithLabel(shaders_choices, _("SHADERS SET"), _(MenuMessages::GAME_SHADERS_HELP_MSG));
  // Integer scale
  auto integerscale_enabled = std::make_shared<SwitchComponent>(mWindow);
  integerscale_enabled->setState(RecalboxConf::Instance().AsBool("global.integerscale"));
  s->addWithLabel(integerscale_enabled, _("INTEGER SCALE (PIXEL PERFECT)"), _(MenuMessages::GAME_INTEGER_SCALE_HELP_MSG));
  s->addSaveFunc([integerscale_enabled] {
    RecalboxConf::Instance().SetBool("global.integerscale", integerscale_enabled->getState());
    RecalboxConf::Instance().SaveRecalboxConf();
  });

  shaders_choices->setSelectedChangedCallback(
      [integerscale_enabled](const std::string& selectedShader) {
        integerscale_enabled->setState(selectedShader != "none");
      });

  if (RecalboxConf::Instance().AsString("emulationstation.menu") != "bartop") {
    // Retroachievements
    {
      std::function<void()> openGui = [this] {
        GuiSettings *retroachievements = new GuiSettings(mWindow, _("RETROACHIEVEMENTS SETTINGS").c_str());
        // retroachievements_enable
        auto retroachievements_enabled = std::make_shared<SwitchComponent>(mWindow);
        retroachievements_enabled->setState(
            RecalboxConf::Instance().AsBool("global.retroachievements"));
        retroachievements->addWithLabel(retroachievements_enabled, _("RETROACHIEVEMENTS"), _(MenuMessages::RA_ONOFF_HELP_MSG));

        // retroachievements_hardcore_mode
        auto retroachievements_hardcore_enabled = std::make_shared<SwitchComponent>(
            mWindow);
        retroachievements_hardcore_enabled->setState(
            RecalboxConf::Instance().AsBool("global.retroachievements.hardcore"));
        retroachievements->addWithLabel(retroachievements_hardcore_enabled, _("HARDCORE MODE"), _(MenuMessages::RA_HARDCORE_HELP_MSG));


        // retroachievements, username, password
        createInputTextRow(retroachievements, _("USERNAME"), "global.retroachievements.username", false);
        createInputTextRow(retroachievements, _("PASSWORD"), "global.retroachievements.password", true);

        retroachievements->addSaveFunc(
            [retroachievements_enabled, retroachievements_hardcore_enabled] {
              RecalboxConf::Instance().SetBool("global.retroachievements", retroachievements_enabled->getState());
              RecalboxConf::Instance().SetBool("global.retroachievements.hardcore", retroachievements_hardcore_enabled->getState());
              RecalboxConf::Instance().SaveRecalboxConf();
            });
        mWindow.pushGui(retroachievements);
      };
      auto retroachievementsSettings = std::make_shared<TextComponent>(mWindow,
          _("RETROACHIEVEMENTS SETTINGS"), mMenuTheme->menuText.font, mMenuTheme->menuText.color);
      s->addSubMenu(_("RETROACHIEVEMENTS SETTINGS"), openGui, _(MenuMessages::RA_HELP_MSG));
    }
    if (RecalboxConf::Instance().AsString("emulationstation.menu") != "bartop") {
      // Netplay
      {
        std::function<void()> openGui = [this] {
          GuiSettings *netplay = new GuiSettings(mWindow, _("NETPLAY SETTINGS").c_str());
          // netplay_enable
          auto netplay_enabled = std::make_shared<SwitchComponent>(mWindow);
          netplay_enabled->setState(RecalboxConf::Instance().AsBool("global.netplay"));
          netplay->addWithLabel(netplay_enabled, _("NETPLAY"), _(MenuMessages::NP_ONOFF_HELP_MSG));

          // netplay username
          createInputTextRow(netplay, _("NICKNAME"), "global.netplay.nickname", false);

          // netplay port
          createInputTextRow(netplay, _("PORT"), "global.netplay.port", false);

          //mitm
          auto mitm_choices = std::make_shared<OptionListComponent<std::string> >(mWindow,
                                                                                  _("NETPLAY MITM"), false);
          std::string currentMitm = RecalboxConf::Instance().AsString("global.netplay.relay");
          if (currentMitm.empty()) {
            currentMitm = std::string("none");
          }

          mitm_choices->add(_("NONE"), "none", currentMitm == "none");
          mitm_choices->add(_("NEW YORK"), "nyc", currentMitm == "nyc");
          mitm_choices->add(_("MADRID"), "madrid", currentMitm == "madrid");
          netplay->addWithLabel(mitm_choices, _("NETPLAY MITM"), _(MenuMessages::NP_RELAY_HELP_MSG));

          netplay->addSaveFunc(
              [netplay_enabled, mitm_choices] {
                std::string mitm = mitm_choices->getSelected();
                if (mitm == "none") {
                  mitm.clear();
                }
                RecalboxConf::Instance().SetBool("global.netplay", netplay_enabled->getState());
                RecalboxConf::Instance().SetString("global.netplay.relay", mitm);
                RecalboxConf::Instance().SaveRecalboxConf();
              });
                    auto openHashNow = [this] { mWindow.pushGui(new GuiHashStart(mWindow, mSystemManager)); };
                    netplay->addSubMenu(_("HASH ROMS"), openHashNow, _(MenuMessages::NP_HASH_HELP_MSG));
          mWindow.pushGui(netplay);
        };
        s->addSubMenu(_("NETPLAY SETTINGS"), openGui, _(MenuMessages::NP_HELP_MSG));
      }
    }

  }
  s->addSaveFunc([smoothing_enabled, rewind_enabled, shaders_choices, autosave_enabled] {
    RecalboxConf::Instance().SetBool("global.smooth", smoothing_enabled->getState());
    RecalboxConf::Instance().SetBool("global.rewind", rewind_enabled->getState());
    RecalboxConf::Instance().SetString("global.shaderset", shaders_choices->getSelected());
    RecalboxConf::Instance().SetBool("global.autosave", autosave_enabled->getState());
    RecalboxConf::Instance().SaveRecalboxConf();
  });
  mWindow.pushGui(s);
}

void GuiMenu::menuControllers() {

  GuiSettings *s = new GuiSettings(mWindow, _("CONTROLLERS SETTINGS").c_str());

  ComponentListRow row;
  row.makeAcceptInputHandler([this, s] {
    mWindow.pushGui(new GuiMsgBox(mWindow,
        _("YOU ARE GOING TO CONFIGURE A CONTROLLER. IF YOU HAVE ONLY ONE JOYSTICK, "
        "CONFIGURE THE DIRECTIONS KEYS AND SKIP JOYSTICK CONFIG BY HOLDING A BUTTON. "
        "IF YOU DO NOT HAVE A SPECIAL KEY FOR HOTKEY, CHOOSE THE SELECT BUTTON. SKIP "
        "ALL BUTTONS YOU DO NOT HAVE BY HOLDING A KEY. BUTTONS NAMES ARE BASED ON THE "
        "SNES CONTROLLER."), _("OK"),
        [this, s] {
          mWindow.pushGui(new GuiDetectDevice(mWindow, false, [this, s] {
            s->setSave(false);
            delete s;
            this->menuControllers();
          }));
        }));
  });


  row.addElement(std::make_shared<TextComponent>(mWindow, _("CONFIGURE A CONTROLLER"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRowWithHelp(row, _("CONFIGURE A CONTROLLER"), _(MenuMessages::CONTROLLER_CONF_HELP_MSG));

  row.elements.clear();

  std::function<void(std::vector<std::string>)> showControllerList = [this](const std::vector<std::string>& controllers)
  {
    if (controllers.empty())
    {
      mWindow.pushGui(new GuiMsgBox(mWindow, _("NO CONTROLLERS FOUND"), _("OK")));
    }
    else
    {
      GuiSettings *pairGui = new GuiSettings(mWindow, _("PAIR A BLUETOOTH CONTROLLER").c_str());
      for (auto & controllerString : controllers)
      {
        ComponentListRow controllerRow;
        std::function<void()> pairController = [this, controllerString]
        {
          mWindow.pushGui(new GuiLoading<bool>(mWindow, [controllerString]
          {
            return RecalboxSystem::pairBluetooth(controllerString);
          }, [this](bool paired)
          {
            mWindow.pushGui(new GuiMsgBox(mWindow, paired ? _("CONTROLLER PAIRED") : _("UNABLE TO PAIR CONTROLLER"), _("OK")));
          }));
        };
        controllerRow.makeAcceptInputHandler(pairController);
        auto update = std::make_shared<TextComponent>(mWindow, controllerString,
                                mMenuTheme->menuText.font, mMenuTheme->menuText.color);
        auto bracket = makeArrow(mWindow);
        controllerRow.addElement(update, true);
        controllerRow.addElement(bracket, false);
        pairGui->addRow(controllerRow);
      }
      mWindow.pushGui(pairGui);
    }
  };

  row.makeAcceptInputHandler([this, showControllerList]
  {
    mWindow.pushGui(new GuiLoading<std::vector<std::string>>(mWindow, []
    {
      return RecalboxSystem::scanBluetooth();
    }, showControllerList));
  });

  row.addElement(std::make_shared<TextComponent>(mWindow, _("PAIR A BLUETOOTH CONTROLLER"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRowWithHelp(row, _("PAIR A BLUETOOTH CONTROLLER"), _(MenuMessages::CONTROLLER_BT_HELP_MSG));
  row.elements.clear();

  row.makeAcceptInputHandler([this] {
    RecalboxSystem::forgetBluetoothControllers();
    mWindow.pushGui(new GuiMsgBox(mWindow, _("CONTROLLERS LINKS HAVE BEEN DELETED."), _("OK")));
  });
  row.addElement(std::make_shared<TextComponent>(mWindow, _("FORGET BLUETOOTH CONTROLLERS"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRowWithHelp(row, _("FORGET BLUETOOTH CONTROLLERS"), _(MenuMessages::CONTROLLER_FORGET_HELP_MSG));
  row.elements.clear();


  row.elements.clear();

  // Here we go; for each player
  std::list<int> alreadyTaken = std::list<int>();

  // clear the current loaded inputs
  clearLoadedInput();

  std::vector<std::shared_ptr<OptionListComponent<StrInputConfig *>>> options;
  char strbuf[256];

  for (int player = 0; player < InputEvent::sMaxPlayers; player++)
  {
    snprintf(strbuf, 256, _("INPUT P%i").c_str(), player + 1);
    auto inputOptionList = std::make_shared<OptionListComponent<StrInputConfig *> >(mWindow, strbuf, false);
    options.push_back(inputOptionList);

    // Checking if a setting has been saved, else setting to default
    const std::string& configuratedName = Settings::Instance().InputName(player);
    const std::string& configuratedGuid = Settings::Instance().InputGuid(player);
    bool found = false;
    // For each available and configured input
    for (auto it = 0; it < InputManager::Instance().DeviceCount(); it++) {
      InputDevice *config = InputManager::Instance().GetDeviceConfiguration(it);
      if (config->IsConfigured()) {
        // create name
        std::string displayName = '#' + Strings::ToString(config->Identifier()) + ' ';
        const std::string& deviceName = config->Name();
        if (deviceName.size() > 25) {
          displayName += deviceName.substr(0, 16) + "..." +
                 deviceName.substr(deviceName.size() - 5, deviceName.size() - 1);
        } else {
          displayName += deviceName;
        }

        bool foundFromConfig = configuratedName == config->Name() && configuratedGuid == config->GUID();
        int deviceID = config->Identifier();
        // Si la manette est configurée, qu'elle correspond a la configuration, et qu'elle n'est pas
        // deja selectionnée on l'ajoute en séléctionnée
        StrInputConfig *newInputConfig = new StrInputConfig(config->Name(), config->GUID());
        mLoadedInput.push_back(newInputConfig);

        if (foundFromConfig
          && std::find(alreadyTaken.begin(), alreadyTaken.end(), deviceID) == alreadyTaken.end()
          && !found) {
          found = true;
          alreadyTaken.push_back(deviceID);
          LOG(LogWarning) << "adding entry for player" << player << " (selected): " << config->Name() << "  " << config->GUID();
          inputOptionList->add(displayName, newInputConfig, true);
        } else {
          LOG(LogWarning) << "adding entry for player" << player << " (not selected): " << config->Name() << "  " << config->GUID();
          inputOptionList->add(displayName, newInputConfig, false);
        }
      }
    }
    if (configuratedName.empty() || !found) {
      LOG(LogWarning) << "adding default entry for player " << player << "(selected : true)";
      inputOptionList->add("default", nullptr, true);
    } else {
      LOG(LogWarning) << "adding default entry for player" << player << "(selected : false)";
      inputOptionList->add("default", nullptr, false);
    }

    // ADD default config

    // Populate controllers list
    s->addWithLabel(inputOptionList, strbuf);
  }
  s->addSaveFunc([options] {
    for (int player = 0; player < InputEvent::sMaxPlayers; player++) {
      std::string sstm = "INPUT P" + Strings::ToString(player + 1);
      std::string confName = sstm + "NAME";
      std::string confGuid = sstm + "GUID";

      auto input_p1 = options.at(player);
      std::string name;
      std::string selectedName = input_p1->getSelectedName();

      if (selectedName == "DEFAULT") {
        name = "DEFAULT";
        Settings::Instance().SetInputName(player, name);
        Settings::Instance().SetInputGuid(player, "");
      } else {
        if (input_p1->getSelected() != nullptr) {
          LOG(LogWarning) << "Found the selected controller ! : name in list  = " << selectedName;
          LOG(LogWarning) << "Found the selected controller ! : guid  = " << input_p1->getSelected()->deviceGUIDString;

          Settings::Instance().SetInputName(player, input_p1->getSelected()->deviceName);
          Settings::Instance().SetInputGuid(player, input_p1->getSelected()->deviceGUIDString);
        }
      }
    }

    Settings::Instance().saveFile();

  });

  row.elements.clear();
  mWindow.pushGui(s);
}

void GuiMenu::menuUISettings(){
  auto s = new GuiSettings(mWindow, _("UI SETTINGS").c_str());


  std::function<void()> openGuiSS = [this] {
	  auto ss = new GuiSettings(mWindow, _("SCREENSAVER").c_str());

	  // screensaver time
	  auto screensaver_time = std::make_shared<SliderComponent>(mWindow, 0.f, 30.f, 1.f, "m");
	  screensaver_time->setSlider((float)Settings::Instance().ScreenSaverTime() / (1000.0f * 60.0f));
	  ss->addWithLabel(screensaver_time, _("SCREENSAVER AFTER"), _(MenuMessages::UI_SCREENSAVER_AFTER_HELP_MSG));
	  ss->addSaveFunc([screensaver_time] {
		  Settings::Instance().SetScreenSaverTime(Math::roundi(screensaver_time->getSlider()) * (1000 * 60));
	  });

	  // screensaver behavior
	  auto screensaver_behavior = std::make_shared<OptionListComponent<std::string> >(mWindow, _("TRANSITION STYLE"),
	                                                                                  false);
	  std::vector<std::string> screensavers;
	  screensavers.push_back("dim");
	  screensavers.push_back("black");
	  screensavers.push_back("demo");
	  for (auto & screensaver : screensavers)
		  screensaver_behavior->add(screensaver, screensaver, Settings::Instance().ScreenSaverBehavior() == screensaver);
	  ss->addWithLabel(screensaver_behavior, _("SCREENSAVER BEHAVIOR"),
	                   _(MenuMessages::UI_SCREENSAVER_BEHAVIOR_HELP_MSG));
	  ss->addSaveFunc([screensaver_behavior] {
		  Settings::Instance().SetScreenSaverBehavior(screensaver_behavior->getSelected());
	  });

	  // add systems (all with a platformid specified selected)
	  auto systems = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SYSTEMS TO SHOW IN DEMO"), true);
	  for (auto it : mSystemManager.GetAllSystemList()) {
		  if (!it->hasPlatformId(PlatformIds::PlatformId::PLATFORM_IGNORE))
			  systems->add(it->getFullName(), it->getName(),
			               RecalboxConf::Instance().isInList("global.demo.systemlist", it->getName()) &&
			               it->PlatformCount() != 0);
	  }
	  ss->addWithLabel(systems, _("SYSTEMS FOR DEMO"));
	  ss->addSaveFunc([systems] {
		  std::vector<std::string> names = systems->getSelectedObjects();
		  RecalboxConf::Instance().SetList("global.demo.systemlist", names);
	  });
	  mWindow.pushGui(ss);
  };

  s->addSubMenu(_("SCREENSAVER"), openGuiSS, _(MenuMessages::UI_SCREENSAVER_HELP_MSG));


  // display clock
  auto show_time = std::make_shared<SwitchComponent>(mWindow);
  show_time->setState(Settings::Instance().ShowClock());
  s->addWithLabel(show_time, _("CLOCK IN MENU"), _(MenuMessages::UI_CLOCK_HELP_MSG));
  s->addSaveFunc([show_time] {
    Settings::Instance().SetShowClock(show_time->getState());
  });

  // show help
  auto show_help = std::make_shared<SwitchComponent>(mWindow);
  show_help->setState(Settings::Instance().ShowHelpPrompts());
  s->addWithLabel(show_help, _("ON-SCREEN HELP"), _(MenuMessages::UI_ONSCREENHELP_HELP_MSG));
  s->addSaveFunc([show_help] {
    Settings::Instance().SetShowHelpPrompts(show_help->getState());
  });

  std::function<void()> openGuiSub = [this] {
    GuiSettings *PopupGui = new GuiSettings(mWindow, _("POPUP SETTINGS").c_str());

    auto popup_position = std::make_shared<OptionListComponent<std::string> >(mWindow, _("POPUP POSITION"), false);

    std::string currentPos = Settings::Instance().PopupPosition();
    popup_position->add(_("TOP/RIGHT"), "Top/Right", currentPos == "Top/Right");
    popup_position->add(_("BOTTOM/RIGHT"), "Bottom/Right", currentPos == "Bottom/Right");
    popup_position->add(_("BOTTOM/LEFT"), "Bottom/Left", currentPos == "Bottom/Left");
    popup_position->add(_("TOP/LEFT"), "Top/Left", currentPos == "Top/Left");
    PopupGui->addWithLabel(popup_position, _("POPUP POSITION"), _(MenuMessages::UI_POPUP_POSITION_HELP_MSG));

    //help popup time 0=no popup
    auto help_popup_time = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 1.f, "s");
    help_popup_time->setSlider((float) (Settings::Instance().HelpPopupTime()));
    PopupGui->addWithLabel(help_popup_time, _("HELP POPUP DURATION"), _(MenuMessages::UI_HELP_POPUP_DURATION_HELP_MSG));

    //music popup time 0=no popup
    auto music_popup_time = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 1.f, "s");
    music_popup_time->setSlider((float) (Settings::Instance().MusicPopupTime()));
    PopupGui->addWithLabel(music_popup_time, _("MUSIC POPUP DURATION"), _(MenuMessages::UI_MUSIC_POPUP_DURATION_HELP_MSG));

    //netplay popup time 0=no popup
    /*auto netplay_popup_time = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 1.f, "s");
    netplay_popup_time->setValue((float) (Settings::Instance().getInt("NetplayPopupTime")));
    PopupGui->addWithLabel(netplay_popup_time, _("NETPLAY POPUP DURATION"), _(MenuMessages::UI_NETPLAY_POPUP_DURATION_HELP_MSG));*/

    PopupGui->addSaveFunc([help_popup_time, music_popup_time, popup_position] {
      Settings::Instance().SetHelpPopupTime(Math::roundi(help_popup_time->getSlider()));
      Settings::Instance().SetMusicPopupTime(Math::roundi(music_popup_time->getSlider()));
      //Settings::Instance().setInt("NetplayPopupTime", Math::roundi(netplay_popup_time->getValue()));
      Settings::Instance().SetPopupPosition(popup_position->getSelected());
    });
    mWindow.pushGui(PopupGui);
  };
  s->addSubMenu(_("POPUP SETTINGS"), openGuiSub, _(MenuMessages::UI_POPUP_HELP_MSG));

  // quick system select (left/right in game list view)
  auto quick_sys_select = std::make_shared<SwitchComponent>(mWindow);
  quick_sys_select->setState(Settings::Instance().QuickSystemSelect());
  s->addWithLabel(quick_sys_select, _("QUICK SYSTEM SELECT"), _(MenuMessages::UI_QUICK_HELP_MSG));
  s->addSaveFunc([quick_sys_select] {
    Settings::Instance().SetQuickSystemSelect(quick_sys_select->getState());
  });

  // Enable OSK (On-Screen-Keyboard)
  auto osk_enable = std::make_shared<SwitchComponent>(mWindow);
  osk_enable->setState(Settings::Instance().UseOSK());
  s->addWithLabel(osk_enable, _("ON SCREEN KEYBOARD"), _(MenuMessages::UI_KEYBOARD_HELP_MSG));
  s->addSaveFunc([osk_enable] {
    Settings::Instance().SetUseOSK(osk_enable->getState());
  });

  std::function<void()> openGuiTheme = [this] {
		auto st = new GuiSettings(mWindow, _("THEME").c_str());

  // carousel transition option
  auto move_carousel = std::make_shared<SwitchComponent>(mWindow);
  move_carousel->setState(Settings::Instance().MoveCarousel());
  st->addWithLabel(move_carousel, _("CAROUSEL ANIMATION"), _(MenuMessages::UI_CAROUSEL_HELP_MSG));
  st->addSaveFunc([move_carousel] { Settings::Instance().SetMoveCarousel(move_carousel->getState()); });

  // transition style
  auto transition_style = std::make_shared<OptionListComponent<std::string> >(mWindow, _("TRANSITION STYLE"), false);
  std::vector<std::string> transitions;
  transitions.push_back("fade");
  transitions.push_back("slide");
  transitions.push_back("instant");
  for (auto & transition : transitions)
    transition_style->add(transition, transition, Settings::Instance().TransitionStyle() == transition);
  st->addWithLabel(transition_style, _("TRANSITION STYLE"), _(MenuMessages::UI_TRANSITION_HELP_MSG));
  st->addSaveFunc([transition_style] {
    Settings::Instance().SetTransitionStyle(transition_style->getSelected());
  });

  // theme set
  auto themeSets = ThemeData::getThemeSets();

  auto selectedSet = themeSets.find(Settings::Instance().ThemeSet());
  if (selectedSet == themeSets.end())
    selectedSet = themeSets.begin();

  auto theme_set = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME SET"), false);
  for (auto it = themeSets.begin(); it != themeSets.end(); it++)
    theme_set->add(it->first, it->first, it == selectedSet);
  st->addWithLabel(theme_set, _("THEME SET"), _(MenuMessages::UI_THEME_HELP_MSG));

  std::function<void()> ReloadAll = [] () {
    ViewController::Instance().deleteAndReloadAll();
    MenuThemeData::getInstance();
    auto transi = ThemeData::getCurrent().getTransition();
    if (!transi.empty())
      Settings::Instance().SetTransitionStyle(transi);
    Settings::Instance().SetThemeChanged(false);
  };


  st->addSaveFunc([theme_set, ReloadAll] {

    if (Settings::Instance().ThemeSet() != theme_set->getSelected())
    {

      Settings::Instance().SetThemeSet(theme_set->getSelected());

      auto themeSubSets = ThemeData::getThemeSubSets(theme_set->getSelected());
      auto themeColorSets = ThemeData::sortThemeSubSets(themeSubSets, "colorset");
      auto themeIconSets = ThemeData::sortThemeSubSets(themeSubSets, "iconset");
      auto themeMenus = ThemeData::sortThemeSubSets(themeSubSets, "menu");
      auto themeSystemviewSets = ThemeData::sortThemeSubSets(themeSubSets, "systemview");
      auto themeGamelistViewSets = ThemeData::sortThemeSubSets(themeSubSets, "gamelistview");
      auto themeRegions = ThemeData::sortThemeSubSets(themeSubSets, "region");

      // theme changed without setting options, forcing options to avoid crash/blank theme
      if (themeRegions.empty())
        Settings::Instance().SetThemeRegionName("");
      else
        Settings::Instance().SetThemeRegionName(themeRegions.begin()->first);

      if (themeColorSets.empty())
        Settings::Instance().SetThemeColorSet("");
      else
        Settings::Instance().SetThemeColorSet(themeColorSets.begin()->first);

      if (themeIconSets.empty())
        Settings::Instance().SetThemeIconSet("");
      else
        Settings::Instance().SetThemeIconSet(themeIconSets.begin()->first);

      if (themeMenus.empty())
        Settings::Instance().SetThemeMenu("");
      else
        Settings::Instance().SetThemeMenu(themeMenus.begin()->first);

      if (themeSystemviewSets.empty())
        Settings::Instance().SetThemeSystemView("");
      else
        Settings::Instance().SetThemeSystemView(themeSystemviewSets.begin()->first);

      if (themeGamelistViewSets.empty())
        Settings::Instance().SetThemeGamelistView("");
      else
        Settings::Instance().SetThemeGamelistView(themeGamelistViewSets.begin()->first);

      Settings::Instance().SetThemeChanged(true);

      //reload theme:

      ReloadAll();
    }
  });


  // theme config
  std::function<void()> openGui = [this, theme_set, ReloadAll] {
    auto themeconfig = new GuiSettings(mWindow, _("THEME CONFIGURATION").c_str());

    auto SelectedTheme = theme_set->getSelected();

    auto themeSubSets = ThemeData::getThemeSubSets(SelectedTheme);
    auto themeColorSets = ThemeData::sortThemeSubSets(themeSubSets, "colorset");
    auto themeIconSets = ThemeData::sortThemeSubSets(themeSubSets, "iconset");
    auto themeMenus = ThemeData::sortThemeSubSets(themeSubSets, "menu");
    auto themeSystemviewSets = ThemeData::sortThemeSubSets(themeSubSets, "systemview");
    auto themeGamelistViewSets = ThemeData::sortThemeSubSets(themeSubSets, "gamelistview");
    auto themeRegions = ThemeData::sortThemeSubSets(themeSubSets, "region");

    // colorset

    auto selectedColorSet = themeColorSets.find(Settings::Instance().ThemeColorSet());
    if (selectedColorSet == themeColorSets.end())
      selectedColorSet = themeColorSets.begin();
    auto theme_colorset = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME COLORSET"), false);

    for (auto it = themeColorSets.begin(); it != themeColorSets.end(); it++)
      theme_colorset->add(it->first, it->first, it == selectedColorSet);

    if (!themeColorSets.empty())
      themeconfig->addWithLabel(theme_colorset, _("THEME COLORSET"), _(MenuMessages::UI_THEME_COLORSET_MSG));

    // iconset

    auto selectedIconSet = themeIconSets.find(Settings::Instance().ThemeIconSet());
    if (selectedIconSet == themeIconSets.end())
      selectedIconSet = themeIconSets.begin();
    auto theme_iconset = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME ICONSET"), false);

    for (auto it = themeIconSets.begin(); it != themeIconSets.end(); it++)
      theme_iconset->add(it->first, it->first, it == selectedIconSet);

    if (!themeIconSets.empty())
      themeconfig->addWithLabel(theme_iconset, _("THEME ICONSET"), _(MenuMessages::UI_THEME_ICONSET_MSG));

    // menu

    auto selectedMenu = themeMenus.find(Settings::Instance().ThemeMenu());
    if (selectedMenu == themeMenus.end())
      selectedMenu = themeMenus.begin();
    auto theme_menu = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME MENU"), false);

    for (auto it = themeMenus.begin(); it != themeMenus.end(); it++)
      theme_menu->add(it->first, it->first, it == selectedMenu);

    if (!themeMenus.empty())
      themeconfig->addWithLabel(theme_menu, _("THEME MENU"), _(MenuMessages::UI_THEME_MENU_MSG));


    // systemview

    auto selectedSystemviewSet = themeSystemviewSets.find(Settings::Instance().ThemeSystemView());
    if (selectedSystemviewSet == themeSystemviewSets.end())
      selectedSystemviewSet = themeSystemviewSets.begin();

    auto theme_systemview = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME SYSTEMVIEW"), false);

    for (auto it = themeSystemviewSets.begin(); it != themeSystemviewSets.end(); it++)
      theme_systemview->add(it->first, it->first, it == selectedSystemviewSet);

    if (!themeSystemviewSets.empty())
      themeconfig->addWithLabel(theme_systemview, _("THEME SYSTEMVIEW"), _(MenuMessages::UI_THEME_SYSTEMVIEW_MSG));

    // gamelistview

    auto selectedGamelistViewSet = themeGamelistViewSets.find(Settings::Instance().ThemeGamelistView());
    if (selectedGamelistViewSet == themeGamelistViewSets.end())
      selectedGamelistViewSet = themeGamelistViewSets.begin();

    auto theme_gamelistview = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME GAMELISTVIEW"), false);

    for (auto it = themeGamelistViewSets.begin(); it != themeGamelistViewSets.end(); it++)
      theme_gamelistview->add(it->first, it->first, it == selectedGamelistViewSet);

    if (!themeGamelistViewSets.empty())
      themeconfig->addWithLabel(theme_gamelistview, _("THEME GAMELISTVIEW"), _(MenuMessages::UI_THEME_GAMELISTVIEW_MSG));

    // themeregion

    auto selectedRegion = themeRegions.find(Settings::Instance().ThemeRegionName());
    if (selectedRegion == themeRegions.end())
      selectedRegion = themeRegions.begin();

    auto theme_region = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME GAMELISTVIEW"), false);

    for (auto it = themeRegions.begin(); it != themeRegions.end(); it++)
      theme_region->add(it->first, it->first, it == selectedRegion);

    if (!themeRegions.empty())
      themeconfig->addWithLabel(theme_region, _("THEME REGION"), _(MenuMessages::UI_THEME_REGION_MSG));


    themeconfig->addSaveFunc([theme_set, theme_colorset, theme_iconset, theme_menu, theme_systemview, theme_gamelistview, theme_region, ReloadAll] {
      bool needReload = false;
      if (Settings::Instance().ThemeColorSet() != theme_colorset->getSelected() && !theme_colorset->getSelected().empty())
        needReload = true;
      if (Settings::Instance().ThemeIconSet() != theme_iconset->getSelected() && !theme_iconset->getSelected().empty())
        needReload = true;
      if (Settings::Instance().ThemeMenu() != theme_menu->getSelected() && !theme_menu->getSelected().empty())
        needReload = true;
      if (Settings::Instance().ThemeSystemView() != theme_systemview->getSelected() && !theme_systemview->getSelected().empty())
        needReload = true;
      if (Settings::Instance().ThemeGamelistView() != theme_gamelistview->getSelected() && !theme_gamelistview->getSelected().empty())
        needReload = true;
      if (Settings::Instance().ThemeRegionName() != theme_region->getSelected() && !theme_region->getSelected().empty())
        needReload = true;

      if (needReload){
        Settings::Instance().SetThemeSet(theme_set->getSelected());
        Settings::Instance().SetThemeColorSet(theme_colorset->getSelected());
        Settings::Instance().SetThemeIconSet(theme_iconset->getSelected());
        Settings::Instance().SetThemeMenu(theme_menu->getSelected());
        Settings::Instance().SetThemeSystemView(theme_systemview->getSelected());
        Settings::Instance().SetThemeGamelistView(theme_gamelistview->getSelected());
        Settings::Instance().SetThemeRegionName(theme_region->getSelected());
        Settings::Instance().SetThemeChanged(true);
        //reload theme
        ReloadAll();
      }
    });
    if (!themeRegions.empty() || !themeGamelistViewSets.empty() || !themeSystemviewSets.empty() || !themeIconSets.empty()
          || !themeMenus.empty() || !themeColorSets.empty())
      mWindow.pushGui(themeconfig);
    else
      mWindow.pushGui(new GuiMsgBox(mWindow, _("THIS THEME HAS NO OPTION"), _("OK")));
  };
  st->addSubMenu(_("THEME CONFIGURATION"), openGui, _(MenuMessages::UI_THEME_CONFIGURATION_MSG));

  st->addSaveFunc([] {
    RecalboxConf::Instance().SaveRecalboxConf();
  });
		mWindow.pushGui(st);
	};

	s->addSubMenu(_("THEME"), openGuiTheme, _(MenuMessages::UI_THEME_HELP_MSG));

	// Game List Update
	s->addSubMenu(_("UPDATE GAMES LISTS"), [this] {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY UPDATE GAMES LISTS ?"), _("YES"), [] {
      ViewController::Instance().deleteAndReloadAll();
		}, _("NO"), nullptr));
	}, _(MenuMessages::UI_UPDATE_GAMELIST_HELP_MSG));

  mWindow.pushGui(s);
}

void GuiMenu::menuSoundSettings(){
  auto s = new GuiSettings(mWindow, _("SOUND SETTINGS").c_str());

  // volume
  auto setVolume = [](const float &newVal) {
    VolumeControl::getInstance()->setVolume(Math::roundi(newVal));
  };
  auto volume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
  volume->setSlider((float) VolumeControl::getInstance()->getVolume());
  volume->setSelectedChangedCallback(setVolume);
  s->addWithLabel(volume, _("SYSTEM VOLUME"), _(MenuMessages::SOUND_VOLUME_HELP_MSG));

  // disable sounds
  auto sounds_enabled = std::make_shared<SwitchComponent>(mWindow);
  sounds_enabled->setState(RecalboxConf::Instance().AsBool("audio.bgmusic"));
  s->addWithLabel(sounds_enabled, _("FRONTEND MUSIC"), _(MenuMessages::SOUND_FRONTEND_MUSIC_HELP_MSG));

  // audio device
  auto optionsAudio = std::make_shared<OptionListComponent<std::string> >(mWindow, _("OUTPUT DEVICE"),
                                      false, FONT_SIZE_EXTRASMALL);
  std::string currentDevice = RecalboxConf::Instance().AsString("audio.device");
  if (currentDevice.empty()) currentDevice = "auto";

  std::vector<std::string> availableAudio = RecalboxSystem::getAvailableAudioOutputDevices();
  std::string selectedAudio = RecalboxSystem::getCurrentAudioOutputDevice();

  if (RecalboxConf::Instance().AsString("emulationstation.menu") != "bartop") {
    for (auto& it : availableAudio)
    {
      std::vector<std::string> tokens = Strings::Split(it, ' ');

      if(tokens.size()>= 8){
        std::string vname;
        for (unsigned int i=0; i<8; i++) {
          vname += ' ';
          vname += tokens.at(i);
        }
        optionsAudio->add(vname, it, selectedAudio == it);
      } else {
        optionsAudio->add(it, it, selectedAudio == it);
      }
    }
    auto setAudioDevice = [](const std::string &newVal) {
      RecalboxSystem::setAudioOutputDevice(newVal);
    };
    optionsAudio->setSelectedChangedCallback(setAudioDevice);
    s->addWithLabel(optionsAudio, _("OUTPUT DEVICE"));
  }
  s->addSaveFunc([optionsAudio, currentDevice, sounds_enabled, volume] {
    RecalboxConf::Instance().SetInt("audio.volume", Math::roundi(volume->getSlider()));

    RecalboxConf::Instance().SetBool("audio.bgmusic", sounds_enabled->getState());
    if (!sounds_enabled->getState())
      AudioManager::Instance().StopAll();
    if (currentDevice != optionsAudio->getSelected()) {
      RecalboxConf::Instance().SetString("audio.device", optionsAudio->getSelected());
    }
    RecalboxConf::Instance().SaveRecalboxConf();
  });

  mWindow.pushGui(s);
}

void GuiMenu::menuNetworkSettings(){
  auto s = new GuiSettings(mWindow, _("NETWORK SETTINGS").c_str());
  auto status = std::make_shared<TextComponent>(mWindow, RecalboxSystem::ping() ? _( "CONNECTED")
      : _("NOT CONNECTED"), mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  s->addWithLabel(status, _("STATUS"), _(MenuMessages::NETWORK_STATUS_HELP_MSG));
  auto ip = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getIpAdress(),
      mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  s->addWithLabel(ip, _("IP ADDRESS"), _(MenuMessages::NETWORK_IP_HELP_MSG));

  // Hostname
  createInputTextRow(s, _("HOSTNAME"), "system.hostname", false, _(MenuMessages::NETWORK_HOST_HELP_MSG));

  // Wifi enable
  auto enable_wifi = std::make_shared<SwitchComponent>(mWindow);
  bool baseEnabled = RecalboxConf::Instance().AsBool("wifi.enabled");
  enable_wifi->setState(baseEnabled);
  s->addWithLabel(enable_wifi, _("ENABLE WIFI"), _(MenuMessages::NETWORK_WIFI_HELP_MSG));

  //SSID
  std::string baseSSID = RecalboxConf::Instance().AsString("wifi.ssid");
  auto WifiSSID = [this, baseSSID, enable_wifi, baseEnabled] (GuiSettings *gui, const std::string& title,
      const std::string& value, const std::string& help = "") {

    ComponentListRow row;

    auto lbl = std::make_shared<TextComponent>(mWindow, title, mMenuTheme->menuText.font, mMenuTheme->menuText.color);
    row.addElement(lbl, true); // label

    std::shared_ptr<GuiComponent> ed;

    ed = std::make_shared<TextComponent>(mWindow, value, mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Right);
    row.addElement(ed, true);

    auto spacer = std::make_shared<GuiComponent>(mWindow);
    spacer->setSize(Renderer::getDisplayWidthAsFloat() * 0.005f, 0);
    row.addElement(spacer, false);

    auto bracket = std::make_shared<ImageComponent>(mWindow);
    bracket->setImage(Path(":/arrow.svg"));
    bracket->setResize(Vector2f(0, lbl->getFont()->getLetterHeight()));
    row.addElement(bracket, false);

    auto updateVal = [ed](const std::string &newVal) {
      ed->setValue(newVal);
    };

    row.makeAcceptInputHandler([this, updateVal, enable_wifi, baseEnabled] {

      GuiSettings *SSID = new GuiSettings(mWindow, _("WIFI SSID").c_str());
      std::shared_ptr<GuiComponent> ed;
      ComponentListRow row;
      ed = std::make_shared<TextComponent>(mWindow, _("MANUAL INPUT"), mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Left);
      row.addElement(ed, true);
      auto updateValue = [updateVal, SSID](const std::string &newVal) {
        RecalboxConf::Instance().SetString("wifi.ssid", newVal);
        updateVal(newVal);
        delete SSID;
      };
      row.makeAcceptInputHandler([this, updateValue] {
        if (Settings::Instance().UseOSK())
          mWindow.pushGui(new GuiTextEditPopupKeyboard(mWindow, "", "", updateValue, false));
        else
          mWindow.pushGui(new GuiTextEditPopup(mWindow, "", "", updateValue, false));
      });
      SSID->addRowWithHelp(row, _("MANUAL INPUT"), MenuMessages::NETWORK_MANUAL_INPUT_HELP_MSG);
      if (enable_wifi->getState()) {
        std::vector<std::string> availableSSID = RecalboxSystem::getAvailableWiFiSSID(baseEnabled);
        RecalboxConf::Instance().SetBool("wifi.enabled", true);
        for (auto & it : availableSSID) {

          if (it != "\n") {

            row.elements.clear();
            std::vector<std::string> tokens = Strings::Split(it, ' ');

            if (tokens.size() >= 8) {
              std::string vname;
              for (unsigned int i = 0; i < 8; i++) {
                vname += ' ';
                vname += tokens.at(i);
              }
              ed = std::make_shared<TextComponent>(mWindow, vname, mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Left);
            } else {
              ed = std::make_shared<TextComponent>(mWindow, it, mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Left);
            }
            row.addElement(ed, true);
            row.makeAcceptInputHandler([updateValue, ed] { updateValue(ed->getValue()); });
            SSID->addRow(row);
          }
        }
      }
      mWindow.pushGui(SSID);
    });

    gui->addRowWithHelp(row, title, help);
  };
  WifiSSID(s, _("WIFI SSID"), baseSSID, _(MenuMessages::NETWORK_SSID_HELP_MSG));

  const std::string baseKEY = RecalboxConf::Instance().AsString("wifi.key");
  createInputTextRow(s, _("WIFI KEY"), "wifi.key", true, _(MenuMessages::NETWORK_KEY_HELP_MSG));

  s->addSaveFunc([baseEnabled, baseSSID, baseKEY, enable_wifi, this] {
    bool wifienabled = enable_wifi->getState();

    std::string newSSID = RecalboxConf::Instance().AsString("wifi.ssid");
    std::string newKey = RecalboxConf::Instance().AsString("wifi.key");

    if (wifienabled) {
      if (baseSSID != newSSID
        || baseKEY != newKey
        || !baseEnabled) {
        if (RecalboxSystem::enableWifi(newSSID, newKey)) {
          mWindow.pushGui(new GuiMsgBox(mWindow, _("WIFI ENABLED"))
          );
        } else {
          mWindow.pushGui(new GuiMsgBox(mWindow, _("WIFI CONFIGURATION ERROR"))
          );
        }
      }
    }
    else if (baseEnabled || RecalboxConf::Instance().AsBool("wifi.enabled")){
      RecalboxSystem::disableWifi();
    }

    RecalboxConf::Instance().SetBool("wifi.enabled", wifienabled);
    RecalboxConf::Instance().SaveRecalboxConf();
    RecalboxSystem::backupRecalboxConf();
  });
  mWindow.pushGui(s);
}

void GuiMenu::menuScrapper(){

  auto openScrapeNow = [this] { mWindow.pushGui(new GuiScraperStart(mWindow, mSystemManager)); };
  auto s = new GuiSettings(mWindow, _("SCRAPER").c_str());

  // scrape from
  auto scraper_list = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCRAPE FROM"), false);
  std::vector<std::string> scrapers = getScraperList();
  for (auto& scraper : scrapers)
    scraper_list->add(scraper, scraper, scraper == Settings::Instance().Scraper());

  s->addWithLabel(scraper_list, _("SCRAPE FROM"), _(MenuMessages::SCRAPER_FROM_HELP_MSG));
  s->addSaveFunc([scraper_list] {
    Settings::Instance().SetScraper(scraper_list->getSelected());
  });

  // scrape ratings
  auto scrape_ratings = std::make_shared<SwitchComponent>(mWindow);
  scrape_ratings->setState(Settings::Instance().ScrapeRatings());
  s->addWithLabel(scrape_ratings, _("SCRAPE RATINGS"), _(MenuMessages::SCRAPER_RATINGS_HELP_MSG));
  s->addSaveFunc([scrape_ratings] {
    Settings::Instance().SetScrapeRatings(scrape_ratings->getState());
  });

  // scrape now
  std::function<void()> openAndSave = openScrapeNow;
  openAndSave = [s, openAndSave] {
    s->save();
    openAndSave();
  };
  s->addSubMenu(_("SCRAPE NOW"), openAndSave, _(MenuMessages::SCRAPER_NOW_HELP_MSG));

  mWindow.pushGui(s);
}

void GuiMenu::menuAdvancedSettings(){

  auto s = new GuiSettings(mWindow, _("ADVANCED SETTINGS").c_str());

  // Overclock choice
  auto overclock_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, _("OVERCLOCK"), false);
  const std::string& currentOverclock = Settings::Instance().Overclock();
  switch(getRaspberryVersion())
  {
    case RaspberryGeneration::Pi1:
    {
      overclock_choice->add(_("EXTREM (1100Mhz)"), "extrem", currentOverclock == "extrem");
      overclock_choice->add(_("TURBO (1000Mhz)"), "turbo", currentOverclock == "turbo");
      overclock_choice->add(_("HIGH (950Mhz)"), "high", currentOverclock == "high");
      overclock_choice->add(_("NONE"), "none", currentOverclock == "none");
      break;
    }
    case RaspberryGeneration::Pi2:
    {
      overclock_choice->add(_("EXTREM (1100Mhz)"), "rpi2-extrem", currentOverclock == "rpi2-extrem");
      overclock_choice->add(_("TURBO (1050Mhz)+"), "rpi2-turbo", currentOverclock == "rpi2-turbo");
      overclock_choice->add(_("HIGH (1050Mhz)"), "rpi2-high", currentOverclock == "rpi2-high");
      overclock_choice->add(_("NONE (900Mhz)"), "none", currentOverclock == "none");
      break;
    }
    case RaspberryGeneration::Pi3:
    {
      overclock_choice->add(_("EXTREM (1375Mhz)"), "rpi3-extrem", currentOverclock == "rpi3-extrem");
      overclock_choice->add(_("TURBO (1350Mhz)"), "rpi3-turbo", currentOverclock == "rpi3-turbo");
      overclock_choice->add(_("HIGH (1300Mhz)"), "rpi3-high", currentOverclock == "rpi3-high");
      overclock_choice->add(_("NONE (1200Mhz)"), "none", currentOverclock == "none");
      break;
    }
    case RaspberryGeneration::Pi3plus:
    {
      overclock_choice->add(_("EXTREM (1500Mhz)"), "rpi3plus-extrem", currentOverclock == "rpi3plus-extrem");
      overclock_choice->add(_("TURBO (1450Mhz)"), "rpi3plus-turbo", currentOverclock == "rpi3plus-turbo");
      overclock_choice->add(_("HIGH (1425Mhz)"), "rpi3plus-high", currentOverclock == "rpi3plus-high");
      overclock_choice->add(_("NONE (1400Mhz)"), "none", currentOverclock == "none");
      break;
    }
    case RaspberryGeneration::Pi0:
    case RaspberryGeneration::Pi4:
    case RaspberryGeneration::NotRaspberry:
    case RaspberryGeneration::NotYetKnown:
    case RaspberryGeneration::UndetectedYet:
    default:
    {
      overclock_choice->add(_("NONE"), "none", true);
      break;
    }
  }

  std::vector<std::string> overclockWarning = {
    "turbo",
    "extrem",
    "rpi2-turbo",
    "rpi2-extrem",
    "rpi3-turbo",
    "rpi3-extrem",
    "rpi3plus-turbo",
    "rpi3plus-extrem"
  };

  s->addWithLabel(overclock_choice, _("OVERCLOCK"), _(MenuMessages::ADVANCED_OVERCLOCK_HELP_MSG));
  s->addSaveFunc([overclock_choice, overclockWarning, this]  {
    bool reboot = false;
    if (Settings::Instance().Overclock() != overclock_choice->getSelected()) {
      Settings::Instance().SetOverclock(overclock_choice->getSelected());
      RecalboxSystem::setOverclock(overclock_choice->getSelected());
      reboot = true;
    }
    RecalboxConf::Instance().SaveRecalboxConf();
    if (reboot) {
      if (std::find(overclockWarning.begin(), overclockWarning.end(), overclock_choice->getSelected()) != overclockWarning.end()) {
        mWindow.pushGui(
            new GuiMsgBox(mWindow, _("TURBO AND EXTREM OVERCLOCK PRESETS MAY CAUSE SYSTEM UNSTABILITIES, SO USE THEM AT YOUR OWN RISK.\nIF YOU CONTINUE, THE SYSTEM WILL REBOOT NOW."),
                _("YES"), [] {
                      if (runRestartCommand() != 0) {
                        LOG(LogWarning) << "Reboot terminated with non-zero result!";
                      }
                    }, _("NO"), [] {
                  Settings::Instance().SetOverclock("none");
                  RecalboxSystem::setOverclock("none");
                  Settings::Instance().saveFile();
                }));
      } else {
        mWindow.pushGui(
            new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [] {
                      if (runRestartCommand() != 0) {
                        LOG(LogWarning) << "Reboot terminated with non-zero result!";
                      }
                    }));
      }
    }
  });

  // BOOT
  {
    std::function<void()> openGui = [this] {
      GuiSettings *bootGui = new GuiSettings(mWindow, _("BOOT SETTINGS").c_str());
      auto kodiAtStart = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("kodi.atstartup"));
      bootGui->addWithLabel(kodiAtStart, _("KODI AT START"), _(MenuMessages::ADVANCED_KODI_AT_START_HELP_MSG));
      // Gamelists only
      bool gamelistOnly = RecalboxConf::Instance().AsBool("emulationstation.gamelistonly");
      auto gamelistOnlyComp = std::make_shared<SwitchComponent>(mWindow, gamelistOnly);
      bootGui->addWithLabel(gamelistOnlyComp, _("GAMELIST ONLY"), _(MenuMessages::ADVANCED_GAMELISTONLY_HELP_MSG));

      // Selected System
      std::string selectedsystem = RecalboxConf::Instance().AsString("emulationstation.selectedsystem");
      auto system_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("BOOT ON SYSTEM"), false);
      std::string currentSystem = !selectedsystem.empty() ? selectedsystem : "favorites";
      // For each activated system
      std::vector<SystemData *> systems = mSystemManager.GetVisibleSystemList();
      bool found = false;
      for (auto & system : systems) {
        const std::string& systemName = system->getName();
        if(systemName != "favorites") {
          found = found || currentSystem == systemName;
          system_choices->add(systemName, systemName, currentSystem == systemName);
        }
      }
      // Always add favorites, and select it if the user never chose another system
      system_choices->add("favorites", "favorites", !found);

      bootGui->addWithLabel(system_choices, _("BOOT ON SYSTEM"), _(MenuMessages::ADVANCED_BOOT_ON_SYSTEM_HELP_MSG));
      // Boot on gamelist
      bool bootOnGamelist = RecalboxConf::Instance().AsBool("emulationstation.bootongamelist");
      auto bootOnGamelistComp = std::make_shared<SwitchComponent>(mWindow, bootOnGamelist);
      bootGui->addWithLabel(bootOnGamelistComp, _("BOOT ON GAMELIST"), _(MenuMessages::ADVANCED_BOOTGAMELIST_HELP_MSG));
      // Hide system view
      bool hidesystemview = RecalboxConf::Instance().AsBool("emulationstation.hidesystemview");
      auto hidesystemviewComp = std::make_shared<SwitchComponent>(mWindow, hidesystemview);
      bootGui->addWithLabel(hidesystemviewComp, _("HIDE SYSTEM VIEW"), _(MenuMessages::ADVANCED_HIDESYSTEMVIEW_HELP_MSG));

      // Force Basicgamelist View
      bool basicgamelistview =
          RecalboxConf::Instance().AsBool("emulationstation.forcebasicgamelistview");
      auto basicgamelistviewComp = std::make_shared<SwitchComponent>(mWindow, basicgamelistview);
      bootGui->addWithLabel(basicgamelistviewComp, _("FORCE BASIC GAMELIST VIEW"), _(MenuMessages::ADVANCED_BASICGAMELISTVIEW_HELP_MSG));

      bootGui->addSaveFunc(
          [gamelistOnlyComp, system_choices, kodiAtStart, bootOnGamelistComp, hidesystemviewComp, basicgamelistviewComp] {
            RecalboxConf::Instance().SetBool("kodi.atstartup", kodiAtStart->getState());
            RecalboxConf::Instance().SetBool("emulationstation.gamelistonly", gamelistOnlyComp->getState());
            RecalboxConf::Instance().SetString("emulationstation.selectedsystem", system_choices->getSelected());
            RecalboxConf::Instance().SetBool("emulationstation.bootongamelist", bootOnGamelistComp->getState());
            RecalboxConf::Instance().SetBool("emulationstation.hidesystemview", hidesystemviewComp->getState());
            RecalboxConf::Instance().SetBool("emulationstation.forcebasicgamelistview", basicgamelistviewComp->getState());
            RecalboxConf::Instance().SaveRecalboxConf();
          });
      mWindow.pushGui(bootGui);
    };

    s->addSubMenu(_("BOOT SETTINGS"), openGui, _(MenuMessages::ADVANCED_BOOT_HELP_MSG));
  }

  // Custom config for systems
  {
    std::function<void()> openGuiD = [this, s] {
      s->save();
      GuiSettings *configuration = new GuiSettings(mWindow, _("EMULATOR ADVANCED CONFIGURATION").c_str());
      // For each activated system
      std::vector<SystemData *> systems = mSystemManager.GetAllSystemList();
      for (auto& system : systems)
      {
        if (system != mSystemManager.FavoriteSystem()) {
          SystemData *systemData = system;
          configuration->addSubMenu(system->getFullName(), [this, systemData] {
            popSystemConfigurationGui(systemData);
          });
        }
      }
      mWindow.pushGui(configuration);

    };
    // Advanced button
    s->addSubMenu(_("ADVANCED EMULATOR CONFIGURATION"), openGuiD, _(MenuMessages::ADVANCED_EMULATOR_ADVANCED_HELP_MSG));
  }


  //Kodi
  {
    std::function<void()> openGui = [this] {
      GuiSettings *kodiGui = new GuiSettings(mWindow, _("KODI SETTINGS").c_str());
      auto kodiEnabled = std::make_shared<SwitchComponent>(mWindow);
      kodiEnabled->setState(RecalboxConf::Instance().AsBool("kodi.enabled"));
      kodiGui->addWithLabel(kodiEnabled, _("ENABLE KODI"), _(MenuMessages::ADVANCED_KODI_ENABLE_HELP_MSG));
      auto kodiAtStart = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("kodi.atstartup"));
      kodiGui->addWithLabel(kodiAtStart, _("KODI AT START"), _(MenuMessages::ADVANCED_KODI_AT_START_HELP_MSG));
      auto kodiX = std::make_shared<SwitchComponent>(mWindow);
      kodiX->setState(RecalboxConf::Instance().AsBool("kodi.xbutton"));
      kodiGui->addWithLabel(kodiX, _("START KODI WITH X"), _(MenuMessages::ADVANCED_KODI_X_HELP_MSG));
      kodiGui->addSaveFunc([kodiEnabled, kodiAtStart, kodiX] {
        RecalboxConf::Instance().SetBool("kodi.enabled", kodiEnabled->getState());
        RecalboxConf::Instance().SetBool("kodi.atstartup", kodiAtStart->getState());
        RecalboxConf::Instance().SetBool("kodi.xbutton", kodiX->getState());
        RecalboxConf::Instance().SaveRecalboxConf();
      });
      mWindow.pushGui(kodiGui);
    };
    s->addSubMenu(_("KODI SETTINGS"), openGui, _(MenuMessages::ADVANCED_KODI_HELP_MSG));
  }

  //Security
  {
    std::function<void()> openGui = [this] {
      GuiSettings *securityGui = new GuiSettings(mWindow, _("SECURITY").c_str());
      auto securityEnabled = std::make_shared<SwitchComponent>(mWindow);
      securityEnabled->setState(
          RecalboxConf::Instance().AsBool("system.security.enabled"));
      securityGui->addWithLabel(securityEnabled, _("ENFORCE SECURITY"), _(MenuMessages::ADVANCED_ENFORCE_SECURITY_HELP_MSG));

      auto rootpassword = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getRootPassword(),
          mMenuTheme->menuText.font, mMenuTheme->menuText.color);
      securityGui->addWithLabel(rootpassword, _("ROOT PASSWORD"), _(MenuMessages::ADVANCED_ROOT_PWD_HELP_MSG));

      securityGui->addSaveFunc([this, securityEnabled] {
        bool reboot = false;

        if (securityEnabled->changed()) {
          RecalboxConf::Instance().SetBool("system.security.enabled", securityEnabled->getState());
          RecalboxConf::Instance().SaveRecalboxConf();
          reboot = true;
        }

        if (reboot) {
          mWindow.pushGui(
            new GuiMsgBox(mWindow, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [] {
              if (runRestartCommand() != 0) {
                LOG(LogWarning) << "Reboot terminated with non-zero result!";
              }
            })
          );
        }
      });
      mWindow.pushGui(securityGui);
    };
    s->addSubMenu(_("SECURITY"), openGui, _(MenuMessages::ADVANCED_SECURITY_HELP_MSG));
  }
  // overscan
  auto overscan_enabled = std::make_shared<SwitchComponent>(mWindow, Settings::Instance().Overscan());
  s->addWithLabel(overscan_enabled, _("OVERSCAN"), _(MenuMessages::ADVANCED_OVERSCAN_HELP_MSG));
  s->addSaveFunc([overscan_enabled] {
    if (Settings::Instance().Overscan() != overscan_enabled->getState()) {
      Settings::Instance().SetOverscan(overscan_enabled->getState());
      RecalboxSystem::setOverscan(overscan_enabled->getState());
    }
  });

  // framerate
  auto framerate = std::make_shared<SwitchComponent>(mWindow, Settings::Instance().DrawFramerate());
  s->addWithLabel(framerate, _("SHOW FRAMERATE"), _(MenuMessages::ADVANCED_FRAMERATE_HELP_MSG));
  s->addSaveFunc([framerate] {
    Settings::Instance().SetDrawFramerate(framerate->getState());
  });

  // Recalbox Manager
  auto manager = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("system.manager.enabled"));
  s->addWithLabel(manager, _("RECALBOX MANAGER"), _(MenuMessages::ADVANCED_MANAGER_HELP_MSG));
  s->addSaveFunc([manager] {
    RecalboxConf::Instance().SetBool("system.manager.enabled", manager->getState());
    RecalboxConf::Instance().SaveRecalboxConf();
  });
  // Recalbox API
  auto recalboxApi = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::Instance().AsBool("system.api.enabled"));
  s->addWithLabel(recalboxApi, _("RECALBOX API"), _(MenuMessages::ADVANCED_API_HELP_MSG));
  s->addSaveFunc([recalboxApi] {
    RecalboxConf::Instance().SetBool("system.api.enabled", recalboxApi->getState());
    RecalboxConf::Instance().SaveRecalboxConf();
  });
  mWindow.pushGui(s);
}

void GuiMenu::menuQuit(){
  auto s = new GuiSettings(mWindow, _("QUIT").c_str());

  ComponentListRow row;

  row.makeAcceptInputHandler([this] {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN?"), _("YES"), [] {
      MainRunner::RequestQuit(MainRunner::ExitState::Shutdown);
        }, _("NO"), nullptr));
  });
  row.addElement(std::make_shared<TextComponent>(mWindow, _("SHUTDOWN SYSTEM"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRow(row);

  row.elements.clear();
  row.makeAcceptInputHandler([this] {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"), [] {
      MainRunner::RequestQuit(MainRunner::ExitState::FastShutdown);
        }, _("NO"), nullptr));
  });
  row.addElement(std::make_shared<TextComponent>(mWindow, _("FAST SHUTDOWN SYSTEM"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRow(row);

  row.elements.clear();
  row.makeAcceptInputHandler([this] {
    mWindow.pushGui(new GuiMsgBox(mWindow, _("REALLY RESTART?"), _("YES"), [] {
      MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot);
        }, _("NO"), nullptr));
  });
  row.addElement(std::make_shared<TextComponent>(mWindow, _("RESTART SYSTEM"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRow(row);

  mWindow.pushGui(s);
}

void GuiMenu::popSystemConfigurationGui(SystemData *systemData) const {

    // The system configuration
    GuiSettings *systemConfiguration = new GuiSettings(mWindow, systemData->getFullName().c_str());

    EmulatorDefaults emulatorDefaults = RecalboxSystem::getEmulatorDefaults(systemData->getName());

    //Emulator choice
    auto emu_choice = std::make_shared<OptionListComponent<std::string>>(mWindow, _("Emulator"), false);

    // Core choice
    auto core_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, _("Core"), false);

    std::string currentEmulator = RecalboxConf::Instance().AsString(systemData->getName() + ".emulator");

    emu_choice->add(Strings::Replace(_("DEFAULT (%1%)"), "%1%", emulatorDefaults.emulator), "default", true);

    for (int i = systemData->Emulators().Count(); --i >= 0;)
    {
      emu_choice->add(systemData->Emulators().At(i).Name(), systemData->Emulators().At(i).Name(),
                      systemData->Emulators().At(i).Name() == currentEmulator);
    }

    // when emulator changes, load new core list
    emu_choice->setSelectedChangedCallback([systemData, emulatorDefaults, core_choice](const std::string& emulatorName) {
        core_choice->clear();

        if (emulatorName == "default") {
            core_choice->add(Strings::Replace(_("DEFAULT (%1%)"), "%1%", emulatorDefaults.core), "default", true);
            return ;
        }
        
        const EmulatorDescriptor& emulator = systemData->Emulators().Named(emulatorName);
        std::string currentCore = RecalboxConf::Instance().AsString(systemData->getName() + ".core");

        if (currentCore.empty()) {
            currentCore = "default";
        }

        // update current core if it is not available in the emulator selected core list
        if (currentCore != "default" && emulator.HasCore(currentCore)) {
            if (emulatorName == emulatorDefaults.emulator) {
                currentCore = emulatorDefaults.core;
            } else {
                // use first one
                currentCore = emulator.Core(0);
            }
        }

        for (int i = 0; i < emulator.CoreCount(); ++i)
        {
            // select at least first one in case of bad entry in config file
            bool selected = currentCore == emulator.Core(i) || i == 0;
            if (currentCore == "default" && emulatorName == emulatorDefaults.emulator)
                selected = selected || (emulator.Core(i) == emulatorDefaults.core);
            core_choice->add(emulator.Core(i), emulator.Core(i), selected);
        }
    });

    systemConfiguration->addWithLabel(emu_choice, _("Emulator"), _(MenuMessages::ADVANCED_EMU_EMU_HELP_MSG));
    systemConfiguration->addWithLabel(core_choice, _("Core"), _(MenuMessages::ADVANCED_EMU_CORE_HELP_MSG));

    // force change event to load core list
    emu_choice->invalidate(); 

    // Screen ratio choice
    auto ratio_choice = createRatioOptionList(mWindow, systemData->getName());
    systemConfiguration->addWithLabel(ratio_choice, _("GAME RATIO"), _(MenuMessages::GAME_RATIO_HELP_MSG));
    // smoothing
    auto smoothing_enabled = std::make_shared<SwitchComponent>(mWindow);
    smoothing_enabled->setState(RecalboxConf::Instance().AsBool(systemData->getName() + ".smooth", RecalboxConf::Instance().AsBool("global.smooth")));
    systemConfiguration->addWithLabel(smoothing_enabled, _("SMOOTH GAMES"), _(MenuMessages::GAME_SMOOTH_HELP_MSG));
    // rewind
    auto rewind_enabled = std::make_shared<SwitchComponent>(mWindow);
    rewind_enabled->setState(RecalboxConf::Instance().AsBool(systemData->getName() + ".rewind", RecalboxConf::Instance().AsBool("global.rewind")));
    systemConfiguration->addWithLabel(rewind_enabled, _("REWIND"), _(MenuMessages::GAME_REWIND_HELP_MSG));
    // autosave
    auto autosave_enabled = std::make_shared<SwitchComponent>(mWindow);
    autosave_enabled->setState(RecalboxConf::Instance().AsBool(systemData->getName() + ".autosave", RecalboxConf::Instance().AsBool("global.autosave")));
    systemConfiguration->addWithLabel(autosave_enabled, _("AUTO SAVE/LOAD"), _(MenuMessages::GAME_AUTOSAVELOAD_HELP_MSG));


    systemConfiguration->addSaveFunc(
            [systemData, smoothing_enabled, rewind_enabled, ratio_choice, emu_choice, core_choice, autosave_enabled] {
                if (ratio_choice->changed()) {
                    RecalboxConf::Instance().SetString(systemData->getName() + ".ratio", ratio_choice->getSelected());
                }
                if (rewind_enabled->changed()) {
                    RecalboxConf::Instance().SetBool(systemData->getName() + ".rewind", rewind_enabled->getState());
                }
                if (smoothing_enabled->changed()) {
                    RecalboxConf::Instance().SetBool(systemData->getName() + ".smooth", smoothing_enabled->getState());
                }
                // always save both core and emulator
                // this is required to distinguish default
                if (emu_choice->changed() || core_choice->changed()) {
                    RecalboxConf::Instance().SetString(systemData->getName() + ".emulator", emu_choice->getSelected());
                    RecalboxConf::Instance().SetString(systemData->getName() + ".core", core_choice->getSelected());
                }
                if (autosave_enabled->changed()) {
                    RecalboxConf::Instance().SetBool(systemData->getName() + ".autosave", autosave_enabled->getState());
                }
                RecalboxConf::Instance().SaveRecalboxConf();
            });
    mWindow.pushGui(systemConfiguration);
}

void GuiMenu::onSizeChanged() {
    mVersion.setSize(mSize.x(), 0);
    mVersion.setPosition(0, mSize.y() - mVersion.getSize().y());
}

void GuiMenu::addEntryWithHelp(const char *name, const std::string& help, unsigned int color, bool add_arrow, const std::function<void()> &func, const Path& iconPath) {

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  std::shared_ptr<Font> font = menuTheme->menuText.font;
    // populate the list
    ComponentListRow row;

    if (!iconPath.Empty())
    {
        // icon
	    auto icon = std::make_shared<ImageComponent>(mWindow);
	    icon->setImage(iconPath);
	    icon->setColorShift(menuTheme->menuText.color);
	    icon->setResize(0, menuTheme->menuText.font->getLetterHeight() * 1.25f);
	    row.addElement(icon, false);


        // spacer between icon and text
        auto spacer = std::make_shared<GuiComponent>(mWindow);
        spacer->setSize(10, 0);
        row.addElement(spacer, false);
    }

    row.addElement(std::make_shared<TextComponent>(mWindow, name, font, color), true);

    if (add_arrow) {
        std::shared_ptr<ImageComponent> bracket = makeArrow(mWindow);
        row.addElement(bracket, false);
    }

    row.makeAcceptInputHandler(func);

    mMenu.addRowWithHelp(row, name, help);
}

void GuiMenu::addEntry(const char *name, unsigned int color, bool add_arrow, const std::function<void()> &func, const Path& iconPath) {
    addEntryWithHelp(name, "", color, add_arrow, func, iconPath);
}

bool GuiMenu::ProcessInput(const InputCompactEvent& event)
{
    if (GuiComponent::ProcessInput(event))
        return true;

    if (event.APressed() || event.StartPressed()) {
        Close();
        return true;
    }

    return false;
}

bool GuiMenu::getHelpPrompts(Help& help)
{
  mMenu.getHelpPrompts(help);
  help.Set(HelpType::UpDown, _("CHOOSE"))
      .Set(HelpType::B, _("SELECT"))
      .Set(HelpType::Start, _("CLOSE"));
  return true;
}

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createRatioOptionList(Window& window, const std::string& configname) const {
    auto ratio_choice = std::make_shared<OptionListComponent<std::string> >(window, _("GAME RATIO"), false);
    std::string currentRatio = RecalboxConf::Instance().AsString(configname + ".ratio");
    if (currentRatio.empty()) {
        currentRatio = std::string("auto");
    }

    std::map<std::string, std::string> *ratioMap = LibretroRatio::getInstance()->getRatio();
    for (auto ratio = ratioMap->begin(); ratio != ratioMap->end(); ratio++) {
        ratio_choice->add(_(ratio->first.c_str()), ratio->second, currentRatio == ratio->second);
    }

    return ratio_choice;
}

void GuiMenu::clearLoadedInput() {
    for (auto& i : mLoadedInput)
    {
        delete i;
    }
    mLoadedInput.clear();
}
