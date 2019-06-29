#include <list>
#include <algorithm>
#include <string>
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/format.hpp>

#include <functional>
#include <LibretroRatio.h>
#include <guis/GuiMsgBoxScroll.h>

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

GuiMenu::GuiMenu(Window* window) : GuiComponent(window), mMenu(window, _("MAIN MENU").c_str()), mVersion(window) {
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
    // QUIT >

  mMenuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  bool bartop =  RecalboxConf::getInstance()->get("emulationstation.menu") == "bartop";

  //KODI
    if (RecalboxConf::getInstance()->get("kodi.enabled") == "1") {
        addEntryWithHelp(_("KODI MEDIA CENTER").c_str(), _(MenuMessages::START_KODI_HELP_MSG), mMenuTheme->menuText.color, true,
                 [this] {
                     Window* window = mWindow;
                     if (!RecalboxSystem::getInstance()->launchKodi(window)) {
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

    //QUIT
    addEntry(_("QUIT").c_str(), mMenuTheme->menuText.color, true,
             [this] { menuQuit(); }, mMenuTheme->menuIconSet.quit);

  mVersion.setFont(mMenuTheme->menuFooter.font);
  mVersion.setColor(mMenuTheme->menuFooter.color);

    mVersion.setText("RB EMULATIONSTATION V" + strToUpper(PROGRAM_VERSION_STRING));
    mVersion.setHorizontalAlignment(ALIGN_CENTER);

    addChild(&mMenu);
    addChild(&mVersion);

    setSize(mMenu.getSize());
    setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);

  // Animation
  auto fadeFunc = [this](float t) {
    setOpacity(lerp<float>(0, 255, t));
    setPosition(getPosition().x(), lerp<float>(getPosition().y(), (Renderer::getScreenHeight() - mSize.y()) / 2, t));
  };

  setOpacity(0);

  setAnimation(new LambdaAnimation(fadeFunc, 200), 0);
}

GuiMenu::~GuiMenu() {
    clearLoadedInput();
}

void GuiMenu::createInputTextRow(GuiSettings *gui, std::string title, const char *settingsID, bool password, std::string help = "") {
  // LABEL
  Window *window = mWindow;
  ComponentListRow row;

  auto lbl = std::make_shared<TextComponent>(window, title, mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  row.addElement(lbl, true); // label

  std::shared_ptr<GuiComponent> ed;

  ed = std::make_shared<TextComponent>(window, ((password && !RecalboxConf::getInstance()->get(settingsID).empty()) ?
      "*********" : RecalboxConf::getInstance()->get(settingsID)),
      mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_RIGHT);
  row.addElement(ed, true);

  auto spacer = std::make_shared<GuiComponent>(mWindow);
  spacer->setSize(Renderer::getScreenWidth() * 0.005f, 0);
  row.addElement(spacer, false);

  auto bracket = std::make_shared<ImageComponent>(mWindow);
  bracket->setImage(mMenuTheme->iconSet.arrow);
  bracket->setColorShift(mMenuTheme->menuText.color);
  bracket->setResize(Eigen::Vector2f(0, round(mMenuTheme->menuText.font->getLetterHeight())));


  row.addElement(bracket, false);

  auto updateVal = [ed, settingsID, password](const std::string &newVal) {
    if (!password)
      ed->setValue(newVal);
    else {
      ed->setValue("*********");
    }
    RecalboxConf::getInstance()->set(settingsID, newVal);
  }; // ok callback (apply new value to ed)

  row.makeAcceptInputHandler([this, title, updateVal, settingsID] {
    if (Settings::getInstance()->getBool("UseOSK")) {
      mWindow->pushGui(
          new GuiTextEditPopupKeyboard(mWindow, title, RecalboxConf::getInstance()->get(settingsID),
                         updateVal, false));
    } else {
      mWindow->pushGui(
          new GuiTextEditPopup(mWindow, title, RecalboxConf::getInstance()->get(settingsID),
                     updateVal, false));
    }
  });
  gui->addRowWithHelp(row, title, help);
}

void GuiMenu::menuSystem(){
  Window *window = mWindow;

  auto s = new GuiSettings(mWindow, _("SYSTEM SETTINGS").c_str());

  auto version = std::make_shared<TextComponent>(mWindow,
      RecalboxUpgrade::getInstance()->getVersion(),
      mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  s->addWithLabel(version, _("VERSION"), _(MenuMessages::VERSION_HELP_MSG));
  bool warning = RecalboxSystem::getInstance()->isFreeSpaceLimit();
  auto space = std::make_shared<TextComponent>(mWindow,
      RecalboxSystem::getInstance()->getFreeSpaceInfo(),
      mMenuTheme->menuText.font,
      warning ? 0xFF0000FF : mMenuTheme->menuText.color);
  s->addWithLabel(space, _("DISK USAGE"), _(MenuMessages::DISK_USAGE_HELP_MSG));

  std::vector<std::string> availableStorage = RecalboxSystem::getInstance()->getAvailableStorageDevices();
  std::string selectedStorage = RecalboxSystem::getInstance()->getCurrentStorage();

  // Storage device
  auto optionsStorage = std::make_shared<OptionListComponent<std::string> >(window,
      _("STORAGE DEVICE"), false);
  if (selectedStorage == "NETWORK") {
    optionsStorage->add("NETWORK", "NETWORK", true);
  }
  for (auto& it : availableStorage)
  {
    if (it != "RAM") {
      if (boost::starts_with(it, "DEV")) {
        std::vector<std::string> tokens;
        boost::split(tokens, it, boost::is_any_of(" "));
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
  auto language_choice = std::make_shared<OptionListComponent<std::string> >(window, _("LANGUAGE"), false);
  std::string language = RecalboxConf::getInstance()->get("system.language");
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


  auto keyboard_choice = std::make_shared<OptionListComponent<std::string> >(window, _("KEYBOARD"), false);
  std::string keyboard = RecalboxConf::getInstance()->get("system.kblayout");
  if (keyboard.empty()) keyboard = "us";

  // linux loadkeys value
  keyboard_choice->add("DE", "de", keyboard == "de");
  keyboard_choice->add("DK", "dk", keyboard == "dk");
  keyboard_choice->add("ES", "es", keyboard == "es");
  keyboard_choice->add("FR", "fr", keyboard == "fr");
  keyboard_choice->add("IT", "it", keyboard == "it");
  keyboard_choice->add("US", "us", keyboard == "us");

  s->addWithLabel(keyboard_choice, _("KEYBOARD"), _(MenuMessages::KEYBOARD_HELP_MSG));


  s->addSaveFunc([window, language_choice, language, keyboard_choice, keyboard, optionsStorage, selectedStorage] {
    bool reboot = false;
    if (optionsStorage->changed()) {
      RecalboxSystem::getInstance()->setStorage(optionsStorage->getSelected());
      reboot = true;
    }

    if (language != language_choice->getSelected()) {
      RecalboxConf::getInstance()->set("system.language", language_choice->getSelected());
      RecalboxConf::getInstance()->saveRecalboxConf();
      reboot = true;
    }

    if (keyboard != keyboard_choice->getSelected()) {
      RecalboxConf::getInstance()->set("system.kblayout", keyboard_choice->getSelected());
      RecalboxConf::getInstance()->saveRecalboxConf();
      reboot = true;
    }

    if (reboot) {
      window->pushGui(
          new GuiMsgBox(window, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [window] {
            if (runRestartCommand() != 0) {
              LOG(LogWarning) << "Reboot terminated with non-zero result!";
            }
          })
      );
    }

  });
  mWindow->pushGui(s);
}

void GuiMenu::menuUpdates(){
  GuiSettings *updateGui = new GuiSettings(mWindow, _("UPDATES").c_str());
  // Enable updates
  auto updates_enabled = std::make_shared<SwitchComponent>(mWindow);
  updates_enabled->setState(RecalboxConf::getInstance()->get("updates.enabled") == "1");
  updateGui->addWithLabel(updates_enabled, _("CHECK UPDATES"), _(MenuMessages::UPDATE_CHECK_HELP_MSG));
  // Display available update version
  if (RecalboxUpgrade::getInstance()->canUpdate()) {
    auto updateVersion = std::make_shared<TextComponent>(mWindow,
        _("YES"), mMenuTheme->menuText.font, mMenuTheme->menuText.color);
    updateGui->addWithLabel(updateVersion, _("AVAILABLE UPDATE"), _(MenuMessages::UPDATE_VERSION_HELP_MSG));

    // Display available update changelog
    updateGui->addSubMenu(_("UPDATE CHANGELOG"), [this] {
      std::string changelog = RecalboxUpgrade::getInstance()->getUpdateChangelog();
      if (!changelog.empty()) {
        std::string message = changelog;
        std::string updateVersion = RecalboxUpgrade::getInstance()->getUpdateVersion();
        mWindow->displayScrollMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"),
            _("UPDATE VERSION:") + " " + updateVersion + "\n" +
            _("UPDATE CHANGELOG:") + "\n" + message);
      } else {
        mWindow->displayMessage(_("AN UPDATE IS AVAILABLE FOR YOUR RECALBOX"));
      }
    }, _(MenuMessages::UPDATE_CHANGELOG_HELP_MSG));
    // Start update
    updateGui->addSubMenu(_("START UPDATE"), [this] {
      mWindow->pushGui(new GuiUpdate(mWindow));
    }, _(MenuMessages::START_UPDATE_HELP_MSG));
  };

  // Enable updates

  auto updatesTypeComp = std::make_shared<OptionListComponent<std::string> >(mWindow,
      _("UPDATE TYPE"), false);
  std::string updatesType = RecalboxConf::getInstance()->get("updates.type");
  // Stable or nothing
  if (updatesType.empty() || updatesType == "beta" || updatesType == "unstable") {
    updatesType = "stable";
  }
  updatesTypeComp->add("stable", "stable", updatesType == "stable");
  updatesTypeComp->add("custom", "custom", updatesType != "stable");

  updateGui->addWithLabel(updatesTypeComp, _("UPDATE TYPE"), _(MenuMessages::UPDATE_TYPE_HELP_MSG));
  updateGui->addSaveFunc([updates_enabled, updatesTypeComp] {
    RecalboxConf::getInstance()->set("updates.enabled", updates_enabled->getState() ? "1" : "0");
    if(updatesTypeComp->getSelected() == "stable"){
      RecalboxConf::getInstance()->set("updates.type", updatesTypeComp->getSelected());
    }
    RecalboxConf::getInstance()->saveRecalboxConf();
  });
  mWindow->pushGui(updateGui);
}

void GuiMenu::menuGameSettings(){
  auto s = new GuiSettings(mWindow, _("GAMES SETTINGS").c_str());
  if (RecalboxConf::getInstance()->get("emulationstation.menu") != "bartop") {
    // Screen ratio choice
    auto ratio_choice = createRatioOptionList(mWindow, "global");
    s->addWithLabel(ratio_choice, _("GAME RATIO"), _(MenuMessages::GAME_RATIO_HELP_MSG));
    s->addSaveFunc([ratio_choice] {
      RecalboxConf::getInstance()->set("global.ratio", ratio_choice->getSelected());
      RecalboxConf::getInstance()->saveRecalboxConf();
    });
  }
  // smoothing
  auto smoothing_enabled = std::make_shared<SwitchComponent>(mWindow);
  smoothing_enabled->setState(RecalboxConf::getInstance()->get("global.smooth") == "1");
  s->addWithLabel(smoothing_enabled, _("SMOOTH GAMES"), _(MenuMessages::GAME_SMOOTH_HELP_MSG));

  // rewind
  auto rewind_enabled = std::make_shared<SwitchComponent>(mWindow);
  rewind_enabled->setState(RecalboxConf::getInstance()->get("global.rewind") == "1");
  s->addWithLabel(rewind_enabled, _("REWIND"), _(MenuMessages::GAME_REWIND_HELP_MSG));


  // autosave/load
  auto autosave_enabled = std::make_shared<SwitchComponent>(mWindow);
  autosave_enabled->setState(RecalboxConf::getInstance()->get("global.autosave") == "1");
  s->addWithLabel(autosave_enabled, _("AUTO SAVE/LOAD"), _(MenuMessages::GAME_AUTOSAVELOAD_HELP_MSG));

  // press twice to quit emulator
  auto quit_press_twice_enabled = std::make_shared<SwitchComponent>(mWindow);
  quit_press_twice_enabled->setState(RecalboxConf::getInstance()->get("global.quitpresstwice") == "1");
  s->addWithLabel(quit_press_twice_enabled, _("PRESS TWICE TO QUIT GAME"), _(MenuMessages::GAME_PRESS_TWICE_QUIT_HELP_MSG));
  s->addSaveFunc([quit_press_twice_enabled] {
      RecalboxConf::getInstance()->set("global.quitpresstwice", quit_press_twice_enabled->getState() ? "1" : "0");
      RecalboxConf::getInstance()->saveRecalboxConf();
    });

  // Shaders preset

  auto shaders_choices = std::make_shared<OptionListComponent<std::string> >(mWindow,
      _("SHADERS SET"), false);
  std::string currentShader = RecalboxConf::getInstance()->get("global.shaderset");
  if (currentShader.empty()) {
    currentShader = std::string("none");
  }

  shaders_choices->add(_("NONE"), "none", currentShader == "none");
  shaders_choices->add(_("SCANLINES"), "scanlines", currentShader == "scanlines");
  shaders_choices->add(_("RETRO"), "retro", currentShader == "retro");
  s->addWithLabel(shaders_choices, _("SHADERS SET"), _(MenuMessages::GAME_SHADERS_HELP_MSG));
  // Integer scale
  auto integerscale_enabled = std::make_shared<SwitchComponent>(mWindow);
  integerscale_enabled->setState(RecalboxConf::getInstance()->get("global.integerscale") == "1");
  s->addWithLabel(integerscale_enabled, _("INTEGER SCALE (PIXEL PERFECT)"), _(MenuMessages::GAME_INTEGER_SCALE_HELP_MSG));
  s->addSaveFunc([integerscale_enabled] {
    RecalboxConf::getInstance()->set("global.integerscale", integerscale_enabled->getState() ? "1" : "0");
    RecalboxConf::getInstance()->saveRecalboxConf();
  });

  shaders_choices->setSelectedChangedCallback(
      [integerscale_enabled](std::string selectedShader) {
        integerscale_enabled->setState(selectedShader != "none");
      });

  if (RecalboxConf::getInstance()->get("emulationstation.menu") != "bartop") {
    // Retroachievements
    {
      std::function<void()> openGui = [this] {
        GuiSettings *retroachievements = new GuiSettings(mWindow, _("RETROACHIEVEMENTS SETTINGS").c_str());
        // retroachievements_enable
        auto retroachievements_enabled = std::make_shared<SwitchComponent>(mWindow);
        retroachievements_enabled->setState(
            RecalboxConf::getInstance()->get("global.retroachievements") == "1");
        retroachievements->addWithLabel(retroachievements_enabled, _("RETROACHIEVEMENTS"), _(MenuMessages::RA_ONOFF_HELP_MSG));

        // retroachievements_hardcore_mode
        auto retroachievements_hardcore_enabled = std::make_shared<SwitchComponent>(
            mWindow);
        retroachievements_hardcore_enabled->setState(
            RecalboxConf::getInstance()->get("global.retroachievements.hardcore") ==
            "1");
        retroachievements->addWithLabel(retroachievements_hardcore_enabled, _("HARDCORE MODE"), _(MenuMessages::RA_HARDCORE_HELP_MSG));


        // retroachievements, username, password
        createInputTextRow(retroachievements, _("USERNAME"), "global.retroachievements.username", false);
        createInputTextRow(retroachievements, _("PASSWORD"), "global.retroachievements.password", true);

        retroachievements->addSaveFunc(
            [retroachievements_enabled, retroachievements_hardcore_enabled] {
              RecalboxConf::getInstance()->set("global.retroachievements", retroachievements_enabled->getState() ? "1" : "0");
              RecalboxConf::getInstance()->set("global.retroachievements.hardcore", retroachievements_hardcore_enabled->getState() ? "1" : "0");
              RecalboxConf::getInstance()->saveRecalboxConf();
            });
        mWindow->pushGui(retroachievements);
      };
      auto retroachievementsSettings = std::make_shared<TextComponent>(mWindow,
          _("RETROACHIEVEMENTS SETTINGS"), mMenuTheme->menuText.font, mMenuTheme->menuText.color);
      s->addSubMenu(_("RETROACHIEVEMENTS SETTINGS"), openGui, _(MenuMessages::RA_HELP_MSG));
    }
    if (RecalboxConf::getInstance()->get("emulationstation.menu") != "bartop") {
      // Netplay
      {
        std::function<void()> openGui = [this] {
          GuiSettings *netplay = new GuiSettings(mWindow, _("NETPLAY SETTINGS").c_str());
          // netplay_enable
          auto netplay_enabled = std::make_shared<SwitchComponent>(mWindow);
          netplay_enabled->setState(RecalboxConf::getInstance()->get("global.netplay") == "1");
          netplay->addWithLabel(netplay_enabled, _("NETPLAY"), _(MenuMessages::NP_ONOFF_HELP_MSG));

          // netplay username
          createInputTextRow(netplay, _("NICKNAME"), "global.netplay.nickname", false);

          // netplay port
          createInputTextRow(netplay, _("PORT"), "global.netplay.port", false);

          //mitm
          auto mitm_choices = std::make_shared<OptionListComponent<std::string> >(mWindow,
                                                                                  _("NETPLAY MITM"), false);
          std::string currentMitm = RecalboxConf::getInstance()->get("global.netplay.relay");
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
                RecalboxConf::getInstance()->set("global.netplay", netplay_enabled->getState() ? "1" : "0");
                RecalboxConf::getInstance()->set("global.netplay.relay", mitm);
                RecalboxConf::getInstance()->saveRecalboxConf();
              });
                    auto openHashNow = [this] { mWindow->pushGui(new GuiHashStart(mWindow)); };
                    netplay->addSubMenu(_("HASH ROMS"), openHashNow, _(MenuMessages::NP_HASH_HELP_MSG));
          mWindow->pushGui(netplay);
        };
        s->addSubMenu(_("NETPLAY SETTINGS"), openGui, _(MenuMessages::NP_HELP_MSG));
      }
    }

  }
  s->addSaveFunc([smoothing_enabled, rewind_enabled, shaders_choices, autosave_enabled] {
    RecalboxConf::getInstance()->set("global.smooth", smoothing_enabled->getState() ? "1" : "0");
    RecalboxConf::getInstance()->set("global.rewind", rewind_enabled->getState() ? "1" : "0");
    RecalboxConf::getInstance()->set("global.shaderset", shaders_choices->getSelected());
    RecalboxConf::getInstance()->set("global.autosave", autosave_enabled->getState() ? "1" : "0");
    RecalboxConf::getInstance()->saveRecalboxConf();
  });
  mWindow->pushGui(s);
}

void GuiMenu::menuControllers() {

  GuiSettings *s = new GuiSettings(mWindow, _("CONTROLLERS SETTINGS").c_str());

  Window *window = mWindow;

  ComponentListRow row;
  row.makeAcceptInputHandler([window, this, s] {
    window->pushGui(new GuiMsgBox(window,
        _("YOU ARE GOING TO CONFIGURE A CONTROLLER. IF YOU HAVE ONLY ONE JOYSTICK, "
        "CONFIGURE THE DIRECTIONS KEYS AND SKIP JOYSTICK CONFIG BY HOLDING A BUTTON. "
        "IF YOU DO NOT HAVE A SPECIAL KEY FOR HOTKEY, CHOOSE THE SELECT BUTTON. SKIP "
        "ALL BUTTONS YOU DO NOT HAVE BY HOLDING A KEY. BUTTONS NAMES ARE BASED ON THE "
        "SNES CONTROLLER."), _("OK"),
        [window, this, s] {
          window->pushGui(new GuiDetectDevice(window, false, [this, s] {
            s->setSave(false);
            delete s;
            this->menuControllers();
          }));
        }));
  });


  row.addElement(std::make_shared<TextComponent>(window, _("CONFIGURE A CONTROLLER"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRowWithHelp(row, _("CONFIGURE A CONTROLLER"), _(MenuMessages::CONTROLLER_CONF_HELP_MSG));

  row.elements.clear();

  std::function<void(void *)> showControllerList = [window, this, s](void *controllers) {
    std::function<void(void *)> deletePairGui = [window](void *pairedPointer) {
      bool paired = *((bool *) pairedPointer);
      window->pushGui(
          new GuiMsgBox(window, paired ? _("CONTROLLER PAIRED") : _("UNABLE TO PAIR CONTROLLER"), _("OK")));
      };
      if (controllers == nullptr) {
        window->pushGui(new GuiMsgBox(window, _("AN ERROR OCCURED"), _("OK")));
      } else {
        std::vector<std::string> *resolvedControllers = ((std::vector<std::string> *) controllers);
        if (resolvedControllers->size() == 0) {
          window->pushGui(new GuiMsgBox(window, _("NO CONTROLLERS FOUND"), _("OK")));
        } else {
          GuiSettings *pairGui = new GuiSettings(window, _("PAIR A BLUETOOTH CONTROLLER").c_str());
          for (std::vector<std::string>::iterator controllerString = ((std::vector<std::string> *) controllers)->begin();
             controllerString != ((std::vector<std::string> *) controllers)->end(); ++controllerString) {

            ComponentListRow controllerRow;
            std::function<void()> pairController = [this, window, pairGui, controllerString, deletePairGui] {
              window->pushGui(new GuiLoading(window, [controllerString] {
                bool paired = RecalboxSystem::getInstance()->pairBluetooth(*controllerString);

                return (void *) new bool(paired);
              }, deletePairGui));

            };
            controllerRow.makeAcceptInputHandler(pairController);
            auto update = std::make_shared<TextComponent>(window, *controllerString,
                                    mMenuTheme->menuText.font, mMenuTheme->menuText.color);
            auto bracket = makeArrow(window);
            controllerRow.addElement(update, true);
            controllerRow.addElement(bracket, false);
            pairGui->addRow(controllerRow);
          }
          window->pushGui(pairGui);
        }
      }

    };

  row.makeAcceptInputHandler([window, this, s, showControllerList] {
    window->pushGui(new GuiLoading(window, [] {
      auto s = RecalboxSystem::getInstance()->scanBluetooth();
      return (void *) s;
    }, showControllerList));
  });

  row.addElement(std::make_shared<TextComponent>(window, _("PAIR A BLUETOOTH CONTROLLER"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRowWithHelp(row, _("PAIR A BLUETOOTH CONTROLLER"), _(MenuMessages::CONTROLLER_BT_HELP_MSG));
  row.elements.clear();

  row.makeAcceptInputHandler([window, this, s] {
    RecalboxSystem::getInstance()->forgetBluetoothControllers();
    window->pushGui(new GuiMsgBox(window, _("CONTROLLERS LINKS HAVE BEEN DELETED."), _("OK")));
  });
  row.addElement(std::make_shared<TextComponent>(window, _("FORGET BLUETOOTH CONTROLLERS"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRowWithHelp(row, _("FORGET BLUETOOTH CONTROLLERS"), _(MenuMessages::CONTROLLER_FORGET_HELP_MSG));
  row.elements.clear();


  row.elements.clear();

  // Here we go; for each player
  std::list<int> alreadyTaken = std::list<int>();

  // clear the current loaded inputs
  clearLoadedInput();

  std::vector<std::shared_ptr<OptionListComponent<StrInputConfig *>>> options;
  char strbuf[256];

  for (int player = 0; player < MAX_PLAYERS; player++) {
    std::stringstream sstm;
    sstm << "INPUT P" << player + 1;
    std::string confName = sstm.str() + "NAME";
    std::string confGuid = sstm.str() + "GUID";
    snprintf(strbuf, 256, _("INPUT P%i").c_str(), player + 1);

    LOG(LogInfo) << player + 1 << " " << confName << " " << confGuid;
    auto inputOptionList = std::make_shared<OptionListComponent<StrInputConfig *> >(mWindow, strbuf, false);
    options.push_back(inputOptionList);

    // Checking if a setting has been saved, else setting to default
    std::string configuratedName = Settings::getInstance()->getString(confName);
    std::string configuratedGuid = Settings::getInstance()->getString(confGuid);
    bool found = false;
    // For each available and configured input
    for (auto it = 0; it < InputManager::getInstance()->getNumJoysticks(); it++) {
      InputConfig *config = InputManager::getInstance()->getInputConfigByDevice(it);
      if (config->isConfigured()) {
        // create name
        std::stringstream dispNameSS;
        dispNameSS << "#" << config->getDeviceId() << " ";
        std::string deviceName = config->getDeviceName();
        if (deviceName.size() > 25) {
          dispNameSS << deviceName.substr(0, 16) << "..." <<
                 deviceName.substr(deviceName.size() - 5, deviceName.size() - 1);
        } else {
          dispNameSS << deviceName;
        }

        std::string displayName = dispNameSS.str();


        bool foundFromConfig = configuratedName == config->getDeviceName() && configuratedGuid == config->getDeviceGUIDString();
        int deviceID = config->getDeviceId();
        // Si la manette est configurée, qu'elle correspond a la configuration, et qu'elle n'est pas
        // deja selectionnée on l'ajoute en séléctionnée
        StrInputConfig *newInputConfig = new StrInputConfig(config->getDeviceName(), config->getDeviceGUIDString());
        mLoadedInput.push_back(newInputConfig);

        if (foundFromConfig
          && std::find(alreadyTaken.begin(), alreadyTaken.end(), deviceID) == alreadyTaken.end()
          && !found) {
          found = true;
          alreadyTaken.push_back(deviceID);
          LOG(LogWarning) << "adding entry for player" << player << " (selected): " << config->getDeviceName() << "  " << config->getDeviceGUIDString();
          inputOptionList->add(displayName, newInputConfig, true);
        } else {
          LOG(LogWarning) << "adding entry for player" << player << " (not selected): " << config->getDeviceName() << "  " << config->getDeviceGUIDString();
          inputOptionList->add(displayName, newInputConfig, false);
        }
      }
    }
    if (configuratedName.compare("") == 0 || !found) {
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
  s->addSaveFunc([this, options, window] {
    for (int player = 0; player < MAX_PLAYERS; player++) {
      std::stringstream sstm;
      sstm << "INPUT P" << player + 1;
      std::string confName = sstm.str() + "NAME";
      std::string confGuid = sstm.str() + "GUID";

      auto input_p1 = options.at(player);
      std::string name;
      std::string selectedName = input_p1->getSelectedName();

      if (selectedName.compare(strToUpper("default")) == 0) {
        name = "DEFAULT";
        Settings::getInstance()->setString(confName, name);
        Settings::getInstance()->setString(confGuid, "");
      } else {
        if (input_p1->getSelected() != nullptr) {
          LOG(LogWarning) << "Found the selected controller ! : name in list  = " << selectedName;
          LOG(LogWarning) << "Found the selected controller ! : guid  = " << input_p1->getSelected()->deviceGUIDString;

          Settings::getInstance()->setString(confName, input_p1->getSelected()->deviceName);
          Settings::getInstance()->setString(confGuid, input_p1->getSelected()->deviceGUIDString);
        }
      }
    }

    Settings::getInstance()->saveFile();

  });

  row.elements.clear();
  window->pushGui(s);

}

void GuiMenu::menuUISettings(){
  auto s = new GuiSettings(mWindow, _("UI SETTINGS").c_str());
  Window *window = mWindow;


  std::function<void()> openGuiSS = [this] {
	  auto ss = new GuiSettings(mWindow, _("SCREENSAVER").c_str());

	  // screensaver time
	  auto screensaver_time = std::make_shared<SliderComponent>(mWindow, 0.f, 30.f, 1.f, "m");
	  screensaver_time->setValue((float) (Settings::getInstance()->getInt("ScreenSaverTime") / (1000 * 60)));
	  ss->addWithLabel(screensaver_time, _("SCREENSAVER AFTER"), _(MenuMessages::UI_SCREENSAVER_AFTER_HELP_MSG));
	  ss->addSaveFunc([screensaver_time] {
		  Settings::getInstance()->setInt("ScreenSaverTime", (int) round(screensaver_time->getValue()) * (1000 * 60));
	  });

	  // screensaver behavior
	  auto screensaver_behavior = std::make_shared<OptionListComponent<std::string> >(mWindow, _("TRANSITION STYLE"),
	                                                                                  false);
	  std::vector<std::string> screensavers;
	  screensavers.push_back("dim");
	  screensavers.push_back("black");
	  screensavers.push_back("demo");
	  for (auto it = screensavers.begin(); it != screensavers.end(); it++)
		  screensaver_behavior->add(*it, *it, Settings::getInstance()->getString("ScreenSaverBehavior") == *it);
	  ss->addWithLabel(screensaver_behavior, _("SCREENSAVER BEHAVIOR"),
	                   _(MenuMessages::UI_SCREENSAVER_BEHAVIOR_HELP_MSG));
	  ss->addSaveFunc([screensaver_behavior] {
		  Settings::getInstance()->setString("ScreenSaverBehavior", screensaver_behavior->getSelected());
	  });

	  // add systems (all with a platformid specified selected)
	  auto systems = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SYSTEMS TO SHOW IN DEMO"), true);
	  for (auto &it : SystemData::sSystemVector) {
		  if (!it->hasPlatformId(PlatformIds::PLATFORM_IGNORE))
			  systems->add(it->getFullName(), it->getName(),
			               RecalboxConf::getInstance()->isInList("global.demo.systemlist", it->getName()) &&
			               !it->getPlatformIds().empty());
	  }
	  ss->addWithLabel(systems, _("SYSTEMS FOR DEMO"));
	  ss->addSaveFunc([systems] {
		  std::vector<std::string> names = systems->getSelectedObjects();
		  RecalboxConf::getInstance()->setList("global.demo.systemlist", names);
	  });
	  mWindow->pushGui(ss);
  };

  s->addSubMenu(_("SCREENSAVER"), openGuiSS, _(MenuMessages::UI_SCREENSAVER_HELP_MSG));


  // display clock
  auto show_time = std::make_shared<SwitchComponent>(mWindow);
  show_time->setState(Settings::getInstance()->getBool("ShowClock"));
  s->addWithLabel(show_time, _("CLOCK IN MENU"), _(MenuMessages::UI_CLOCK_HELP_MSG));
  s->addSaveFunc([show_time] {
    Settings::getInstance()->setBool("ShowClock", show_time->getState());
  });

  // show help
  auto show_help = std::make_shared<SwitchComponent>(mWindow);
  show_help->setState(Settings::getInstance()->getBool("ShowHelpPrompts"));
  s->addWithLabel(show_help, _("ON-SCREEN HELP"), _(MenuMessages::UI_ONSCREENHELP_HELP_MSG));
  s->addSaveFunc([show_help] {
    Settings::getInstance()->setBool("ShowHelpPrompts", show_help->getState());
  });

  std::function<void()> openGuiSub = [this] {
    GuiSettings *PopupGui = new GuiSettings(mWindow, _("POPUP SETTINGS").c_str());

    auto popup_position = std::make_shared<OptionListComponent<std::string> >(mWindow, _("POPUP POSITION"), false);

    std::string currentPos = Settings::getInstance()->getString("PopupPosition");
    popup_position->add(_("TOP/RIGHT"), "Top/Right", currentPos == "Top/Right");
    popup_position->add(_("BOTTOM/RIGHT"), "Bottom/Right", currentPos == "Bottom/Right");
    popup_position->add(_("BOTTOM/LEFT"), "Bottom/Left", currentPos == "Bottom/Left");
    popup_position->add(_("TOP/LEFT"), "Top/Left", currentPos == "Top/Left");
    PopupGui->addWithLabel(popup_position, _("POPUP POSITION"), _(MenuMessages::UI_POPUP_POSITION_HELP_MSG));

    //help popup time 0=no popup
    auto help_popup_time = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 1.f, "s");
    help_popup_time->setValue((float) (Settings::getInstance()->getInt("HelpPopupTime")));
    PopupGui->addWithLabel(help_popup_time, _("HELP POPUP DURATION"), _(MenuMessages::UI_HELP_POPUP_DURATION_HELP_MSG));

    //music popup time 0=no popup
    auto music_popup_time = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 1.f, "s");
    music_popup_time->setValue((float) (Settings::getInstance()->getInt("MusicPopupTime")));
    PopupGui->addWithLabel(music_popup_time, _("MUSIC POPUP DURATION"), _(MenuMessages::UI_MUSIC_POPUP_DURATION_HELP_MSG));

    //netplay popup time 0=no popup
    /*auto netplay_popup_time = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 1.f, "s");
    netplay_popup_time->setValue((float) (Settings::getInstance()->getInt("NetplayPopupTime")));
    PopupGui->addWithLabel(netplay_popup_time, _("NETPLAY POPUP DURATION"), _(MenuMessages::UI_NETPLAY_POPUP_DURATION_HELP_MSG));*/

    PopupGui->addSaveFunc([help_popup_time, music_popup_time, popup_position] {
      Settings::getInstance()->setInt("HelpPopupTime", (int) round(help_popup_time->getValue()));
      Settings::getInstance()->setInt("MusicPopupTime", (int) round(music_popup_time->getValue()));
      //Settings::getInstance()->setInt("NetplayPopupTime", (int) round(netplay_popup_time->getValue()));
      Settings::getInstance()->setString("PopupPosition", popup_position->getSelected());
    });
    mWindow->pushGui(PopupGui);
  };
  s->addSubMenu(_("POPUP SETTINGS"), openGuiSub, _(MenuMessages::UI_POPUP_HELP_MSG));

  // quick system select (left/right in game list view)
  auto quick_sys_select = std::make_shared<SwitchComponent>(mWindow);
  quick_sys_select->setState(Settings::getInstance()->getBool("QuickSystemSelect"));
  s->addWithLabel(quick_sys_select, _("QUICK SYSTEM SELECT"), _(MenuMessages::UI_QUICK_HELP_MSG));
  s->addSaveFunc([quick_sys_select] {
    Settings::getInstance()->setBool("QuickSystemSelect", quick_sys_select->getState());
  });

  // Enable OSK (On-Screen-Keyboard)
  auto osk_enable = std::make_shared<SwitchComponent>(mWindow);
  osk_enable->setState(Settings::getInstance()->getBool("UseOSK"));
  s->addWithLabel(osk_enable, _("ON SCREEN KEYBOARD"), _(MenuMessages::UI_KEYBOARD_HELP_MSG));
  s->addSaveFunc([osk_enable] {
    Settings::getInstance()->setBool("UseOSK", osk_enable->getState());
  });

  std::function<void()> openGuiTheme = [this, window] {
		auto st = new GuiSettings(mWindow, _("THEME").c_str());

  // carousel transition option
  auto move_carousel = std::make_shared<SwitchComponent>(mWindow);
  move_carousel->setState(Settings::getInstance()->getBool("MoveCarousel"));
  st->addWithLabel(move_carousel, _("CAROUSEL ANIMATION"), _(MenuMessages::UI_CAROUSEL_HELP_MSG));
  st->addSaveFunc([move_carousel] { Settings::getInstance()->setBool("MoveCarousel", move_carousel->getState()); });

  // transition style
  auto transition_style = std::make_shared<OptionListComponent<std::string> >(mWindow, _("TRANSITION STYLE"), false);
  std::vector<std::string> transitions;
  transitions.push_back("fade");
  transitions.push_back("slide");
  transitions.push_back("instant");
  for (auto it = transitions.begin(); it != transitions.end(); it++)
    transition_style->add(*it, *it, Settings::getInstance()->getString("TransitionStyle") == *it);
  st->addWithLabel(transition_style, _("TRANSITION STYLE"), _(MenuMessages::UI_TRANSITION_HELP_MSG));
  st->addSaveFunc([transition_style] {
    Settings::getInstance()->setString("TransitionStyle", transition_style->getSelected());
  });

  // theme set
  auto themeSets = ThemeData::getThemeSets();

  auto selectedSet = themeSets.find(Settings::getInstance()->getString("ThemeSet"));
  if (selectedSet == themeSets.end())
    selectedSet = themeSets.begin();

  auto theme_set = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME SET"), false);
  for (auto it = themeSets.begin(); it != themeSets.end(); it++)
    theme_set->add(it->first, it->first, it == selectedSet);
  st->addWithLabel(theme_set, _("THEME SET"), _(MenuMessages::UI_THEME_HELP_MSG));

  std::function<void()> ReloadAll = [this, window] () {
    ViewController::get()->goToStart();
    window->renderShutdownScreen();
    delete ViewController::get();
    for (auto& systems : SystemData::sSystemVector)
      systems->loadTheme();
    GuiComponent *gui;
    while ((gui = window->peekGui()) != nullptr) {
      window->removeGui(gui);
    }
    delete gui;
    ViewController::init(window);
    ViewController::get()->reloadAll();
    window->pushGui(ViewController::get());
    ViewController::get()->goToStart();
    MenuThemeData::getInstance();
    auto transi = ThemeData::getCurrent()->getTransition();
    if (!transi.empty())
      Settings::getInstance()->setString("TransitionStyle", transi);
    Settings::getInstance()->setBool("ThemeChanged", false);
  };


  st->addSaveFunc([this, window, theme_set, ReloadAll] {

    if (Settings::getInstance()->getString("ThemeSet") != theme_set->getSelected())
    {

      Settings::getInstance()->setString("ThemeSet", theme_set->getSelected());

      auto themeSubSets = ThemeData::getThemeSubSets(theme_set->getSelected());
      auto themeColorSets = ThemeData::sortThemeSubSets(themeSubSets, "colorset");
      auto themeIconSets = ThemeData::sortThemeSubSets(themeSubSets, "iconset");
      auto themeMenus = ThemeData::sortThemeSubSets(themeSubSets, "menu");
      auto themeSystemviewSets = ThemeData::sortThemeSubSets(themeSubSets, "systemview");
      auto themeGamelistViewSets = ThemeData::sortThemeSubSets(themeSubSets, "gamelistview");
      auto themeRegions = ThemeData::sortThemeSubSets(themeSubSets, "region");

      // theme changed without setting options, forcing options to avoid crash/blank theme
      if (themeRegions.empty())
        Settings::getInstance()->setString("ThemeRegionName", "");
      else
        Settings::getInstance()->setString("ThemeRegionName", themeRegions.begin()->first);

      if (themeColorSets.empty())
        Settings::getInstance()->setString("ThemeColorSet", "");
      else
        Settings::getInstance()->setString("ThemeColorSet", themeColorSets.begin()->first);

      if (themeIconSets.empty())
        Settings::getInstance()->setString("ThemeIconSet", "");
      else
        Settings::getInstance()->setString("ThemeIconSet", themeIconSets.begin()->first);

      if (themeMenus.empty())
        Settings::getInstance()->setString("ThemeMenu", "");
      else
        Settings::getInstance()->setString("ThemeMenu", themeMenus.begin()->first);

      if (themeSystemviewSets.empty())
        Settings::getInstance()->setString("ThemeSystemView", "");
      else
        Settings::getInstance()->setString("ThemeSystemView", themeSystemviewSets.begin()->first);

      if (themeGamelistViewSets.empty())
        Settings::getInstance()->setString("ThemeGamelistView", "");
      else
        Settings::getInstance()->setString("ThemeGamelistView", themeGamelistViewSets.begin()->first);

      Settings::getInstance()->setBool("ThemeChanged", true);

      //reload theme:

      ReloadAll();
    }
  });


  // theme config
  std::function<void()> openGui = [this, theme_set, window, ReloadAll] {
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

    auto selectedColorSet = themeColorSets.find(Settings::getInstance()->getString("ThemeColorSet"));
    if (selectedColorSet == themeColorSets.end())
      selectedColorSet = themeColorSets.begin();
    auto theme_colorset = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME COLORSET"), false);

    for (auto it = themeColorSets.begin(); it != themeColorSets.end(); it++)
      theme_colorset->add(it->first, it->first, it == selectedColorSet);

    if (!themeColorSets.empty())
      themeconfig->addWithLabel(theme_colorset, _("THEME COLORSET"), _(MenuMessages::UI_THEME_COLORSET_MSG));

    // iconset

    auto selectedIconSet = themeIconSets.find(Settings::getInstance()->getString("ThemeIconSet"));
    if (selectedIconSet == themeIconSets.end())
      selectedIconSet = themeIconSets.begin();
    auto theme_iconset = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME ICONSET"), false);

    for (auto it = themeIconSets.begin(); it != themeIconSets.end(); it++)
      theme_iconset->add(it->first, it->first, it == selectedIconSet);

    if (!themeIconSets.empty())
      themeconfig->addWithLabel(theme_iconset, _("THEME ICONSET"), _(MenuMessages::UI_THEME_ICONSET_MSG));

    // menu

    auto selectedMenu = themeMenus.find(Settings::getInstance()->getString("ThemeMenu"));
    if (selectedMenu == themeMenus.end())
      selectedMenu = themeMenus.begin();
    auto theme_menu = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME MENU"), false);

    for (auto it = themeMenus.begin(); it != themeMenus.end(); it++)
      theme_menu->add(it->first, it->first, it == selectedMenu);

    if (!themeMenus.empty())
      themeconfig->addWithLabel(theme_menu, _("THEME MENU"), _(MenuMessages::UI_THEME_MENU_MSG));


    // systemview

    auto selectedSystemviewSet = themeSystemviewSets.find(Settings::getInstance()->getString("ThemeSystemView"));
    if (selectedSystemviewSet == themeSystemviewSets.end())
      selectedSystemviewSet = themeSystemviewSets.begin();

    auto theme_systemview = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME SYSTEMVIEW"), false);

    for (auto it = themeSystemviewSets.begin(); it != themeSystemviewSets.end(); it++)
      theme_systemview->add(it->first, it->first, it == selectedSystemviewSet);

    if (!themeSystemviewSets.empty())
      themeconfig->addWithLabel(theme_systemview, _("THEME SYSTEMVIEW"), _(MenuMessages::UI_THEME_SYSTEMVIEW_MSG));

    // gamelistview

    auto selectedGamelistViewSet = themeGamelistViewSets.find(Settings::getInstance()->getString("ThemeGamelistView"));
    if (selectedGamelistViewSet == themeGamelistViewSets.end())
      selectedGamelistViewSet = themeGamelistViewSets.begin();

    auto theme_gamelistview = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME GAMELISTVIEW"), false);

    for (auto it = themeGamelistViewSets.begin(); it != themeGamelistViewSets.end(); it++)
      theme_gamelistview->add(it->first, it->first, it == selectedGamelistViewSet);

    if (!themeGamelistViewSets.empty())
      themeconfig->addWithLabel(theme_gamelistview, _("THEME GAMELISTVIEW"), _(MenuMessages::UI_THEME_GAMELISTVIEW_MSG));

    // themeregion

    auto selectedRegion = themeRegions.find(Settings::getInstance()->getString("ThemeRegionName"));
    if (selectedRegion == themeRegions.end())
      selectedRegion = themeRegions.begin();

    auto theme_region = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME GAMELISTVIEW"), false);

    for (auto it = themeRegions.begin(); it != themeRegions.end(); it++)
      theme_region->add(it->first, it->first, it == selectedRegion);

    if (!themeRegions.empty())
      themeconfig->addWithLabel(theme_region, _("THEME REGION"), _(MenuMessages::UI_THEME_REGION_MSG));


    themeconfig->addSaveFunc([this, window, theme_set, theme_colorset, theme_iconset, theme_menu, theme_systemview, theme_gamelistview, theme_region, ReloadAll] {
      bool needReload = false;
      if (Settings::getInstance()->getString("ThemeColorSet") != theme_colorset->getSelected() && !theme_colorset->getSelected().empty())
        needReload = true;
      if (Settings::getInstance()->getString("ThemeIconSet") != theme_iconset->getSelected() && !theme_iconset->getSelected().empty())
        needReload = true;
      if (Settings::getInstance()->getString("ThemeMenu") != theme_menu->getSelected() && !theme_menu->getSelected().empty())
        needReload = true;
      if (Settings::getInstance()->getString("ThemeSystemView") != theme_systemview->getSelected() && !theme_systemview->getSelected().empty())
        needReload = true;
      if (Settings::getInstance()->getString("ThemeGamelistView") != theme_gamelistview->getSelected() && !theme_gamelistview->getSelected().empty())
        needReload = true;
      if (Settings::getInstance()->getString("ThemeRegionName") != theme_region->getSelected() && !theme_region->getSelected().empty())
        needReload = true;

      if (needReload){
        Settings::getInstance()->setString("ThemeSet", theme_set->getSelected());
        Settings::getInstance()->setString("ThemeColorSet", theme_colorset->getSelected());
        Settings::getInstance()->setString("ThemeIconSet", theme_iconset->getSelected());
        Settings::getInstance()->setString("ThemeMenu", theme_menu->getSelected());
        Settings::getInstance()->setString("ThemeSystemView", theme_systemview->getSelected());
        Settings::getInstance()->setString("ThemeGamelistView", theme_gamelistview->getSelected());
        Settings::getInstance()->setString("ThemeRegionName", theme_region->getSelected());
        Settings::getInstance()->setBool("ThemeChanged", true);
        //reload theme
        ReloadAll();
      }
    });
    if (!themeRegions.empty() || !themeGamelistViewSets.empty() || !themeSystemviewSets.empty() || !themeIconSets.empty()
          || !themeMenus.empty() || !themeColorSets.empty())
      mWindow->pushGui(themeconfig);
    else
      mWindow->pushGui(new GuiMsgBox(window, _("THIS THEME HAS NO OPTION"), _("OK")));
  };
  st->addSubMenu(_("THEME CONFIGURATION"), openGui, _(MenuMessages::UI_THEME_CONFIGURATION_MSG));



  st->addSaveFunc([] {
    RecalboxConf::getInstance()->saveRecalboxConf();
  });
		mWindow->pushGui(st);
	};

	s->addSubMenu(_("THEME"), openGuiTheme, _(MenuMessages::UI_THEME_HELP_MSG));

	// Game List Update
	s->addSubMenu(_("UPDATE GAMES LISTS"), [this,window] {
		window->pushGui(new GuiMsgBox(window, _("REALLY UPDATE GAMES LISTS ?"), _("YES"), [this,window] {
			ViewController::get()->goToStart();
			window->renderShutdownScreen();
			delete ViewController::get();
			SystemData::deleteSystems();
			SystemData::loadConfig();
			GuiComponent *gui;
			while ((gui = window->peekGui()) != nullptr) {
				window->removeGui(gui);
				delete gui;
			}
			ViewController::init(window);
			ViewController::get()->reloadAll();
			window->pushGui(ViewController::get());
			ViewController::get()->goToStart();
		}, _("NO"), nullptr));
	}, _(MenuMessages::UI_UPDATE_GAMELIST_HELP_MSG));

  mWindow->pushGui(s);
}

void GuiMenu::menuSoundSettings(){
  auto s = new GuiSettings(mWindow, _("SOUND SETTINGS").c_str());

  // volume
  auto setVolume = [](const float &newVal) {
    VolumeControl::getInstance()->setVolume((int) round(newVal));
  };
  auto volume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
  volume->setValue((float) VolumeControl::getInstance()->getVolume());
  volume->setSelectedChangedCallback(setVolume);
  s->addWithLabel(volume, _("SYSTEM VOLUME"), _(MenuMessages::SOUND_VOLUME_HELP_MSG));

  // disable sounds
  auto sounds_enabled = std::make_shared<SwitchComponent>(mWindow);
  sounds_enabled->setState(!(RecalboxConf::getInstance()->get("audio.bgmusic") == "0"));
  s->addWithLabel(sounds_enabled, _("FRONTEND MUSIC"), _(MenuMessages::SOUND_FRONTEND_MUSIC_HELP_MSG));

  // audio device
  auto optionsAudio = std::make_shared<OptionListComponent<std::string> >(mWindow, _("OUTPUT DEVICE"),
                                      false, FONT_SIZE_EXTRASMALL);
  std::string currentDevice = RecalboxConf::getInstance()->get("audio.device");
  if (currentDevice.empty()) currentDevice = "auto";

  std::vector<std::string> availableAudio = RecalboxSystem::getInstance()->getAvailableAudioOutputDevices();
  std::string selectedAudio = RecalboxSystem::getInstance()->getCurrentAudioOutputDevice();

  if (RecalboxConf::getInstance()->get("emulationstation.menu") != "bartop") {
    for (auto& it : availableAudio)
    {
      std::vector<std::string> tokens;
      boost::split( tokens, it, boost::is_any_of(" ") );

      if(tokens.size()>= 8){
        std::string vname;
        for (unsigned int i=0; i<8; i++) {
          vname += " ";
          vname += tokens.at(i);
        }
        optionsAudio->add(vname, it, selectedAudio == it);
      } else {
        optionsAudio->add(it, it, selectedAudio == it);
      }
    }
    auto setAudioDevice = [](const std::string &newVal) {
      RecalboxSystem::getInstance()->setAudioOutputDevice(newVal);
    };
    optionsAudio->setSelectedChangedCallback(setAudioDevice);
    s->addWithLabel(optionsAudio, _("OUTPUT DEVICE"));
  }
  s->addSaveFunc([optionsAudio, currentDevice, sounds_enabled, volume] {
    RecalboxConf::getInstance()->set("audio.volume", std::to_string((int) round(volume->getValue())));

    RecalboxConf::getInstance()->set("audio.bgmusic", sounds_enabled->getState() ? "1" : "0");
    if (!sounds_enabled->getState())
      AudioManager::getInstance()->stopMusic();
    if (currentDevice != optionsAudio->getSelected()) {
      RecalboxConf::getInstance()->set("audio.device", optionsAudio->getSelected());
    }
    RecalboxConf::getInstance()->saveRecalboxConf();
  });

  mWindow->pushGui(s);
}

void GuiMenu::menuNetworkSettings(){
  Window *window = mWindow;

  auto s = new GuiSettings(mWindow, _("NETWORK SETTINGS").c_str());
  auto status = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getInstance()->ping() ? _( "CONNECTED")
      : _("NOT CONNECTED"), mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  s->addWithLabel(status, _("STATUS"), _(MenuMessages::NETWORK_STATUS_HELP_MSG));
  auto ip = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getInstance()->getIpAdress(),
      mMenuTheme->menuText.font, mMenuTheme->menuText.color);
  s->addWithLabel(ip, _("IP ADDRESS"), _(MenuMessages::NETWORK_IP_HELP_MSG));

  // Hostname
  createInputTextRow(s, _("HOSTNAME"), "system.hostname", false, _(MenuMessages::NETWORK_HOST_HELP_MSG));

  // Wifi enable
  auto enable_wifi = std::make_shared<SwitchComponent>(mWindow);
  bool baseEnabled = RecalboxConf::getInstance()->get("wifi.enabled") == "1";
  enable_wifi->setState(baseEnabled);
  s->addWithLabel(enable_wifi, _("ENABLE WIFI"), _(MenuMessages::NETWORK_WIFI_HELP_MSG));

  //SSID
  std::string baseSSID = RecalboxConf::getInstance()->get("wifi.ssid");
  auto WifiSSID = [this, baseSSID, enable_wifi, baseEnabled] (GuiSettings *gui, std::string title,
      std::string value, std::string help = "") {

    ComponentListRow row;

    auto lbl = std::make_shared<TextComponent>(mWindow, title, mMenuTheme->menuText.font, mMenuTheme->menuText.color);
    row.addElement(lbl, true); // label

    std::shared_ptr<GuiComponent> ed;

    ed = std::make_shared<TextComponent>(mWindow, value, mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_RIGHT);
    row.addElement(ed, true);

    auto spacer = std::make_shared<GuiComponent>(mWindow);
    spacer->setSize(Renderer::getScreenWidth() * 0.005f, 0);
    row.addElement(spacer, false);

    auto bracket = std::make_shared<ImageComponent>(mWindow);
    bracket->setImage(":/arrow.svg");
    bracket->setResize(Eigen::Vector2f(0, lbl->getFont()->getLetterHeight()));
    row.addElement(bracket, false);

    auto updateVal = [ed](const std::string &newVal) {
      ed->setValue(newVal);
    };

    row.makeAcceptInputHandler([this, updateVal, enable_wifi, baseEnabled] {

      GuiSettings *SSID = new GuiSettings(mWindow, _("WIFI SSID").c_str());
      std::shared_ptr<GuiComponent> ed;
      ComponentListRow row;
      ed = std::make_shared<TextComponent>(mWindow, _("MANUAL INPUT"), mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_LEFT);
      row.addElement(ed, true);
      auto updateValue = [this, updateVal, SSID](const std::string &newVal) {
        RecalboxConf::getInstance()->set("wifi.ssid", newVal);
        updateVal(newVal);
        delete SSID;
      };
      row.makeAcceptInputHandler([this, updateValue] {
        if (Settings::getInstance()->getBool("UseOSK"))
          mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "", "", updateValue, false));
        else
          mWindow->pushGui(new GuiTextEditPopup(mWindow, "", "", updateValue, false));
      });
      SSID->addRowWithHelp(row, _("MANUAL INPUT"), MenuMessages::NETWORK_MANUAL_INPUT_HELP_MSG);
      if (enable_wifi->getState()) {
        std::vector<std::string> availableSSID = RecalboxSystem::getInstance()->getAvailableWiFiSSID(baseEnabled);
        RecalboxConf::getInstance()->set("wifi.enabled", "1");
        for (auto it = availableSSID.begin(); it != availableSSID.end(); it++) {

          if ((*it) != "\n") {

            row.elements.clear();
            std::vector<std::string> tokens;
            boost::split(tokens, (*it), boost::is_any_of(" "));

            if (tokens.size() >= 8) {
              std::string vname;
              for (unsigned int i = 0; i < 8; i++) {
                vname += " ";
                vname += tokens.at(i);
              }
              ed = std::make_shared<TextComponent>(mWindow, vname, mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_LEFT);
            } else {
              ed = std::make_shared<TextComponent>(mWindow, (*it), mMenuTheme->menuText.font, mMenuTheme->menuText.color, ALIGN_LEFT);
            }
            row.addElement(ed, true);
            row.makeAcceptInputHandler([this, updateValue, ed] { updateValue(ed->getValue()); });
            SSID->addRow(row);
          }
        }
      }
      mWindow->pushGui(SSID);
    });

    gui->addRowWithHelp(row, title, help);
  };
  WifiSSID(s, _("WIFI SSID"), baseSSID, _(MenuMessages::NETWORK_SSID_HELP_MSG));

  const std::string baseKEY = RecalboxConf::getInstance()->get("wifi.key");
  createInputTextRow(s, _("WIFI KEY"), "wifi.key", true, _(MenuMessages::NETWORK_KEY_HELP_MSG));

  s->addSaveFunc([baseEnabled, baseSSID, baseKEY, enable_wifi, window] {
    bool wifienabled = enable_wifi->getState();

    std::string newSSID = RecalboxConf::getInstance()->get("wifi.ssid");
    std::string newKey = RecalboxConf::getInstance()->get("wifi.key");

    if (wifienabled) {
      if (baseSSID != newSSID
        || baseKEY != newKey
        || !baseEnabled) {
        if (RecalboxSystem::getInstance()->enableWifi(newSSID, newKey)) {
          window->pushGui(new GuiMsgBox(window, _("WIFI ENABLED"))
          );
        } else {
          window->pushGui(new GuiMsgBox(window, _("WIFI CONFIGURATION ERROR"))
          );
        }
      }
    }
    else if (baseEnabled || RecalboxConf::getInstance()->get("wifi.enabled") == "1"){
      RecalboxSystem::getInstance()->disableWifi();
    }

    RecalboxConf::getInstance()->set("wifi.enabled", wifienabled ? "1" : "0");
    RecalboxConf::getInstance()->saveRecalboxConf();
    RecalboxSystem::getInstance()->backupRecalboxConf();
  });
  mWindow->pushGui(s);
}

void GuiMenu::menuScrapper(){

  auto openScrapeNow = [this] { mWindow->pushGui(new GuiScraperStart(mWindow)); };
  auto s = new GuiSettings(mWindow, _("SCRAPER").c_str());

  // scrape from
  auto scraper_list = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCRAPE FROM"), false);
  std::vector<std::string> scrapers = getScraperList();
  for (auto& scraper : scrapers)
    scraper_list->add(scraper, scraper, scraper == Settings::getInstance()->getString("Scraper"));

  s->addWithLabel(scraper_list, _("SCRAPE FROM"), _(MenuMessages::SCRAPER_FROM_HELP_MSG));
  s->addSaveFunc([scraper_list] {
    Settings::getInstance()->setString("Scraper", scraper_list->getSelected());
  });

  // scrape ratings
  auto scrape_ratings = std::make_shared<SwitchComponent>(mWindow);
  scrape_ratings->setState(Settings::getInstance()->getBool("ScrapeRatings"));
  s->addWithLabel(scrape_ratings, _("SCRAPE RATINGS"), _(MenuMessages::SCRAPER_RATINGS_HELP_MSG));
  s->addSaveFunc([scrape_ratings] {
    Settings::getInstance()->setBool("ScrapeRatings", scrape_ratings->getState());
  });

  // scrape now
  std::function<void()> openAndSave = openScrapeNow;
  openAndSave = [s, openAndSave] {
    s->save();
    openAndSave();
  };
  s->addSubMenu(_("SCRAPE NOW"), openAndSave, _(MenuMessages::SCRAPER_NOW_HELP_MSG));

  mWindow->pushGui(s);
}

void GuiMenu::menuAdvancedSettings(){
  Window *window = mWindow;

  auto s = new GuiSettings(mWindow, _("ADVANCED SETTINGS").c_str());

  // Overclock choice
  auto overclock_choice = std::make_shared<OptionListComponent<std::string> >(window, _("OVERCLOCK"), false);
  std::string currentOverclock = Settings::getInstance()->getString("Overclock");
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
  s->addSaveFunc([overclock_choice, overclockWarning, window]  {
    bool reboot = false;
    if (Settings::getInstance()->getString("Overclock") != overclock_choice->getSelected()) {
      Settings::getInstance()->setString("Overclock", overclock_choice->getSelected());
      RecalboxSystem::getInstance()->setOverclock(overclock_choice->getSelected());
      reboot = true;
    }
    RecalboxConf::getInstance()->saveRecalboxConf();
    if (reboot) {
      if (std::find(overclockWarning.begin(), overclockWarning.end(), overclock_choice->getSelected()) != overclockWarning.end()) {
        window->pushGui(
            new GuiMsgBox(window, _("TURBO AND EXTREM OVERCLOCK PRESETS MAY CAUSE SYSTEM UNSTABILITIES, SO USE THEM AT YOUR OWN RISK.\nIF YOU CONTINUE, THE SYSTEM WILL REBOOT NOW."),
                _("YES"), [window] {
                      if (runRestartCommand() != 0) {
                        LOG(LogWarning) << "Reboot terminated with non-zero result!";
                      }
                    }, _("NO"), [] {
                  Settings::getInstance()->setString("Overclock", "none");
                  RecalboxSystem::getInstance()->setOverclock("none");
                  Settings::getInstance()->saveFile();
                }));
      } else {
        window->pushGui(
            new GuiMsgBox(window, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [window] {
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
      auto kodiAtStart = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::getInstance()->get("kodi.atstartup") == "1");
      bootGui->addWithLabel(kodiAtStart, _("KODI AT START"), _(MenuMessages::ADVANCED_KODI_AT_START_HELP_MSG));
      // Gamelists only
      bool gamelistOnly = RecalboxConf::getInstance()->get("emulationstation.gamelistonly") == "1";
      auto gamelistOnlyComp = std::make_shared<SwitchComponent>(mWindow, gamelistOnly);
      bootGui->addWithLabel(gamelistOnlyComp, _("GAMELIST ONLY"), _(MenuMessages::ADVANCED_GAMELISTONLY_HELP_MSG));

      // Selected System
      std::string selectedsystem = RecalboxConf::getInstance()->get(
          "emulationstation.selectedsystem");
      auto system_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("BOOT ON SYSTEM"), false);
      std::string currentSystem = !selectedsystem.empty() ? selectedsystem : "favorites";
      // For each activated system
      std::vector<SystemData *> systems = SystemData::sSystemVector;
      bool found = false;
      for (auto system = systems.begin(); system != systems.end(); system++) {
        std::string systemName = (*system)->getName();
        if(systemName != "favorites") {
          found = found || currentSystem == systemName;
          system_choices->add(systemName, systemName, currentSystem == systemName);
        }
      }
      // Always add favorites, and select it if the user never chose another system
      system_choices->add("favorites", "favorites", !found);

      bootGui->addWithLabel(system_choices, _("BOOT ON SYSTEM"), _(MenuMessages::ADVANCED_BOOT_ON_SYSTEM_HELP_MSG));
      // Boot on gamelist
      bool bootOnGamelist =
          RecalboxConf::getInstance()->get("emulationstation.bootongamelist") == "1";
      auto bootOnGamelistComp = std::make_shared<SwitchComponent>(mWindow, bootOnGamelist);
      bootGui->addWithLabel(bootOnGamelistComp, _("BOOT ON GAMELIST"), _(MenuMessages::ADVANCED_BOOTGAMELIST_HELP_MSG));
      // Hide system view
      bool hidesystemview =
          RecalboxConf::getInstance()->get("emulationstation.hidesystemview") == "1";
      auto hidesystemviewComp = std::make_shared<SwitchComponent>(mWindow, hidesystemview);
      bootGui->addWithLabel(hidesystemviewComp, _("HIDE SYSTEM VIEW"), _(MenuMessages::ADVANCED_HIDESYSTEMVIEW_HELP_MSG));

      // Force Basicgamelist View
      bool basicgamelistview =
          RecalboxConf::getInstance()->get("emulationstation.forcebasicgamelistview") == "1";
      auto basicgamelistviewComp = std::make_shared<SwitchComponent>(mWindow, basicgamelistview);
      bootGui->addWithLabel(basicgamelistviewComp, _("FORCE BASIC GAMELIST VIEW"), _(MenuMessages::ADVANCED_BASICGAMELISTVIEW_HELP_MSG));

      bootGui->addSaveFunc(
          [gamelistOnlyComp, system_choices, kodiAtStart, bootOnGamelistComp, hidesystemviewComp, basicgamelistviewComp] {
            RecalboxConf::getInstance()->set("kodi.atstartup", kodiAtStart->getState() ? "1" : "0");
            RecalboxConf::getInstance()->set("emulationstation.gamelistonly", gamelistOnlyComp->getState() ? "1" : "0");
            RecalboxConf::getInstance()->set("emulationstation.selectedsystem", system_choices->getSelected());
            RecalboxConf::getInstance()->set("emulationstation.bootongamelist", bootOnGamelistComp->getState() ? "1" : "0");
            RecalboxConf::getInstance()->set("emulationstation.hidesystemview", hidesystemviewComp->getState() ? "1" : "0");
            RecalboxConf::getInstance()->set("emulationstation.forcebasicgamelistview", basicgamelistviewComp->getState() ? "1" : "0");
            RecalboxConf::getInstance()->saveRecalboxConf();
          });
      mWindow->pushGui(bootGui);
    };

    s->addSubMenu(_("BOOT SETTINGS"), openGui, _(MenuMessages::ADVANCED_BOOT_HELP_MSG));
  }

  // Custom config for systems
  {
    std::function<void()> openGuiD = [this, s] {
      s->save();
      GuiSettings *configuration = new GuiSettings(mWindow, _("EMULATOR ADVANCED CONFIGURATION").c_str());
      // For each activated system
      std::vector<SystemData *> systems = SystemData::sSystemVector;
      for (auto& system : systems)
      {
        if (system != SystemData::getFavoriteSystem()) {
          SystemData *systemData = system;
          configuration->addSubMenu(system->getFullName(), [this, systemData] {
            popSystemConfigurationGui(systemData);
          });
        }
      }
      mWindow->pushGui(configuration);

    };
    // Advanced button
    s->addSubMenu(_("ADVANCED EMULATOR CONFIGURATION"), openGuiD, _(MenuMessages::ADVANCED_EMULATOR_ADVANCED_HELP_MSG));
  }


  //Kodi
  {
    std::function<void()> openGui = [this] {
      GuiSettings *kodiGui = new GuiSettings(mWindow, _("KODI SETTINGS").c_str());
      auto kodiEnabled = std::make_shared<SwitchComponent>(mWindow);
      kodiEnabled->setState(RecalboxConf::getInstance()->get("kodi.enabled") == "1");
      kodiGui->addWithLabel(kodiEnabled, _("ENABLE KODI"), _(MenuMessages::ADVANCED_KODI_ENABLE_HELP_MSG));
      auto kodiAtStart = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::getInstance()->get("kodi.atstartup") == "1");
      kodiGui->addWithLabel(kodiAtStart, _("KODI AT START"), _(MenuMessages::ADVANCED_KODI_AT_START_HELP_MSG));
      auto kodiX = std::make_shared<SwitchComponent>(mWindow);
      kodiX->setState(RecalboxConf::getInstance()->get("kodi.xbutton") == "1");
      kodiGui->addWithLabel(kodiX, _("START KODI WITH X"), _(MenuMessages::ADVANCED_KODI_X_HELP_MSG));
      kodiGui->addSaveFunc([kodiEnabled, kodiAtStart, kodiX] {
        RecalboxConf::getInstance()->set("kodi.enabled", kodiEnabled->getState() ? "1" : "0");
        RecalboxConf::getInstance()->set("kodi.atstartup", kodiAtStart->getState() ? "1" : "0");
        RecalboxConf::getInstance()->set("kodi.xbutton", kodiX->getState() ? "1" : "0");
        RecalboxConf::getInstance()->saveRecalboxConf();
      });
      mWindow->pushGui(kodiGui);
    };
    s->addSubMenu(_("KODI SETTINGS"), openGui, _(MenuMessages::ADVANCED_KODI_HELP_MSG));
  }

  //Security
  {
    std::function<void()> openGui = [this] {
      GuiSettings *securityGui = new GuiSettings(mWindow, _("SECURITY").c_str());
      auto securityEnabled = std::make_shared<SwitchComponent>(mWindow);
      securityEnabled->setState(
          RecalboxConf::getInstance()->get("system.security.enabled") == "1");
      securityGui->addWithLabel(securityEnabled, _("ENFORCE SECURITY"), _(MenuMessages::ADVANCED_ENFORCE_SECURITY_HELP_MSG));

      auto rootpassword = std::make_shared<TextComponent>(mWindow, RecalboxSystem::getInstance()->getRootPassword(),
          mMenuTheme->menuText.font, mMenuTheme->menuText.color);
      securityGui->addWithLabel(rootpassword, _("ROOT PASSWORD"), _(MenuMessages::ADVANCED_ROOT_PWD_HELP_MSG));

      securityGui->addSaveFunc([this, securityEnabled] {
        Window *window = this->mWindow;
        bool reboot = false;

        if (securityEnabled->changed()) {
          RecalboxConf::getInstance()->set("system.security.enabled", securityEnabled->getState() ? "1" : "0");
          RecalboxConf::getInstance()->saveRecalboxConf();
          reboot = true;
        }

        if (reboot) {
          window->pushGui(
            new GuiMsgBox(window, _("THE SYSTEM WILL NOW REBOOT"), _("OK"), [window] {
              if (runRestartCommand() != 0) {
                LOG(LogWarning) << "Reboot terminated with non-zero result!";
              }
            })
          );
        }
      });
      mWindow->pushGui(securityGui);
    };
    s->addSubMenu(_("SECURITY"), openGui, _(MenuMessages::ADVANCED_SECURITY_HELP_MSG));
  }
  // overscan
  auto overscan_enabled = std::make_shared<SwitchComponent>(mWindow, Settings::getInstance()->getBool("Overscan"));
  s->addWithLabel(overscan_enabled, _("OVERSCAN"), _(MenuMessages::ADVANCED_OVERSCAN_HELP_MSG));
  s->addSaveFunc([overscan_enabled] {
    if (Settings::getInstance()->getBool("Overscan") != overscan_enabled->getState()) {
      Settings::getInstance()->setBool("Overscan", overscan_enabled->getState());
      RecalboxSystem::getInstance()->setOverscan(overscan_enabled->getState());
    }
  });

  // framerate
  auto framerate = std::make_shared<SwitchComponent>(mWindow, Settings::getInstance()->getBool("DrawFramerate"));
  s->addWithLabel(framerate, _("SHOW FRAMERATE"), _(MenuMessages::ADVANCED_FRAMERATE_HELP_MSG));
  s->addSaveFunc([framerate] {
    Settings::getInstance()->setBool("DrawFramerate", framerate->getState());
  });

  // Recalbox Manager
  auto manager = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::getInstance()->get("system.manager.enabled") == "1");
  s->addWithLabel(manager, _("RECALBOX MANAGER"), _(MenuMessages::ADVANCED_MANAGER_HELP_MSG));
  s->addSaveFunc([manager] {
    RecalboxConf::getInstance()->set("system.manager.enabled", manager->getState() ? "1" : "0");
    RecalboxConf::getInstance()->saveRecalboxConf();;
  });
  // Recalbox API
  auto recalboxApi = std::make_shared<SwitchComponent>(mWindow, RecalboxConf::getInstance()->get("system.api.enabled") == "1");
  s->addWithLabel(recalboxApi, _("RECALBOX API"), _(MenuMessages::ADVANCED_API_HELP_MSG));
  s->addSaveFunc([recalboxApi] {
    RecalboxConf::getInstance()->set("system.api.enabled", recalboxApi->getState() ? "1" : "0");
    RecalboxConf::getInstance()->saveRecalboxConf();;
  });
  mWindow->pushGui(s);
}

void GuiMenu::menuQuit(){
  auto s = new GuiSettings(mWindow, _("QUIT").c_str());

  Window *window = mWindow;

  ComponentListRow row;

  row.makeAcceptInputHandler([window] {
    window->pushGui(new GuiMsgBox(window, _("REALLY SHUTDOWN?"), _("YES"), [] {
        if (RecalboxSystem::getInstance()->shutdown() != 0) {
          LOG(LogWarning) << "Shutdown terminated with non-zero result!";
          }
        }, _("NO"), nullptr));
  });
  row.addElement(std::make_shared<TextComponent>(window, _("SHUTDOWN SYSTEM"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRow(row);

  row.elements.clear();
  row.makeAcceptInputHandler([window] {
    window->pushGui(new GuiMsgBox(window, _("REALLY SHUTDOWN WITHOUT SAVING METADATAS?"), _("YES"), [] {
        if (RecalboxSystem::getInstance()->fastShutdown() != 0) {
          LOG(LogWarning) << "Shutdown terminated with non-zero result!";
          }
        }, _("NO"), nullptr));
  });
  row.addElement(std::make_shared<TextComponent>(window, _("FAST SHUTDOWN SYSTEM"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRow(row);

  row.elements.clear();
  row.makeAcceptInputHandler([window] {
    window->pushGui(new GuiMsgBox(window, _("REALLY RESTART?"), _("YES"), [] {
        if (RecalboxSystem::getInstance()->reboot() != 0) {
          LOG(LogWarning) << "Restart terminated with non-zero result!";
          }
        }, _("NO"), nullptr));
  });
  row.addElement(std::make_shared<TextComponent>(window, _("RESTART SYSTEM"), mMenuTheme->menuText.font, mMenuTheme->menuText.color), true);
  s->addRow(row);

  mWindow->pushGui(s);
}

void GuiMenu::popSystemConfigurationGui(SystemData *systemData) const {

    // The system configuration
    GuiSettings *systemConfiguration = new GuiSettings(mWindow, systemData->getFullName().c_str());

    EmulatorDefaults emulatorDefaults = RecalboxSystem::getInstance()->getEmulatorDefaults(systemData->getName());

    //Emulator choice
    auto emu_choice = std::make_shared<OptionListComponent<std::string>>(mWindow, _("Emulator"), false);

    // Core choice
    auto core_choice = std::make_shared<OptionListComponent<std::string> >(mWindow, _("Core"), false);

    std::string currentEmulator = RecalboxConf::getInstance()->get(systemData->getName() + ".emulator");

    emu_choice->add(str(boost::format(_("DEFAULT (%1%)")) % emulatorDefaults.emulator), "default", true);

    for (auto& it : *systemData->getEmulators())
    {
        emu_choice->add(it.first, it.first, it.first == currentEmulator);
    }

    // when emulator changes, load new core list
    emu_choice->setSelectedChangedCallback([this, systemData, emulatorDefaults, core_choice](std::string emulatorName) {
        core_choice->clear();

        if (emulatorName == "default") {
            core_choice->add(str(boost::format(_("DEFAULT (%1%)")) % emulatorDefaults.core), "default", true);
            return ;
        }
        
        std::vector<std::string> cores = systemData->getCores(emulatorName);
        std::string currentCore = RecalboxConf::getInstance()->get(systemData->getName() + ".core");

        if (currentCore.empty()) {
            currentCore = "default";
        }

        // update current core if it is not available in the emulator selected core list
        if (currentCore != "default" && std::find(cores.begin(), cores.end(), currentCore) == cores.end()) {
            if (emulatorName == emulatorDefaults.emulator) {
                currentCore = emulatorDefaults.core;
            } else {
                // use first one
                currentCore = *(cores.begin());
            }
        }

        for (auto it = cores.begin(); it != cores.end(); it++) {
            std::string core = *it;
            // select at least first one in case of bad entry in config file
            bool selected = currentCore == core || it == cores.begin();
            if (currentCore == "default" && emulatorName == emulatorDefaults.emulator) {
                selected = selected || core == emulatorDefaults.core;
            }
            core_choice->add(core, core, selected);
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
    smoothing_enabled->setState(RecalboxConf::getInstance()->get(systemData->getName() + ".smooth", RecalboxConf::getInstance()->get("global.smooth")) == "1");
    systemConfiguration->addWithLabel(smoothing_enabled, _("SMOOTH GAMES"), _(MenuMessages::GAME_SMOOTH_HELP_MSG));
    // rewind
    auto rewind_enabled = std::make_shared<SwitchComponent>(mWindow);
    rewind_enabled->setState(RecalboxConf::getInstance()->get(systemData->getName() + ".rewind", RecalboxConf::getInstance()->get("global.rewind")) == "1");
    systemConfiguration->addWithLabel(rewind_enabled, _("REWIND"), _(MenuMessages::GAME_REWIND_HELP_MSG));
    // autosave
    auto autosave_enabled = std::make_shared<SwitchComponent>(mWindow);
    autosave_enabled->setState(RecalboxConf::getInstance()->get(systemData->getName() + ".autosave", RecalboxConf::getInstance()->get("global.autosave")) == "1");
    systemConfiguration->addWithLabel(autosave_enabled, _("AUTO SAVE/LOAD"), _(MenuMessages::GAME_AUTOSAVELOAD_HELP_MSG));


    systemConfiguration->addSaveFunc(
            [systemData, smoothing_enabled, rewind_enabled, ratio_choice, emu_choice, core_choice, autosave_enabled] {
                if (ratio_choice->changed()) {
                    RecalboxConf::getInstance()->set(systemData->getName() + ".ratio", ratio_choice->getSelected());
                }
                if (rewind_enabled->changed()) {
                    RecalboxConf::getInstance()->set(systemData->getName() + ".rewind", rewind_enabled->getState() ? "1" : "0");
                }
                if (smoothing_enabled->changed()) {
                    RecalboxConf::getInstance()->set(systemData->getName() + ".smooth", smoothing_enabled->getState() ? "1" : "0");
                }
                // always save both core and emulator
                // this is required to distinguish default
                if (emu_choice->changed() || core_choice->changed()) {
                    RecalboxConf::getInstance()->set(systemData->getName() + ".emulator", emu_choice->getSelected());
                    RecalboxConf::getInstance()->set(systemData->getName() + ".core", core_choice->getSelected());
                }
                if (autosave_enabled->changed()) {
                    RecalboxConf::getInstance()->set(systemData->getName() + ".autosave", autosave_enabled->getState() ? "1" : "0");
                }
                RecalboxConf::getInstance()->saveRecalboxConf();
            });
    mWindow->pushGui(systemConfiguration);
}

void GuiMenu::onSizeChanged() {
    mVersion.setSize(mSize.x(), 0);
    mVersion.setPosition(0, mSize.y() - mVersion.getSize().y());
}

void GuiMenu::addEntryWithHelp(const char *name, const std::string help, unsigned int color, bool add_arrow, const std::function<void()> &func, const std::string iconName) {

  auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
  std::shared_ptr<Font> font = menuTheme->menuText.font;
    // populate the list
    ComponentListRow row;

    if (!iconName.empty())
    {
        // icon
        auto icon = std::make_shared<ImageComponent>(mWindow);
        icon->setImage(iconName);
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

void GuiMenu::addEntry(const char *name, unsigned int color, bool add_arrow, const std::function<void()> &func, const std::string iconName) {
    addEntryWithHelp(name, "", color, add_arrow, func, iconName);
}

bool GuiMenu::input(InputConfig *config, Input input) {
    if (GuiComponent::input(config, input))
        return true;

    if ((config->isMappedTo("a", input) || config->isMappedTo("start", input)) && input.value != 0) {
        delete this;
        return true;
    }

    return false;
}

std::vector<HelpPrompt> GuiMenu::getHelpPrompts() {
    std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();
    prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));
    prompts.push_back(HelpPrompt("b", _("SELECT")));
    prompts.push_back(HelpPrompt("start", _("CLOSE")));
    return prompts;
}

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createRatioOptionList(Window *window, std::string configname) const {
    auto ratio_choice = std::make_shared<OptionListComponent<std::string> >(window, _("GAME RATIO"), false);
    std::string currentRatio = RecalboxConf::getInstance()->get(configname + ".ratio");
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
