# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]
### Added
- Added new version support
- Added an option to scrape recalbox-mix-images from screenscraper
- Added new mamedb scrapper mirror
- Only save changed metadata when saving gamelist.xml (improve shutdown time)
- Ignore all known bios and devices for arcade/neogeo platform
- Added Czech support
- Added Latvian support
- Added Luxembourgish support
### Fixed
- Fixed custom update.type
- Fixed bluetooth message when pairing failed
- Avoid screensaver while updating
- Fixed update gamelist crash
- Fixed SEGFAULT when removing last game of a system
- Fixed SEGFAULT when hiding the last game of a system
- Added an option to force the basicgamelist view
- Added RPI3 overclock presets + Warning popup on TURBO/EXTREM presets

## [4.0.0] - 2016-02-02
### Added
- Added help popups for system settings
- HTTPS support for screenscraper mirror
- Support of emulationstation.gamelistonly and emulationstation.hidesystemview
- Added Catalan translation
- Added a virtual keyboard
- Added GuiMsgBoxScroll, a scrollable message box
- Added a Screenscraper DB scraper
- Added 3 new sort lists : by genre/developer/number of players
- Added an option to show hidden games
- Add new Traditional Chinese Language
- Added custom favorites icons for each system
- Favorites as boolean in metadata
- Game options for each available systems
- Emulator and core switch in metadata
- hidden metadata
- Favorite selection now let cursor on the current game
- Reload gamelist feature
- Multithreaded list loading and writing
- Added favorite system
- Switched a and b buttons functions
- Added support for RPI3 overclock (only none)
- Added autosave/load in game options
- Added retroachievements in game options
- Added retroachievements hardcore mode in game options
- Added Creation of READY flag in /tmp directory
- Added integer scale option in game options

### Fixed
- Fixed ParseGamelistOnly support
- Enhanced boot time
- Fix Bug with small SHARE partition
- Fix : Es scraper won't no more gets stuck when screensaver is activated
- Fix : Background musics are now played randomly
