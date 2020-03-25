# Release notes

## Version 6.2

### News
- Add Bios checking capabilities to EmulationStation
- Add License menu
- Adult game filtering in EmulationStation **
- New sorting options: Publisher and system-name for all virtual systems **
- 3 new virtual systems: All-games, Last-Played and Multiplayers **
- New virtual systems per genre (RPG, Shoot'em up, Pinballs, ...) **/*
- Region highligting in gamelist (highlight games from your favorite region) **
- Add laptop switch external screen function only for x86_64
- Add GameCube Bios 
- Add Nintendo 64DD platform
- Add libretro mame for recent mame set 
- Add EmulationStation event-driver user scripts
- Add Search feature and new Arcade-style virtual keyboard
- Add Pad-To-Keyboard driver
- New ngp & ngpc emulator focused on performance (fast) (libretro-race)
- Libretro flycast also available on rpi3, xu4 and x86
- Prboom system has been renamed to Doom
- Libretro puae emulates amigacd32 now

### Improvements
- Improve internal Scraper (ScreenScraper):
  - Faster! Use your ScreenScraper's threads to parallelize workloads 
  - Better! Lots of options, snaps vidéo, and more...
  - Stronger! Improved reliability and strongness
- Improve overall Emulationstation's stability & reliability
- Optimize memory requirements of EmulationStation
- Optimize EmulationStation boot time (up to 20 times faster!)
- Bump DosBox to r4290
- Bump ScummVM to 2.1.0 and map D-pad when no analog available
- Bump ResidualVM (2019-12)
- Bump Odroid UBOOT to 2017/05 version 
- Add 8bitdo N30 Pro 2 controller to pre-configured pads/joyticks
- Add IPega PG-9037 pad
- Improve pad processing in EmulationStation
- Bump & fix dolphin emulator
- Improved Genre & Region processing in EmulationStation * 
- Added missing texts into translated texts
- Improved french translations
- Faster-than-light CRC calculation for netplay 
- Game sorting and Jump-to-letter are now unicode compatible
- Add support for nVidia proprietary drivers version 390 and 440
- Bump oricatmos emulator on lastest version 
- Bump simcoupe emulator 
- Simplify emulator/core selection UI
- Bump retroarch to v1.8.5
- Bump retroarch-cheats to v1.8.5
- Bump libretro-assets on last version
- Bump libretro-cores family
- Rename 4do to opera (newer libretro 3do core)
- Bump Theodore core (add emulation of Thomson TO7 and TO7/70 computers)

### Fixes
- Fix Odroid XU4 fan issue
- Fix Odroid XU4 sound issue
- Fix EMMC boot on XU4
- Fix x86/x64 Boot videos
- Fix music popup crashes
- Fix music loop play
- Fix netplay popup crashes
- Fix apple2 not work on x86/x86_64
- Fix fullscreeen on oricatmos and apple2 on x86/x86_64 
- Fix duplicate folders in Arcade systems
- Fix quit menu not being accessible when boot-on-gamelist is on
- Fix some bad behaviors in favorite management
- Fix reboot Emulationstation with webmanager 

*  : Require to scrape missing data using the internal scraper 
** : Availailable in both EmulationStation menu and configuration file 


## Version 6.1.1

### News

### Improvements
- Add zfast shaders, fast CRT shaders for all platforms
- Odroid C2 removed from supported boards
- Improved button filtering while mapping pads (especially PS2/PS3 pads)
- Make apple II mapping more consistent: Use button A/B instead of L/R
- Bump kernel firmwares to latest version
- Add new Game Boy shader
- New wifi management
- Improve Blutooth association
- Bump FreeIntV to include latest fixes
- Bump FBNeo to include latest fixes
- Bump Retroarch to 1.8.1  to include latest fixes & optimizations
- Bump Doplhin-emu on 5.0-11288 add automatic configuration and netplay compatibility

### Fixes
- Fix Reicast on XU4 and PC platforms and fix wrong flycast entry on Rpi
- Fix .cpr and .bin extensions for the Amstrad GX4000 system
- Fix libretro-uae bios list
- Fix GPIO driver not loading (Arcade, GameCon & DB9)
- Fix random crashed while remapping pads
- Fix AppleIIGS not running with some pads
- Fix AppleIIGS multidisk game configuration
- Fix PiSNES fullscreen/integer-scale
- Fix demo screen ratio on non-16:9 screens
- Fix Arcade system not available in GUI
- Fix demo mode on PC (first game launched no longer runs indefinitely)
- Fix emulationstation reporting wrong free space in System menu
- Fix demo mode exit after the user pressed start
- Fix missing apple IIGS bios information in Manager and bios/readme.txt
- Fix diacritic characters in uppercase texts
- Fix atari800 core crashes
- Fix enable cheevos badge in retroarch menu (retroachievement.org)
- Fix advancemame availability on xu4 and x86 (not x64)
- Fix some retroarch core random crashes and/or lags/slowness
- Fix videosnaps crashs/artifects while scrolling
- Fix missing videosnaps keys in recalbox.conf
- Fix videosnaps crashes & white textures on x64 (Thanks to cpasjuste)


## Version 6.1

### News
- Pi 3A+ compatibility (Firmware bump)
- New system! Uzebox on all systems (RetroArch core: libretro-uzem)
- New system! Amstrad GX4000 added with libretro's cap32 emulator
- New system! Apple IIGS on all systems (Standalone: GSplus 0.14)
- New system! Spectravideo added with libretro's bluemsx emulator
- New system! Sharp X1 added with libretro's xmil emulator
- New system! Palm added with libretro's mu emulator
- New core! Added Gearsystem libretro core. An optimized GG/SMS/SG core working better on some games.
- New core! Added SameBoy libretro core. Game link support for GB/GBC
- New video snaps! Now see short videos of games before launching them
- GPi Case Plug & Play: Autodetect and install drivers & appropriate themes and settings
- New system! PC-88 added with libretro's quasi88 emulator
- New system! TIC-80 added with libretro's tic80 emulator
- New system! MSXturboR as own system. Generic msx system has been removed.
- New system! Multivision added with libretro's gearsystem emulator
- New system! Atomiswave added with libretro's flycast emulator (pc only)
- New system! NAOMI added with libretro's flycast emulator (pc only)
- New core! Added flycast libretro core. Dreamcast (et al.) emulator
- New system! Saturn added with libretro's bettle-saturn, yabause and yabasanshiro emulators (pc only)
- New configuration override system to fine tune all configuration per system, per folder or per game
- Easy AI Service configuration (Retroarch translation service)
- Add Arcade meta-system to group piFBA, FBN, MAME and Neogeo into a single system
- New core! Added UAE libretro core. (Experimental) Amiga emulator on all platforms
- New documentation available on gitbook: http://recalbox.gitbook.io (still WIP)

### Improvements
- Retroarch updated to version 1.7.8v3!
- Retroarch cheats updated to version 1.7.8!
- Switched default Retroarch UI to Ozone (except on GPI)
- Improved RGUI configuration on GPI
- Libretro core updated! FBA Libretro core updated to latest version (Neogeo CDRom Speed fix)
- Libretro core updated! Picodrive updated to latest version (Fix shifted down screen)
- Libretro core updated! Theodore updated to latest version (add emulation of Thomson MO6 and Olivetti Prodest PC128)
- Libretro core updated! Migrate Glupen64 to Mupen64Plus
- Libretro core split! Rebrand stella to stella2014 and add upstream stella core
- Emulator updated! ScummVM updated to September 2019 version (new theme included)
- Emulator updated! ResidualVM updated to September 2019 Version
- Emulator updated! Linapple-Pie now uses upstream repository
- Emulator updated! Oricutron updated
- Enable both hotkeys and I2C on RasberryPi GPIO
- Add .7z support to more emulators
- MoonLight updated! Add support for latest GForceExperience softwares
- New GUI Menu: In gamelist views, START opens game contextual menu. SELECT switch between all games/favorites only
- Add game information screen in demo mode
- Mame changes its default emulator to mame2003_plus
- Add option to confirm before exiting a game (libretro cores only)
- Shutdown System option moved at the top of the Quit menu
- Retroarch ratio resynchronized:
  - "Core provided" and "Retroarch Custom" added to ratio menu
  - Old "custom" ratio renamed "Do not set" (let the user choose in Retroarch)
  - Ratio text localized
- New theme folder in share root folder (old folders are still working)
- New pads added to known controllers: Moga Pro Power, Wiimote, Logitech RumblePad
- Bluetooth pads configuration easier than ever: previously configured pads automatically unpaired before pairing again
- Enable "Threaded DSP" option in 4DO by default (improve emulation speed)
- Odroid XU4 linux kernel upgraded to version 4.14
- pcsx_rearmed supports .chd files now too
- Improve volume balancing between RetroArch and Recalbox
- Add .zip and .7z extensions for N64 (only work with libretro-mupen64plus)
- RetroArch Disc Project: Groundwork for playing games directly from your CDROM
- Almost all core/emulators has been bumped again. Too much for the detailed list!
- ScreenScraper internal scraper is back with new options
  - Choose your image: Screenshot, Title, Front Case, 3D Case, Mix V1 and Mix V2
  - Use your ScreenScraper account to get higher per day limitations
  - Choose your favorite region/language to get appropriate images and texts
- TheGameDB internal scraper is back using newest APIs 
- Commodore 64 has now two emulators: x64 (speed) and x64sc (accuracy)

### Fixes
- Fix boot intro videos on x86 and x64
- Fix system unique key (fullname + platform)
- Fix demo mode using retroachievements
- Fix demo mode using autoload/autosave
- Fix non working pads in Dolphins
- Fix ScummVM not starting if no pad is configured
- Fix x86/x64 stuck in demo mode in some circumstances
- Fix support archive upload error
- Fix intro video fullscreen on x86/x64
- Fix steam controller driver
- Fix Amiga multi-disk bug with [] characters
- Fix Amiga WHDL loader
- Fix default scraper

## Version 6.0 - DragonBlaze

### News
- Raspberry Pi3b+ & CM3 compatibility
- Updated emulator names! catsfc -> snes9x2005, pocketsnes -> snes9x2002, snes9x_next -> snes9x2010, pce -> mednafen_pce_fast, vb -> mednafen_vb, imame -> mame2000, mame078 -> mame2003, fba -> fbalpha
- New emulator! Pokemon Mini on all systems (RetroArch core: libretro-pokemini)
- New emulator! Mattel Intellivision on all systems (RetroArch core: libretro-freeintv)
- New emulator! Fairchild Channel-F on all systems (RetroArch core: libretro-freechanf)
- New emulator! Atari Jaguar on x86/64 (RetroArch core: libretro-virtualjaguar)
- New emulator! MGT Sam Coupé on all systems (Standalone simcoupe)
- New emulator! Tangerine Oric/Atmos and later clones on all raspberry (Standalone oricutron)
- New emulator! Atari 8bits series on all systems (800, XL, XE) and Atari 5200 console (RetroArch core: libretro-atari800) - Thanks to Dubbows!
- New emulator! PC-FX emulator on x86 and XU4 systems (Retroarch core: libretro-beetle-pcfx)
- New emulator! PC-98xx series on all systems but odroidC2 (Retroarch core: libretro-np2kai)
- New System! SNES extensions Satellaview and SuFami Turbo added as own systems
- New System! NeoGeo CD with audio track support (via RetroArch core libretro-fba)
- New System! Amiga CD32 with audio track support (via Amiberry)
- New Core! Added mame2003-plus libretro core. A MAME078 version with added games support plus many fixes and improvements. DAT files available in rom folder
- New free game! "Mission: Liftoff" for Thomson emulator.
- New free game! "Flower" for Amiga emulator.
- New Capcom RB intro video
- New PCEngine intro video
- Add support for Mayflash GameCube adapter
- Add 7z file support for many libretro cores
- Add overlay possibility for advmame core
- Add new Screensaver "DEMO" to play continuously random games (Press START/ENTER to play the current game)

### Improvements
- KODI updated to 17.6
- Youtube plugin for KODI updated to 6.3.1
- Joypads management updated, mostly impacting joypads advertising as a complete keyboard.
- Preconfigured gamepads file rewritten
- x86: Now requires a CPU that can handle at least SSE and SS2 (Core2duo and higher)
- x86: Add support for newer AMD GPU
- Retroarch updated to version 1.7.6!
- Retroarch cheats updated to version 1.7.6!
- Emulator updated! 4DO updated to lastest revision. Also available on Rpi3 (3B+ recommended).
- Emulator updated! Amiberry (Amiga) - Huge up! Add supports of IPF, RP9, CD, zip, 7z & Retroarch pad configuration - Available also on Odroid XU4.
- Emulator updated! DoxBox updated to December 2018 version.
- Emulator updated! ScummVM and ResidualVM updated to January 2019 versions.
- Libretro core updated! 81 (EX-81) updated to latest version.
- Libretro core updated! Atari800 (Atari 5200Atari 8bits) updated to latest version.
- Libretro core updated! Beetle Lynx (Atari Lynx) updated to latest version. Support Headerless roms.
- Libretro core updated! Beetle NGP (Neo Geo Pocket/Color) updated to latest version.
- Libretro core updated! Beetle PCE (PCEngine-CD) updated to latest version.
- Libretro core updated! Beetle PSX (Playstation 1) updated to latest version.
- Libretro core updated! Beetle PSX-HW (Playstation 1) updated to latest version.
- Libretro core updated! Beetle SGX (PCE/PCE-CD/SGX) updated to latest version.
- Libretro core updated! Beetle VB (Virtual Boy) updated to latest version.
- Libretro core updated! Beetle Wonderswan (Wonderswan/Color) updated to latest version.
- Libretro core updated! BlueMSX (MSX) updated to latest version.
- Libretro core updated! Caprice32 (Amstrad) updated to latest version.
- Libretro core updated! Crocods (Amstrad) updated to latest version.
- Libretro core updated! DeSmuME (NDS) updated to latest version.
- Libretro core updated! FBA Libretro core updated to latest version. Recommended Romset: 0.2.97.44
- Libretro core updated! FCEUmm (Nintendo NES) updated to latest version.
- Libretro core updated! fMSX (MSX) updated to latest version.
- Libretro core updated! FreeIntV (Intellivision) updated to latest version.
- Libretro core updated! Fuse (ZX Spectrum) updated to latest version.
- Libretro core updated! Gambatte (Nintendo GB/Color) updated to latest version.
- Libretro core updated! Genesis Plus GX (Sega MD/GG/MS/CD) updated to latest version.
- Libretro core updated! GNupeN64 (Nintendo64) updated to latest version.
- Libretro core updated! GPSP (Nintendo GBA) updated to latest version.
- Libretro core updated! GW (Game & Watch) updated to latest version.
- Libretro core updated! Handy (Atari Lynx) updated to latest version.
- Libretro core updated! Hatari (Atari ST) updated to latest version.
- Libretro core updated! iMAME (Mame 2000) updated to latest version.
- Libretro core updated! Lutro (Lua Game Engine) updated to latest version.
- Libretro core updated! MAME2003 (Mame 2003) updated to latest version.
- Libretro core updated! MAME2010 (Mame 2010) updated to latest version.
- Libretro core updated! MelonDS (NDS) updated to latest version.
- Libretro core updated! mGBA (Nintendo GBA) updated to latest version.
- Libretro core updated! Nestopia (Nintendo NES) updated to latest version.
- Libretro core updated! NxEngine (Cave Story) updated to latest version.
- Libretro core updated! O2EM (Odyssey²) updated to latest version.
- Libretro core updated! PCSX (Playstation 1) updated to latest version.
- Libretro core updated! Picodrive (Sega MS/MD/CD/32X) updated to latest version.
- Libretro core updated! PocketSnes (Nintendo SNES) updated to latest version.
- Libretro core updated! PrBoom (Doom engine) updated to latest version.
- Libretro core updated! ProSystem (Atari 7800) updated to latest version.
- Libretro core updated! Px68k (Sharp X68000) updated to latest version.
- Libretro core updated! QuickNES (Nintendo NES) updated to latest version.
- Libretro core updated! Snes9x (Nintendo SNES) updated to latest version.
- Libretro core updated! Snes9x2005 (Nintendo SNES, formerly CatSFC) updated to latest version.
- Libretro core updated! Snes9x2010 (Nintendo SNES, formerly Snes9x-Next) updated to latest version.
- Libretro core updated! Stella (Atari 2600) updated to latest version.
- Libretro core updated! TGBDual (Nintendo GB/Color) updated to latest version.
- Libretro core updated! Theodore (Thomson) - Supports now TO8, TO8D, TO9, TO9+ & MO5 machines
- Libretro core updated! VecX (Vectrex) updated to latest version.
- Libretro core updated! Vice (Commodores 8bits) updated to latest version.
- GPIO arcade driver upgraded: now support I2C and GPIO reconfiguring from the command line
- EmulationStation: Now automatically reboots once the upgrade is ready
- "overlays" folder and roms subfolders automatically created
- Updated all MAME hiscore/cheat files in bios folder - Thanks to olivierdroid92!
- Static IP configuration available for WIFI connections (in recalbox.conf)
- Add support for AZERTY/QWERTY virtual keyboards
- ScummVM/ResidualVM now use Recalbox pad configurations

### Fixes
- bluetooth detection on Odroid XU4
- bluetooth recovery on Odroid XU4
- EmulationStation: better cyrillic display
- EmulationStation: fix scraped folders display
- MSX Emulator: Fixed Bluemsx core
- PS3/PS4 pads are working now
- SSH is now working when moving the share on Fat32/exFat partitions
- Recalbox sends 0000 to BT devices asking for pincode
- WIFI settings saved in a more secure way

---

## Version 18.07.13 - 2018-07-13

### Improvements
- Bump desmume to desmume2015
- EmulationStation: Arcade roms fullname in Netplay GUID
- EmulationStation: No game launch if core doesn't match

### Fixes
- SELECT as hotkey sometimes messy with arcade
- EmulationStation: remove netplay popup to prevent some crash
- Wifi: options were not saved with nfs cifs or configurations
- N64 rice & GlideN64: fix blank screen
