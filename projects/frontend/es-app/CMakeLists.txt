project("emulationstation")

set(ES_HEADERS
        src/DemoMode.h
        src/EmulationStation.h
        src/games/FileSorts.h
        src/games/ItemType.h
        src/GameNameMapManager.h
        src/Upgrade.h
        src/systems/PlatformId.h
        src/games/IFilter.h
        src/games/FileData.h
        src/games/FolderData.h
        src/games/GameFilesUtils.h
        src/games/GameRandomSelector.h
        src/games/EmptyData.h
        src/games/RootFolderData.h
        src/games/MetadataDescriptor.h
        src/games/MetadataFieldDescriptor.h
        src/games/classifications/Genres.h
        src/games/classifications/Regions.h
        src/systems/EmulatorData.h
        src/systems/EmulatorDescriptor.h
        src/systems/EmulatorList.h
        src/systems/EmulatorManager.h
        src/systems/GameRunner.h
        src/systems/LightGunDatabase.h
        src/systems/NetPlayData.h
        src/systems/CrtData.h
        src/systems/GameLinkedData.h
        src/systems/SystemData.h
        src/systems/SystemDescriptor.h
        src/systems/SystemDeserializer.h
        src/systems/SystemManager.h
        src/systems/SystemSorting.h
        src/usernotifications/NotificationManager.h

        # GuiComponents
        src/components/RatingComponent.h
        src/components/IRatingComponent.h
        src/components/ScraperSearchComponent.h
        src/components/TextListComponent.h
        src/components/ITextListComponentOverlay.h

        # Wizards
        src/guis/wizards/Wizard.h
        src/guis/wizards/WizardAgo2.h
        src/guis/wizards/WizardAgo3.h
        src/guis/wizards/WizardLanguage.h

        # Guis
        src/guis/GuiMetaDataEd.h
        src/guis/menus/GuiCheckMenu.h
        src/guis/menus/GuiMenu.h
        src/guis/menus/GuiMenuAdvancedSettings.h
        src/guis/menus/GuiMenuArcadeVirtualSystem.h
        src/guis/menus/GuiMenuBase.h
        src/guis/menus/GuiMenuBootSettings.h
        src/guis/menus/GuiMenuCRT.h
        src/guis/menus/GuiMenuGamelistOptions.h
        src/guis/menus/GuiMenuGamelistGameDeleteOptions.h
        src/guis/menus/GuiMenuGamelistGameDeleteSelectionOptions.h
        src/guis/menus/GuiMenuGamelistGameOptions.h
        src/guis/menus/GuiMenuGameSettings.h
        src/guis/menus/GuiMenuKodiSettings.h
        src/guis/menus/GuiMenuNetplay.h
        src/guis/menus/GuiMenuNetwork.h
        src/guis/menus/GuiMenuPads.h
        src/guis/menus/GuiMenuPadsPair.h
        src/guis/menus/GuiMenuPopupSettings.h
        src/guis/menus/GuiMenuRetroAchievements.h
        src/guis/menus/GuiMenuScraper.h
        src/guis/menus/GuiMenuScreensavers.h
        src/guis/menus/GuiMenuScreenScraperOptions.h
        src/guis/menus/GuiMenuSound.h
        src/guis/menus/GuiMenuSwitchKodiNetplay.h
        src/guis/menus/GuiMenuSystem.h
        src/guis/menus/GuiMenuSystemList.h
        src/guis/menus/GuiMenuSystemConfiguration.h
        src/guis/menus/GuiMenuThemeConfiguration.h
        src/guis/menus/GuiMenuThemeOptions.h
        src/guis/menus/GuiMenuTools.h
        src/guis/menus/GuiMenuUpdates.h
        src/guis/menus/GuiMenuUserInterface.h
        src/guis/menus/GuiMenuVirtualSystems.h
        src/guis/menus/GuiMenuVirtualSystemPerGenre.h
        src/guis/menus/IGuiMenuBase.h
        src/guis/GuiSettings.h
        src/guis/GuiScraperRun.h
        src/guis/GuiScraperSingleGameRun.h
        src/guis/GuiUpdateRecalbox.h
        src/guis/MenuMessages.h
        src/guis/GuiNetPlay.h
        src/guis/GuiNetPlayEditPasswords.h
        src/guis/GuiNetPlayHostPasswords.h
        src/guis/GuiNetPlayClientPasswords.h
        src/guis/GuiHashStart.h
        src/guis/GuiBiosScan.h
        src/guis/GuiBiosMd5.h
        src/guis/menus/GuiMenuQuit.h
        src/guis/GuiSearch.h
        src/guis/GuiControlHints.h
        src/guis/GuiWaitLongExecution.h

        # MQTT
        src/mqtt/Countdown.h
        src/mqtt/MqttClient.h
        src/mqtt/TcpNetwork.h

        # Bios
        src/bios/Bios.h
        src/bios/BiosList.h
        src/bios/BiosManager.h
        src/bios/IBiosScanReporting.h

        # Scrapers
        src/scraping/ScraperFactory.h
        src/scraping/INotifyScrapeResult.h
        src/scraping/ScrapeResult.h
        src/scraping/ScraperTools.h
        src/scraping/scrapers/IScraperEngine.h
        src/scraping/scrapers/screenscraper/Languages.h
        src/scraping/scrapers/screenscraper/ScreenScraperEngine.h
        src/scraping/scrapers/screenscraper/ScreenScraperEnums.h
        src/scraping/scrapers/screenscraper/ScreenScraperApis.h
        src/scraping/scrapers/thegamedb/TheGameDBEngine.h

        # Views
        src/views/crt/CrtView.h
        src/views/gamelist/BasicGameListView.h
        src/views/gamelist/DetailedGameListView.h
        src/views/gamelist/IGameListView.h
        src/views/gamelist/ISimpleGameListView.h
        src/views/gamelist/GridGameListView.h
        src/views/GameClipView.h
        src/views/SystemView.h
        src/views/ViewController.h
        src/views/SplashView.h
        src/views/IProgressInterface.h

        # Animations
        src/animations/LaunchAnimation.h
        src/animations/MoveCameraAnimation.h

        # Recalbox
        src/recalbox/RecalboxSystem.h
        src/recalbox/StorageDevices.h
        src/LibretroRatio.h
        src/CommandThread.h
        src/netplay/NetPlayThread.h
        src/netplay/DefaultPasswords.h
        src/MainRunner.h
        src/ApplicationWindow.h

        # Pad 2 Keyboard
        src/padtokeyboard/Configuration.h
        src/padtokeyboard/PadToKeyboardManager.h
        src/padtokeyboard/keyboard/VirtualKeyboard.h
        src/padtokeyboard/mouse/VirtualMouse.h
        src/padtokeyboard/pad/Pad.h
        src/padtokeyboard/pad/MappingConfiguration.h
        src/padtokeyboard/pad/PadItems.h
        src/padtokeyboard/pad/PadConfiguration.h

        # Paho C
        src/mqtt/paho/c/MQTTTime.h
        src/mqtt/paho/c/MQTTProtocolClient.h
        src/mqtt/paho/c/Clients.h
        src/mqtt/paho/c/utf-8.h
        src/mqtt/paho/c/MQTTPacket.h
        src/mqtt/paho/c/MQTTPacketOut.h
        src/mqtt/paho/c/Messages.h
        src/mqtt/paho/c/Tree.h
        src/mqtt/paho/c/Socket.h
        src/mqtt/paho/c/Log.h
        src/mqtt/paho/c/MQTTPersistence.h
        src/mqtt/paho/c/Thread.h
        src/mqtt/paho/c/MQTTProtocolOut.h
        src/mqtt/paho/c/MQTTPersistenceDefault.h
        src/mqtt/paho/c/SocketBuffer.h
        src/mqtt/paho/c/LinkedList.h
        src/mqtt/paho/c/MQTTProperties.h
        src/mqtt/paho/c/MQTTReasonCodes.h
        src/mqtt/paho/c/Base64.h
        src/mqtt/paho/c/SHA1.h
        src/mqtt/paho/c/WebSocket.h
        src/mqtt/paho/c/MQTTAsync.h

        # Paho cpp
        src/mqtt/paho/cpp/async_client.h
        src/mqtt/paho/cpp/buffer_ref.h
        src/mqtt/paho/cpp/buffer_view.h
        src/mqtt/paho/cpp/callback.h
        src/mqtt/paho/cpp/client.h
        src/mqtt/paho/cpp/connect_options.h
        src/mqtt/paho/cpp/delivery_token.h
        src/mqtt/paho/cpp/disconnect_options.h
        src/mqtt/paho/cpp/exception.h
        src/mqtt/paho/cpp/iaction_listener.h
        src/mqtt/paho/cpp/iasync_client.h
        src/mqtt/paho/cpp/iclient_persistence.h
        src/mqtt/paho/cpp/message.h
        src/mqtt/paho/cpp/properties.h
        src/mqtt/paho/cpp/response_options.h
        src/mqtt/paho/cpp/server_response.h
        src/mqtt/paho/cpp/ssl_options.h
        src/mqtt/paho/cpp/string_collection.h
        src/mqtt/paho/cpp/subscribe_options.h
        src/mqtt/paho/cpp/thread_queue.h
        src/mqtt/paho/cpp/token.h
        src/mqtt/paho/cpp/topic.h
        src/mqtt/paho/cpp/types.h
        src/mqtt/paho/cpp/will_options.h
        )

set(ES_SOURCES
        src/main.cpp
        src/DemoMode.cpp
        src/games/FileSorts.cpp
        src/GameNameMap.cpp
        src/GameNameMapManager.cpp
        src/Upgrade.cpp
        src/systems/PlatformId.cpp
        src/games/FileData.cpp
        src/games/FolderData.cpp
        src/games/GameFilesUtils.cpp
        src/games/GameRandomSelector.cpp
        src/games/MetadataDescriptor.cpp
        src/games/classifications/Genres.cpp
        src/games/classifications/Regions.cpp
        src/systems/EmulatorManager.cpp
        src/systems/EmulatorList.cpp
        src/systems/GameRunner.cpp
        src/systems/LightGunDatabase.cpp
        src/systems/SystemData.cpp
        src/systems/SystemDescriptor.cpp
        src/systems/SystemDeserializer.cpp
        src/systems/SystemManager.cpp
        src/systems/SystemSorting.cpp
        src/usernotifications/NotificationManager.cpp

        # GuiComponents
        src/components/RatingComponent.cpp
        src/components/ScraperSearchComponent.cpp

        # Wizards
        src/guis/wizards/Wizard.cpp
        src/guis/wizards/WizardAgo2.cpp
        src/guis/wizards/WizardAgo3.cpp
        src/guis/wizards/WizardLanguage.cpp

        # Guis
        src/guis/GuiMetaDataEd.cpp
        src/guis/menus/GuiCheckMenu.cpp
        src/guis/menus/GuiMenu.cpp
        src/guis/menus/GuiMenuAdvancedSettings.cpp
        src/guis/menus/GuiMenuArcadeVirtualSystem.cpp
        src/guis/menus/GuiMenuBase.cpp
        src/guis/menus/GuiMenuBootSettings.cpp
        src/guis/menus/GuiMenuCRT.cpp
        src/guis/menus/GuiMenuGamelistOptions.cpp
        src/guis/menus/GuiMenuGamelistGameDeleteOptions.cpp
        src/guis/menus/GuiMenuGamelistGameDeleteSelectionOptions.cpp
        src/guis/menus/GuiMenuGamelistGameOptions.cpp
        src/guis/menus/GuiMenuGameSettings.cpp
        src/guis/menus/GuiMenuKodiSettings.cpp
        src/guis/menus/GuiMenuNetplay.cpp
        src/guis/menus/GuiMenuNetwork.cpp
        src/guis/menus/GuiMenuPads.cpp
        src/guis/menus/GuiMenuPadsPair.cpp
        src/guis/menus/GuiMenuPopupSettings.cpp
        src/guis/menus/GuiMenuRetroAchievements.cpp
        src/guis/menus/GuiMenuScraper.cpp
        src/guis/menus/GuiMenuScreensavers.cpp
        src/guis/menus/GuiMenuScreenScraperOptions.cpp
        src/guis/menus/GuiMenuSound.cpp
        src/guis/menus/GuiMenuSwitchKodiNetplay.cpp
        src/guis/menus/GuiMenuSystem.cpp
        src/guis/menus/GuiMenuSystemList.cpp
        src/guis/menus/GuiMenuSystemConfiguration.cpp
        src/guis/menus/GuiMenuThemeConfiguration.cpp
        src/guis/menus/GuiMenuThemeOptions.cpp
        src/guis/menus/GuiMenuTools.cpp
        src/guis/menus/GuiMenuUpdates.cpp
        src/guis/menus/GuiMenuUserInterface.cpp
        src/guis/menus/GuiMenuVirtualSystems.cpp
        src/guis/menus/GuiMenuVirtualSystemPerGenre.cpp
        src/guis/GuiSettings.cpp
        src/guis/GuiScraperRun.cpp
        src/guis/GuiScraperSingleGameRun.cpp
        src/guis/GuiUpdateRecalbox.cpp
        src/guis/MenuMessages.cpp
        src/guis/GuiNetPlay.cpp
        src/guis/GuiNetPlayEditPasswords.cpp
        src/guis/GuiNetPlayHostPasswords.cpp
        src/guis/GuiNetPlayClientPasswords.cpp
        src/guis/GuiHashStart.cpp
        src/guis/GuiBiosScan.cpp
        src/guis/GuiBiosMd5.cpp
        src/guis/menus/GuiMenuQuit.cpp
        src/guis/GuiSearch.cpp
        src/guis/GuiControlHints.cpp
        src/guis/GuiWaitLongExecution.cpp

        # MQTT
        src/mqtt/MqttClient.cpp
        src/mqtt/TcpNetwork.cpp

        # Bios
        src/bios/Bios.cpp
        src/bios/BiosList.cpp
        src/bios/BiosManager.cpp

        # Scrapers
        src/scraping/ScraperTools.cpp
        src/scraping/ScraperFactory.cpp
        src/scraping/scrapers/screenscraper/Languages.cpp
        src/scraping/scrapers/screenscraper/ScreenScraperEngine.cpp
        src/scraping/scrapers/screenscraper/ScreenScraperEnums.cpp
        src/scraping/scrapers/screenscraper/ScreenScraperApis.cpp
        src/scraping/scrapers/thegamedb/TheGameDBEngine.cpp

        # Views
        src/views/crt/CrtView.cpp
        src/views/gamelist/BasicGameListView.cpp
        src/views/gamelist/DetailedGameListView.cpp
        src/views/gamelist/IGameListView.cpp
        src/views/gamelist/ISimpleGameListView.cpp
        src/views/gamelist/GridGameListView.cpp
        src/views/GameClipView.cpp
        src/views/SystemView.cpp
        src/views/ViewController.cpp
        src/views/SplashView.cpp

        src/recalbox/RecalboxSystem.cpp
        src/recalbox/StorageDevices.cpp
        src/LibretroRatio.cpp
        src/CommandThread.cpp
        src/netplay/NetPlayThread.cpp
        src/netplay/DefaultPasswords.cpp
        src/MainRunner.cpp
        src/ApplicationWindow.cpp

        # Pad 2 Keyboard
        src/padtokeyboard/PadToKeyboardManager.cpp
        src/padtokeyboard/keyboard/VirtualKeyboard.cpp
        src/padtokeyboard/mouse/VirtualMouse.cpp
        src/padtokeyboard/pad/Pad.cpp
        src/padtokeyboard/pad/MappingConfiguration.cpp
        src/padtokeyboard/pad/PadConfiguration.cpp

        # Paho C
        src/mqtt/paho/c/MQTTTime.c
        src/mqtt/paho/c/MQTTProtocolClient.c
        src/mqtt/paho/c/Clients.c
        src/mqtt/paho/c/utf-8.c
        src/mqtt/paho/c/MQTTPacket.c
        src/mqtt/paho/c/MQTTPacketOut.c
        src/mqtt/paho/c/Messages.c
        src/mqtt/paho/c/Tree.c
        src/mqtt/paho/c/Socket.c
        src/mqtt/paho/c/Log.c
        src/mqtt/paho/c/MQTTPersistence.c
        src/mqtt/paho/c/Thread.c
        src/mqtt/paho/c/MQTTProtocolOut.c
        src/mqtt/paho/c/MQTTPersistenceDefault.c
        src/mqtt/paho/c/SocketBuffer.c
        src/mqtt/paho/c/LinkedList.c
        src/mqtt/paho/c/MQTTProperties.c
        src/mqtt/paho/c/MQTTReasonCodes.c
        src/mqtt/paho/c/Base64.c
        src/mqtt/paho/c/SHA1.c
        src/mqtt/paho/c/WebSocket.c
        src/mqtt/paho/c/MQTTAsync.c

        # Paho cpp
        src/mqtt/paho/cpp/async_client.cpp
        src/mqtt/paho/cpp/client.cpp
        src/mqtt/paho/cpp/connect_options.cpp
        src/mqtt/paho/cpp/disconnect_options.cpp
        src/mqtt/paho/cpp/iclient_persistence.cpp
        src/mqtt/paho/cpp/message.cpp
        src/mqtt/paho/cpp/properties.cpp
        src/mqtt/paho/cpp/response_options.cpp
        src/mqtt/paho/cpp/ssl_options.cpp
        src/mqtt/paho/cpp/string_collection.cpp
        src/mqtt/paho/cpp/subscribe_options.cpp
        src/mqtt/paho/cpp/token.cpp
        src/mqtt/paho/cpp/topic.cpp
        src/mqtt/paho/cpp/will_options.cpp
        )

#-------------------------------------------------------------------------------
# define OS specific sources and headers
if (MSVC)
    LIST(APPEND ES_SOURCES
            src/EmulationStation.rc
            )
endif ()

#-------------------------------------------------------------------------------
# define target
include_directories(${COMMON_INCLUDE_DIRS} src src/mqtt/paho/c src/mqtt/paho/cpp)
add_executable(emulationstation ${ES_SOURCES} ${ES_HEADERS})
target_link_libraries(emulationstation ${COMMON_LIBRARIES} es-core resolv)

# special properties for Windows builds
if (MSVC)
    # Always compile with the "WINDOWS" subsystem to avoid console window flashing at startup
    # when --debug is not set (see es-core/src/main.cpp for explanation).
    # The console will still be shown if launched with --debug.
    # Note that up to CMake 2.8.10 this feature is broken: http://public.kitware.com/Bug/view.php?id=12566
    set_target_properties(emulationstation PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(emulationstation PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
    set_target_properties(emulationstation PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(emulationstation PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif ()


#-------------------------------------------------------------------------------
# set up CPack install stuff so `make install` does something useful

install(TARGETS emulationstation
        RUNTIME
        DESTINATION bin)

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A flexible graphical emulator front-end")
SET(CPACK_PACKAGE_DESCRIPTION "EmulationStation is a flexible, graphical front-end designed for keyboardless navigation of your multi-platform retro game collection.")

SET(CPACK_RESOURCE_FILE LICENSE "LICENSE.md")
SET(CPACK_RESOURCE_FILE README "README.md")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alec Lofquist <allofquist@yahoo.com>")
SET(CPACK_DEBIAN_PACKAGE_SECTION "misc")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libsdl2-2.0-0, libsdl2-mixer-2.0-0, libfreeimage3, libfreetype6, libcurl3, libasound2")
SET(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "debhelper (>= 8.0.0), cmake, g++ (>= 4.8), libsdl2-dev, libsdl2-mixer-dev, libfreeimage-dev, libfreetype6-dev, libcurl4-openssl-dev, libasound2-dev, libgl1-mesa-dev")

SET(CPACK_PACKAGE_VENDOR "emulationstation.org")
SET(CPACK_PACKAGE_VERSION "2.0.0~rc1")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "emulationstation_${CMAKE_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_EXECUTABLES "emulationstation" "emulationstation")

SET(CPACK_GENERATOR "TGZ;DEB")

INCLUDE(CPack)
