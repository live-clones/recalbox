# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [6.1]
### Added
- Shutdown System option moved at the top of the Quit menu
- Add option to press Hotkey+Start twice to quit the game

## [6.0]
### Added
- Mame roms fullname in Netplay GUI

### Fixed
- No game launch if core doesn't match
- Game not listed in lobby if password protected
- Restore scrapped folder display
- Changed popups handling for better stability
- Wifi options securely saved automatically

## [18.06.27] - 2018-06-27
### Added
- Refactored gamepad configuration panel
- Update Emulator / Core selection process to avoid gui inception and to display default values
- Update font size on game metadata gui
- Remove dead code of RomsManager, GuiFastSelect, FileSystemSelectorComponent
- Bring back battery indicator with new process - x86 only
- Added option for popup position
- Added help messages in popups
- Added slider for popups duration
- Added new fallback fonts to support all languages
- Language names in their own language
- Added an option to list folder content instead of folder name (flatfolder)
- Make gamelist sort persistent
- Hide previous metadata when focusing on a folder in gamelist
- Show folder content in detail panel
- Refresh detailed panel on favorite system after running a game
- Show single folder game as game in parent list
- Only display available letter in JumpToLetter + Append figures
- Avoid reload of images if source doesn't changed
- Update Games lists from GameList menu
- Set Menu list in loop mode
- Delete EmulationStation.h from source + add to gitignore

### Fixed
- Fixed back button in gamelist (back to parent level, not to root one)
- Fixed back from game launch, does not reset gamelist to keep cursor position
- Fixed old themes crash
- Fixed clock not in local time
- Deactivating battery indicator until better solution found
- Removed slider for max vram (default is now 80)
- Make "Available update" popup waiting for no popup displayed
- Fixed detailed panel missing when jumping to letter A
- Fixed set a game favorite for the first time (required two switches before)
- Stay is selected sub-folder when changing sort type / order in gamelist
- Unify onCursorChanged call in IList

## [18.04.20] - 2018-04-20
### Added
- Added pop-ups
- Added tag in theme to force transition style
- Added dynamic change of volume and audio output in menu
- Added ubuntu_condensed.ttf as fallback font
- Added clock in main menu
- Added missing fav icons for Amigas 3DO and X68k
- Add "QUIT" help message on main menu

### Fixed
- Fixed change of audio card
- Fixed reload of theme when gamelistonly=0
- Fixed HelpMessages not translated

## [18.03.30] - 2018-03-30
### Added
- Added scan music subfolders
- Added icons in quick menu
- Added default subset values for recalbox theme
- Added new item to menutheme, menus now fully themable 
- Added new subset for menu styles
- Added themable menus
- Redesigned sub-menus for theme and changed region attribute handling
- Added new subset for helpsystem icons
- Added system variable in theme
- Added themable icons in helpsystem
- Added themable helpsystem in menus
- Redesigned theme subsets handling to add flexibility
- Changed theme XML parser to manage themes with subset and regional options (see THEMES.MD)
- Added UI submenu to manage theme subsets and region
- Added vertical slide transition when carousel is vertical
- Added theming options for carousel and systeminfo in system view
- Added "features" tag in theming options
- Add a UDP sock server on port 1337
- Added new version support
- Added an option to scrape recalbox-mix-images from screenscraper
- Added new mamedb scrapper mirror
- Only save changed metadata when saving gamelist.xml (improve shutdown time)
- Ignore all known bios and devices for arcade/neogeo platform
- Added Czech support
- Added Latvian support
- Added Luxembourgish support
- Improved audio output device selection
- Added an upgrade progression indicator
- Display update changelog + update version, if update available
- Added help popups for games options
- Split Game informations into two popup screen

### Fixed
- Fixed busyanim display bug
- Fixed systeminfo not populated on startup
- Fixed all texts resized in vertical mode
- Fixed compilation with gcc7 and libboost1.62
- Fixed security option impossible to disable
- Fixed menu width
- Fixed display scrape informations if rom is inside a subfolder
- Fixed datetime bug in gamelistoptions
- Fixed gamelist reload after scrap and gamelist change
- Fixed custom update.type
- Fixed bluetooth message when pairing failed
- Avoid screensaver while updating
- Fixed update gamelist crash
- Fixed SEGFAULT when removing last game of a system
- Fixed SEGFAULT when hiding the last game of a system
- Added an option to force the basicgamelist view
- Added RPI3 overclock presets + Warning popup on TURBO/EXTREM presets
- Display NETWORK in storage devices list once switched on a NAS
- Jump to letter now moves to the first occurrence of the letter
- Fixed displayed names using more than 14 characters in TextEditComponent
- Avoid to launch Kodi when not in main interface
- Fixed black screen when exiting a game and clicking left / right during the animation

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
