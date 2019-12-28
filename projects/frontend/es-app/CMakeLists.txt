project("emulationstation")

set(ES_HEADERS
        src/DemoMode.h
        src/EmulationStation.h
        src/FileSorts.h
        src/ItemType.h
        src/MameNameMap.h
        src/NetworkThread.h
        src/PlatformId.h
        src/VolumeControl.h
        src/games/FileData.h
        src/games/FolderData.h
        src/games/EmptyData.h
        src/games/RootFolderData.h
        src/games/MetadataDescriptor.h
        src/games/MetadataFieldDescriptor.h
        src/systems/EmulatorDescriptor.h
        src/systems/EmulatorList.h
        src/systems/SystemDescriptor.h
        src/systems/SystemData.h
        src/systems/SystemManager.h

        # GuiComponents
        src/components/AsyncReqComponent.h
        src/components/RatingComponent.h
        src/components/ScraperSearchComponent.h
        src/components/TextListComponent.h

        # Guis
        src/guis/GuiMetaDataEd.h
        src/guis/GuiGameScraper.h
        src/guis/GuiGamelistOptions.h
        src/guis/GuiMenu.h
        src/guis/GuiSettings.h
        src/guis/GuiScraperMulti.h
        src/guis/GuiScraperStart.h
        src/guis/GuiUpdate.h
        src/guis/GuiInfoPopup.h
        src/guis/MenuMessages.h
        src/guis/GuiNetPlay.h
        src/guis/GuiHashStart.h
        src/guis/GuiLoading.h

        # Scrapers
        src/scraping/new/ScraperFactory.h
        src/scraping/new/INotifyScrapeResult.h
        src/scraping/new/ScrapeResult.h
        src/scraping/new/scrapers/IScraperEngine.h
        src/scraping/new/scrapers/screenscraper/ScreenScraperEngine.h
        src/scraping/new/scrapers/screenscraper/ScreenScraperApis.h
        src/scraping/new/scrapers/thegamedb/TheGameDBEngine.h

        #src/scraping/Scraper.h
        #src/scraping/GamesDBJSONScraper.h
        #src/scraping/GamesDBJSONScraperResources.h
        #src/scrapers/MamedbScraper.h
        #src/scraping/ScreenscraperScraper.h

        # Views
        src/views/gamelist/BasicGameListView.h
        src/views/gamelist/DetailedGameListView.h
        src/views/gamelist/IGameListView.h
        src/views/gamelist/ISimpleGameListView.h
        src/views/gamelist/GridGameListView.h
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
        )

set(ES_SOURCES
        src/main.cpp
        src/DemoMode.cpp
        src/FileSorts.cpp
        src/MameNameMap.cpp
        src/NetworkThread.cpp
        src/PlatformId.cpp
        src/VolumeControl.cpp
        src/games/FileData.cpp
        src/games/FolderData.cpp
        src/games/MetadataDescriptor.cpp
        src/systems/SystemData.cpp
        src/systems/SystemManager.cpp

        # GuiComponents
        src/components/AsyncReqComponent.cpp
        src/components/RatingComponent.cpp
        src/components/ScraperSearchComponent.cpp

        # Guis
        src/guis/GuiMetaDataEd.cpp
        src/guis/GuiGameScraper.cpp
        src/guis/GuiGamelistOptions.cpp
        src/guis/GuiMenu.cpp
        src/guis/GuiSettings.cpp
        src/guis/GuiScraperMulti.cpp
        src/guis/GuiScraperStart.cpp
        src/guis/GuiUpdate.cpp
        src/guis/GuiInfoPopup.cpp
        src/guis/MenuMessages.cpp
        src/guis/GuiNetPlay.cpp
        src/guis/GuiHashStart.cpp

        # Scrapers
        src/scraping/new/ScraperFactory.cpp
        src/scraping/new/scrapers/screenscraper/ScreenScraperEngine.cpp
        src/scraping/new/scrapers/screenscraper/ScreenScraperApis.cpp
        src/scraping/new/scrapers/thegamedb/TheGameDBEngine.cpp

        #src/scraping/Scraper.cpp
        #src/scraping/GamesDBJSONScraper.cpp
        #src/scraping/GamesDBJSONScraperResources.cpp
        #src/scrapers/MamedbScraper.cpp
        #src/scraping/ScreenscraperScraper.cpp

        # Views
        src/views/gamelist/BasicGameListView.cpp
        src/views/gamelist/DetailedGameListView.cpp
        src/views/gamelist/IGameListView.cpp
        src/views/gamelist/ISimpleGameListView.cpp
        src/views/gamelist/GridGameListView.cpp
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
SET(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "debhelper (>= 8.0.0), cmake, g++ (>= 4.8), libsdl2-dev, libsdl2-mixer-dev, libfreeimage-dev, libfreetype6-dev, libeigen3-dev, libcurl4-openssl-dev, libasound2-dev, libgl1-mesa-dev")

SET(CPACK_PACKAGE_VENDOR "emulationstation.org")
SET(CPACK_PACKAGE_VERSION "2.0.0~rc1")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "emulationstation_${CMAKE_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_EXECUTABLES "emulationstation" "emulationstation")

SET(CPACK_GENERATOR "TGZ;DEB")

INCLUDE(CPack)
