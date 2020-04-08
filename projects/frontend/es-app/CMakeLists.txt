project("emulationstation")

set(ES_HEADERS
        src/DemoMode.h
        src/EmulationStation.h
        src/games/FileSorts.h
        src/ItemType.h
        src/MameNameMapManager.h
        src/NetworkThread.h
        src/systems/PlatformId.h
        src/VolumeControl.h
        src/games/IFilter.h
        src/games/FileData.h
        src/games/FolderData.h
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
        src/systems/NetPlayData.h
        src/systems/SystemData.h
        src/systems/SystemDescriptor.h
        src/systems/SystemDeserializer.h
        src/systems/SystemManager.h
        src/usernotifications/NotificationManager.h

        # GuiComponents
        src/components/RatingComponent.h
        src/components/ScraperSearchComponent.h
        src/components/TextListComponent.h
        src/components/ITextListComponentOverlay.h

        # Guis
        src/guis/GuiMetaDataEd.h
        src/guis/GuiGamelistOptions.h
        src/guis/GuiMenu.h
        src/guis/GuiSettings.h
        src/guis/GuiScraperRun.h
        src/guis/GuiScraperSelect.h
        src/guis/GuiScraperOptions.h
        src/guis/GuiScraperSingleGameRun.h
        src/guis/GuiUpdate.h
        src/guis/GuiInfoPopup.h
        src/guis/MenuMessages.h
        src/guis/GuiNetPlay.h
        src/guis/GuiHashStart.h
        src/guis/GuiLoading.h
        src/guis/GuiBiosScan.h
        src/guis/GuiBiosMd5.h
        src/guis/GuiQuit.h
        src/guis/GuiSearch.h

        # MQTT
        src/mqtt/Countdown.h
        src/mqtt/MqttClient.h
        src/mqtt/TcpNetwork.h
        src/mqtt/MQTTClient/FP.h
        src/mqtt/MQTTClient/MQTTClient.h
        src/mqtt/MQTTClient/MQTTLogging.h
        src/mqtt/MQTTPacket/MQTTConnect.h
        src/mqtt/MQTTPacket/MQTTFormat.h
        src/mqtt/MQTTPacket/MQTTPublish.h
        src/mqtt/MQTTPacket/MQTTSubscribe.h
        src/mqtt/MQTTPacket/MQTTPacket.h
        src/mqtt/MQTTPacket/MQTTUnsubscribe.h

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
        src/scraping/scrapers/screenscraper/ScreenScraperEngine.h
        src/scraping/scrapers/screenscraper/ScreenScraperApis.h
        src/scraping/scrapers/thegamedb/TheGameDBEngine.h

        # Views
        src/views/gamelist/BasicGameListView.h
        src/views/gamelist/DetailedGameListView.h
        src/views/gamelist/IGameListView.h
        src/views/gamelist/ISimpleGameListView.h
        src/views/gamelist/GridGameListView.h
        src/views/gamelist/SystemIcons.h
        src/views/SystemView.h
        src/views/ViewController.h
        src/views/SplashView.h
        src/views/IProgressInterface.h

        # Animations
        src/animations/LaunchAnimation.h
        src/animations/MoveCameraAnimation.h

        # Recalbox
        src/recalbox/RecalboxSystem.h
        src/recalbox/RecalboxUpgrade.h
        src/LibretroRatio.h
        src/CommandThread.h
        src/NetPlayThread.h
        src/MainRunner.h
        src/ApplicationWindow.h

        # Pad 2 Keyboard
        src/padtokeyboard/Configuration.h
        src/padtokeyboard/PadToKeyboardManager.h
        src/padtokeyboard/keyboard/VirtualKeyboard.h
        src/padtokeyboard/pad/Pad.h
        src/padtokeyboard/pad/MappingConfiguration.h
        src/padtokeyboard/pad/PadItems.h
        src/padtokeyboard/pad/PadConfiguration.h
        )

set(ES_SOURCES
        src/main.cpp
        src/DemoMode.cpp
        src/games/FileSorts.cpp
        src/MameNameMap.cpp
        src/MameNameMapManager.cpp
        src/NetworkThread.cpp
        src/systems/PlatformId.cpp
        src/VolumeControl.cpp
        src/games/FileData.cpp
        src/games/FolderData.cpp
        src/games/MetadataDescriptor.cpp
        src/games/classifications/Genres.cpp
        src/games/classifications/Regions.cpp
        src/systems/EmulatorManager.cpp
        src/systems/EmulatorList.cpp
        src/systems/SystemData.cpp
        src/systems/SystemDeserializer.cpp
        src/systems/SystemManager.cpp
        src/usernotifications/NotificationManager.cpp

        # GuiComponents
        src/components/RatingComponent.cpp
        src/components/ScraperSearchComponent.cpp

        # Guis
        src/guis/GuiMetaDataEd.cpp
        src/guis/GuiGamelistOptions.cpp
        src/guis/GuiMenu.cpp
        src/guis/GuiSettings.cpp
        src/guis/GuiScraperRun.cpp
        src/guis/GuiScraperSelect.cpp
        src/guis/GuiScraperOptions.cpp
        src/guis/GuiScraperSingleGameRun.cpp
        src/guis/GuiUpdate.cpp
        src/guis/GuiInfoPopup.cpp
        src/guis/MenuMessages.cpp
        src/guis/GuiNetPlay.cpp
        src/guis/GuiHashStart.cpp
        src/guis/GuiBiosScan.cpp
        src/guis/GuiBiosMd5.cpp
        src/guis/GuiQuit.cpp
        src/guis/GuiSearch.cpp

        # MQTT
        src/mqtt/MqttClient.cpp
        src/mqtt/TcpNetwork.cpp
        src/mqtt/MQTTClient/FP.h
        src/mqtt/MQTTClient/MQTTClient.h
        src/mqtt/MQTTClient/MQTTLogging.h
        src/mqtt/MQTTPacket/MQTTConnectClient.c
        src/mqtt/MQTTPacket/MQTTConnectServer.c
        src/mqtt/MQTTPacket/MQTTDeserializePublish.c
        src/mqtt/MQTTPacket/MQTTFormat.c
        src/mqtt/MQTTPacket/MQTTPacket.c
        src/mqtt/MQTTPacket/MQTTSerializePublish.c
        src/mqtt/MQTTPacket/MQTTSubscribeClient.c
        src/mqtt/MQTTPacket/MQTTSubscribeServer.c
        src/mqtt/MQTTPacket/MQTTUnsubscribeClient.c
        src/mqtt/MQTTPacket/MQTTUnsubscribeServer.c

        # Bios
        src/bios/Bios.cpp
        src/bios/BiosList.cpp
        src/bios/BiosManager.cpp

        # Scrapers
        src/scraping/ScraperTools.cpp
        src/scraping/ScraperFactory.cpp
        src/scraping/scrapers/screenscraper/ScreenScraperEngine.cpp
        src/scraping/scrapers/screenscraper/ScreenScraperApis.cpp
        src/scraping/scrapers/thegamedb/TheGameDBEngine.cpp

        # Views
        src/views/gamelist/BasicGameListView.cpp
        src/views/gamelist/DetailedGameListView.cpp
        src/views/gamelist/IGameListView.cpp
        src/views/gamelist/ISimpleGameListView.cpp
        src/views/gamelist/GridGameListView.cpp
        src/views/gamelist/SystemIcons.cpp
        src/views/SystemView.cpp
        src/views/ViewController.cpp
        src/views/SplashView.cpp

        src/recalbox/RecalboxSystem.cpp
        src/recalbox/RecalboxUpgrade.cpp
        src/LibretroRatio.cpp
        src/CommandThread.cpp
        src/NetPlayThread.cpp
        src/MainRunner.cpp
        src/ApplicationWindow.cpp

        # Pad 2 Keyboard
        src/padtokeyboard/PadToKeyboardManager.cpp
        src/padtokeyboard/keyboard/VirtualKeyboard.cpp
        src/padtokeyboard/pad/Pad.cpp
        src/padtokeyboard/pad/MappingConfiguration.cpp
        src/padtokeyboard/pad/PadConfiguration.cpp
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
include_directories(${COMMON_INCLUDE_DIRS} src)
add_executable(emulationstation ${ES_SOURCES} ${ES_HEADERS})
target_link_libraries(emulationstation ${COMMON_LIBRARIES} es-core)

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
