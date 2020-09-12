# Release notes

This file will be processed and will be distributed with Recalbox Releases.
The text, located between the first `## Version XXX` and the second one, will appear
as release notes for end user on a Recalbox upgrade.


## Version NEXT

### News
- Add TI-99/4A system, emulated using the ti99sim standalone emulator
- Add Dragon 32/64 system (XRoar standalone emulator)
- Add TRS-80 Color Computer (XRoar standalone emulator)
- Add genesisplusgxwide, the genesis/megadrive widescreen core!
- Add libretro fbneo core for channel f system
- Add libretro uae4arm core, an Amiga core optimized for ARM boards!
- Add stella2014 core on RPI1/Zero and RPI2 as it is more performant than stella
- Add recalbox overlays for all systems!
- Add LowRes NX virtual console on all boards
- Add CHD support for Mega CD Games with Picodrive core
- Add Saturn on Raspberry pi4 (libretro core: Yabasanshiro)
- Add gamelink overload for gb and gbc
- Add recalbox-wpaf tool for controlling various RPI fan controllers

### Improvements
- Bump libretro MGBA (Gameboy Advance core)
- Bump Libretro PX68K (Sharp x68000)
- Bump Oricutron to latest version
- Bump SimCoupé to v1.2.5
- Bump Retroarch to v1.9.3
- Bump Hypseus to version 2.4.8 (DirtBagXon) to support Singe
- Bump Fbneo v1.0.0.02
- Bump Sharp X68000 core (px68k)
- Bump Flycast
- Improve Netplay window
  - More responsive & faster to get client latencies
  - Manual refresh with START
  - Seamless auto-hashing when Netplay is on
- Bump Kronos libretro core
- Bump Mame 0.235
- Add new Pad-to-Keyboard capabilities:
  - Mouse emulation using joystick or dpad.
  - Keystroke sequences (like typing a word)
  - Keystroke combination (like CTRL+C)
- Bump ScummVM to 2.4.0git

### Fixes
- Fix message "failed to set last used disc" for core libretro px68k
- Fix laggy video playback on Odroid XU4 OGST
- Fix message "failed to extract content from compressed" for core libretro xrick


## Version 7.2.2-Reloaded

### Fixes
- Fix mupen64plus resolution (standalone)
- Make mupen64plus working on Odroid Advance Go2/Go Super (standalone)
- Make mupen64plus + RICE working on RPi4
- Fix ratio issues in Dolphin & Supermodel3
- Switch PUAE to A600 instead of A500PLUS for amiga600 system
- PSP (ppsspp) saves are now in (share/saves/PSP)
- Fix controler #2 configuration in swanstation
- Fix sound issue on boot video
- Audio is working on Sound Open Firmware audio adapters
- Added configuration for SN30-GP controller
- Fix OGST display not working on Odroid XU4
- Fix no boot black screen on some PCs
- Fix realtek wifi driver for rtl8812au and rtl8821au
- Fix missing rtl8821cu wifi drivers for Odroid XU4, RPI2, RPI3 and PCs
- Fix some bluetooth adapters not working when detected as HID device (like some Apple or Dell devices)
- Add Mupen64plus Next libretro core on PC platforms
- Bump Mupen64plus standalone emulator
- Fix Libretro Parallel N64 video plugin selection to Rice (better speed on selected H/W)
- Make Libretro Parallel N64 works on Odroid XU4
- Make Mupen64plus Rice works on Odroid XU4, Odroid GO Advanced and Odroid GO Super
- Fix black screen on Moonlight on RPI4
- Fix the time conflict between Windows and Recalbox in case of dual-boot
- Make Mupen64plus GLideN64 works on Raspberry PI3
- Fix MAME menu is displayed if you add credit with the wiimote
- Make Mupen64plus Glide64MK2 works on various boards
- Fix image display on dreamcast when using overlays
- Fix ratio selection for gamecube, wii and supermodel
- Bump supermodel to r862 and enable build optimisation


## Version 7.2.1-Reloaded

### Fixes
- Bring back fan speed setting on Odroid XU4
- Fix loop reboot with non official themes
- Lightgun system is shown by default if any game is detected
- Kodi video playback is back on Rpi 4
- Reicast has been removed from Rpi, PC and XU4 as this emulator does not work anymore with our new kernel and softwares versions. You can now use Flycast as the main emulator for the dreamcast.
- On PSP, with PPSSPP emulator, videos now run full speed
- Raspberry Pi 1,2,3 boot video now run full speed
- Fix arcade view systems missing model3, atomiswave, naomi, naomigd, daphné
- Duckstation has been renamed Swanstation
- Fix Swanstation issues when loading games
- Fix cavestory x86 and x86_64
- Fix House of the dead bios validation
- Add missings game extensions detection for SCV and WII
- Default language is now used when scraping
- Fix gameclip initialization on network shares
- Fix freezes when running a game while playing gameclip
- Using IPv4 in priority when connecting WIFI
- Factory reset now reset case configuration
- Fix rating componant running out of screen
- Fix sound menu & output auto-selection
- Fix Odroid Go Advance V1 gamepad detection
- Fix Super Cassette Vision scraping
- Fix rtl88x2bu Realtek wifi driver
- Fix "Forget all bluetooth devices"
- Fix pixel-perfect settings being incorrectly set in menu
- Fix update popup appearing over Gameclip Screensaver
- Fix hiding a game wipe all its metadata
- Fix theme display after switching theme or editing theme options
- Fix some files referenced in .cue/.gdi/.m3u not being ignored by the scraper
- Force volume of all outputs to 100%
- Fix crash when running a game from the search window
- Fix GBA gpsp emulator


## Version 7.2-Reloaded

### News
- Bump libretro-mame
- Bump Datfiles for Mame, Atomiswave, Naomi, NaomiGd, Model3 on Romset 0.230
- Bump of the port family
- Bump Dolphin Emulator to Version 5.0-13178
- Bump Buildroot to version 2020.02
- Bump Kernel to 5.8 (except on Odroid Go Advance)
- New multi-source rom management:
  - Pre-installed games are no longer copied into SHARE partition (except ports)
  - New option in game menu to show/hide pre-installed games
- New "GameClip" screensaver playing games' videos in many configurable ways
- Show feature slides during install
- Add Super Cassette Vision core
- Add Libretro NeoCD core (w/ CHD support)
- Add Nintendo Switch joycon support (both solo or combined mode)
- Add new libretro core for DOS games (dosbox-pure available on rpi3/4, x86/x64 & Xu4.Goa)
- Add Pulseaudio support
- Add PICO-8 core (libretro-retro8)
- Add Pocket Challenge v2 system (pcv2)
- Lightgun feature : more than 200 games already ready to play! (using dolphin bar/wiimote)
- Add Minecraft's port Craft on pc
- Add support for Odroid Go Super
- Add Watara SuperVision System on all boards
- Add Elektronika BK series on all boards
- Add VideoPac+ System
- Separate VIC-20 system from C64
- Add sound mode option (Music/Video sound)

### Improvements
- Bump supermodel to version 858
- Add frequency screen option for X86, X86_64 by Chriskt78
- Bump Amiberry to version 3.3
- Bump Stella (A2600) to fix lots of games (Pitfall II among many others)
- Set default overlay to vc4-kms-v3d on RPI4 (better 3D performance)
- Bump and Patch Beetle-Saturn for best support of european Virtua Cop games
- Patch Retroarch for X11 multi-mouse
- Flycast crossairs updated
- Improve menu ergonomy
- Improve internal scraper
- Bump libretro-uae adding CHD support
- Bump ScummVM and merge with ResidualVM
- Bump BlueMSX
- Bump rockchip-libmali (for odroidgo2 and 3)
- Bump Hypseus to version 1.3
- Bump Duckstation, PSX games now run full speed (for odroidgo2 and 3)
- Bump Fbneo
- Bump Libretro Mupen64plus Next
- Bump Odyssey²: Add support for european & french models
- Bump Np2kai NP21/W rev.79
- Bump nVidia video drivers, add support for RTX 20x0 and RTX 30x0
- Bump PPSSPP, PSP games now run far more quickly (for odroidgo2 and 3)

### Fixes
- Fix bypass configgen option for model3
- Fix reactivation of the deleted mesa 3d demos package
- Fix Dolphin emulator
- Fix Odroid Go Advance controller (v1)
- Check recalbox image integrity before upgrading
- Fix lost support of open and WPA-PSK-SHA256 wifi networks
- Fix 8Bitdo NES30 Pro rev 1 mapping (thanks @fredbcode)
- Fix name for palmos rom (thanks @RaZer0k)
- Fix 4k resolution handling on rpi4
- Fix shaders on odroidgo2
- Fix RetroArch shortcuts on odroidgo2
- Fix Libretro Parallel N64 core on odroidgo2 & Pi4
- Fix fan not stopping on shutdown on nespi4case
- Fix libretro-mame, now full speed

## Version 7.1.1-Reloaded

### Fixes
- Fix abnormally long boots
- Fix Sega Model 3 missing sound option
- Fix theme issues when switching themes or theme options
- Fix save required when exiting metadata edition w/o changing anything
- Fix popup settings not saved
- Fix quick system change setting not saved
- Fix Supermodel build and move Nvram path
- Fix Demo mode informùation screen
- Fix P2K hanging on game exit


## Version 7.1-Reloaded

### News
- Add model3 arcade system
- Add WPS support for quick WIFI configuration
- Recalbox is distributed on Noobs (again) and Raspberry Pi Imager
- Add Raspberry Pi400 support
- Add Odroid Go Advance board support
  - Run all emulators up to Naomi/Dreamcast (smoothly) or even Saturn/Atomiswave (not all games)
  - Full power-saving support (slow down or speed up CPU/GPU/Ram regarding the running emulator)
  - Suspend/Soft Power-off using POWER button
  - Automatic audio switch between speakers/headpĥones
  - Volume & Brightness buttons
  - First-boot wizard
- Add 8 Broke Studio's game demos on NES!
- Add 4 MAME games from the '80

### Improvements
- Improved NESPi4 case detection
- Libretro-atari800 with .car extension support
- Check free space before downloading updates
- Bump Picodrive
- Bump FBNeo to v1.0.0.0
- Add FBNeo as core for
  - Colecovision
  - Gamegear
  - Famicom Disk System
  - Master-System
  - Megadrive
  - Msx 1
  - NES
  - NeoGeo Pocket
  - PC-Engine
  - SG-1000
  - SuperGraphx
  - ZX-Spectrum
- Add overlay global/per-system settings support in share/overlays
- Recalbox is back in Window's Network
- Update BIOS database
- Improved Flashback support: just copy your disk or CD game into roms/ports/flashback
- Bump Kodi to 18.9

### Fixes
- Fix AdvanceMAME
- Fix freezes on megadrive/picodrive
- Fix broken WIFI on some Intel chips
- Bring back all firmwares
- Fix boot from SSH/HDD on rpi4
- Fix arcade hash calculations
- Fix game extension filtering
- Fix screenshots on x86/x64
- Fix multiple issues in ScummVM/ResidualVM: lag/crash on Pi4, joystick buttons, ...
- Fix upgrade procedure that could fail in some circumstances
- Fix battery icon on PC
- Display p2k hints properly, including folders's p2k files
- Fix Vic20 games not running
- Fix inconsistent MD5 on multi-file zipped bios (neogeo.zip, ...)
- Fix gamelist bein overwritten when edited from outside Emulationstation
- Fix scrapper settings not always being saved
- Fix VirtualKeyboard on resolution higher than 720p
- Fix UAE configuration for CDTV/CD32
- Fix UAE kickstart copy for WHDL games
- Fix PSP mapping configuration
- Fix Mupen64 resolution on x86/x64


## Version 7.0.1

### News
- Add Sigil in PORTS (https://www.romerogames.ie/si6il)
- Add Kubo 3 on NES (dale_coop & seiji)
- Add raspi-gpio to manage GPIO in scripts
- Add new boot videos (GBA & Amiga)
- Add missing bios report file (share/bios/missing_bios_file.txt)

### Improvements
- SNES default emulators pi0/1: pisnes, pi2/3: snes9x2010 & snes9x elsewhere
- Bump FBNeo - Emulation improvements + fix neogeo-CD audio artifects
- Bump linux-firmware to 2020.09.18
- Bump kodi to 18.8
- Make pc98 playable with pads/joysticks

### Fixes
- Fix Missing dat files mame, naomi(Gd), atomiswave
- Fix Netflix plugin authentication (bump to v1.10.0)
- Fix RPI4 blackscreen issues while booting
- Fix doom.wad required for WADs games (prboom core)
- Fix Mupen64 L1/R1 mapping
- Fix DosBOX not returning back to emulationstation
- Fix pads in reversed order by default
- Fix recalbox.conf modifications getting lost
- Fix missing media being not updated using internal scrapper
- Fix outdated bios list in webmanager
- Fix zfast and crt-pi shaders' path
- Fix "Last Played" metadata & display in gamelist
- Fix missing psx bios md5
- Fix hdmi-cec not working on rpi4
- Fix PS4 pad pairing issue
- Fix duplicated entries in "Last Played"
- Fix Ports & aAcade virtual system not searchable
- Fix empty card names in audio output device list
- Fix no sound through jack on RPi1/2/3
- Fix information popup positions
- Fix Oricutron bios path
- Fix false update message
- Fix X-Arcade driver. Now supports v2 panels and other keyboard encoders
- Fix ShaderSets
- Fix mouse not working on rpi4
- Fix multi-file zipped roms hash calculations (arcade)
- Add more hard-patched xbox pads
- Fix idroid pad for kodi
- Fix connected/disconnected status in Network menu
- Fix i18n plural forms (Chineese language are now working)
- Fix video player behavior while reading invalid files
- Fix NESPi4 case shutdown script
- Fix VirtualKeyboard red mess on Pi4
- Fix crash while quitting gamelist menu in virtual systems
- Fix main menu still accessible while it should not be.
- Fix crash in p2k hints with some p2k definitions
- Fix GPI case shutdown script
- Fix screensaver triggered while downloading updates
- Fix rpi4 random freeze


## Version 7.0

### News
- New compatible board: Raspberry Pi4
- Recalbox system is now a firmware, more robust than ever:
  - Automatic recovery after 3 boots failure (reset to factory settings)
  - Brand new update system, easier and faster
- Share partition is created in exfat on fresh install. Access your share partition from your Windows PC!
- Add BIOS Window in EmulationStation to list missing/incorrect Bios:
- Add new BIOS XML list aware of mandatory/optionnal bios and multiple working Bios signatures.
- Add sorting options: Publisher and system-name for all virtual systems **
- Add 3 new virtual systems: All-games, Last-Played and Multiplayers **
- Add virtual systems per genre (RPG, Shoot'em up, Pinballs, ...) **/*
- Add Region highligting in gamelist (highlight games from your favorite region) **
- Add Adult game filtering in EmulationStation **
- Add laptop switch to external screen (x86/x64)
- Add EmulationStation event-driven user scripts
- Add Search feature (with a brand new Arcade-style virtual keyboard!)
- Add Pad-To-Keyboard driver. Play keyboard computer games with your pad!
- Add License menu
- Add support for nVidia proprietary drivers version 390 and 440 (x86/x64)
- Add Arcade virtual system in EmulationStation
- Add automatic detection & management of RetroFlag's NESPi4 case
- Add animation while creating/populating SHARE partition
- Add BIOS check *before* running a game.
- Add Screenshot from the Webmanager for Xu4 and x86/x64.
- Add new Netplay-able systems: Atari2600, PCEngine CD, PC-FX, Family Disc System, TIC-80, Sega CD & Mr.Boom
- Add new systems:
  - Add Nintendo 64DD platform
  - Add OpenBOR (Beat'em up engine)
  - Add Solarus (RPG engine)
  - Add EasyRPG (RPG engine, using libretro-easyrpg, compatible w/ RPG Maker 2000 & 2003)
  - Add naomigd system for your NAOMI GD-ROM games
  - Add Amiga CD32 on x86/x64 (w/ libretro-puae core)
  - Add Naomi on Pi3 (w/ libretro-flycast)
- Add new emulators/cores:
  - Add libretro's pcsx_rearmed on pc too
  - Add libretro's mupen64plus-nx, an improved n64/64dd emulator (rpi only for now)
  - Add libetro's Mesen, accurate NES & FDS emulator (rpi4, xu4, x86/x64 only)
  - Add libetro's Mesen-S, accurate SNES, Satellaview, GB/GBC & Super GameBoy emulator (rpi4, xu4, x86/x64 only)
  - Add libretro mame for recent mame set
  - Add libretro-race, an ngp & ngpc emulator focused on performance
  - Add Libretro-flycast on on rpi3, xu4 and x86
- Add PORTS system:
  - CaveStory moves to ports and is now included, ready to play
  - Add Mr. Boom (8-player Bomberman clone), ready to play
  - Prboom (Former DOOM system) moves to ports
  - Add Quake 1 game engine, ready to play
  - Add 2048 game, ready to play
  - Add Dinothawr game, ready to play
  - Add XRick (Rick Dangerous clone) game, ready to play
  - Add Flashback game. Requires full game files to be added (read the .txt)
  - Add Wolfenstein 3D game, ready to play
  - Add Out Run game. Required arcade rom to be played (read the .txt)
- Add support for popular music formats in EmulationStation
  - MP3 files
  - High quality FLAC files
  - OGG OPUS
  - Amiga Modules (and all derivatives traker formats)
  - Wave (Raw audio format)
  - MIDI file (soundfont file required)
    More information available in share/music/readme.txt
- Add new "networkable" folders in share (overlays, shaders, scripts, screenshots)
- Add *LOTS* of awesome homebrews (more info in related subfolders):
  - A2600: Arguna (Nathan Tolbert)
  - A2600: Halo2600 (Ed Fries, former VP of Microsoft XBox division)
  - A2600: The End 2600 (Rayman_C)
  - A5200: BeefDrop (Ken Siders)
  - A7800: Meteor Shower (Robert DeCrescenzo, exclusively licensed to recalbox)
  - A7800: BonQ (Ken Siders)
  - Amstrad CPC: UWOL 2 (The MojonTwins)
  - Amstrad CPC: The Dawn Of Kernel (Juan J. Martinez)
  - Amstrad CPC: Relentless (Axelay)
  - Amstrad GX4000: Baba's Palace (Rafael Castillo, John McKlain & Dragon)
  - Apple II: Mystery House (Ken & Roberta Williams)
  - Apple II: Lamb Chops (TanRuNomad)
  - Apple II: Retro Fever (TanRuNomad)
  - Atari8bits: Crownland (Piotr Wisniewski)
  - Atari8bits: Ridiculous Reality (Matosimi)
  - Atari8bits: Time Pilot (Solo & Laoo)
  - C64: Wolfling (LazyCow)
  - C64: Transe Sector Ultimate (Richard Richard Bayliss)
  - C64: Vortex Crystals (Richard Richard Bayliss)
  - C64: X-Force (Richard Richard Bayliss)
  - C64: Zap Fight 2 Special Edition (Richard Richard Bayliss)
  - C64: Rescuing Orc (Juan J. Martinez)
  - C64/Plus/4: Adventure in Time (Kisnémeth Róbert)
  - C64/Plus/4: Adventure Park (Varga Gábor)
  - C64/Plus/4: Digiloi (Tero Heikkinen)
  - C64/Plus/4: XPlode-Man (Varga Gábor)
  - Colecovision: MazezaM (PortableDev)
  - Gameboy: Into The Blue (Jonas Fischbach)
  - Gameboy: Retroïd (Jonas Fischbach)
  - Gameboy: Tuff (Ivo Wetzel)
  - Gameboy Advance: Lindsi Luna Blast (PortableDev)
  - Gameboy Color: Wing Warriors (Kitmaker Entertainment & Francisco Javier Del Pino Cuesta)
  - GameGear: Wing Warriors (Kitmaker Entertainment & Francisco Javier Del Pino Cuesta)
  - Intellivision: Princes Lidie (Marco Marrero)
  - Intellivision: Deep zone (Artrag)
  - Intellivision: Hotel Bunny (Sebastian Mihaï)
  - Lynx: Nutmeg (RETROGURU & PHOTON STORM)
  - Lynx: Silas' Adventure (Krzysztof Kluczek)
  - Master System: Wing Warriors (Kitmaker Entertainment & Francisco Javier Del Pino Cuesta)
  - Master System: Astro Force (Enrique Ruiz)
  - Master System: Silver Valley (Enrique Ruiz)
  - Master System: Galactic Revenge (Enrique Ruiz)
  - Master System: Papi Commando in CPP Land (Studio Vetea)
  - Megadrive/Genesis: Old Towers (Retrosouls)
  - Megadrive/Genesis: Misplaced (Retrosouls)
  - Megadrive/Genesis: Yazzie (Retrosouls)
  - Megadrive/Genesis: Gluf (Retrosouls)
  - Megadrive/Genesis: Papi Commando Remix Deluxe (Studio Vetea)
  - Megadrive/Genesis: Bomb on Basic City (Studio Vetea)
  - Megadrive/Genesis: L'Abbaye des Morts (Mun)
  - Megadrive/Genesis: Bug Hunt (Mun)
  - Megadrive/Genesis: Griel's Quest (Mun)
  - MSX: Wing Warriors (Kitmaker Entertainment & Francisco Javier Del Pino Cuesta)
  - MSX: Yazzie (Retrosouls)
  - MSX: XSpelunker (Santi Ontañón Villar)
  - MSX: XRacing (Santi Ontañón Villar)
  - MSX: Night Knight (Juan J. Martinez)
  - MSX2: The Sword of Ianna (RetroWorks)
  - MSX2: Brunilda (Retroworks)
  - NES: Nova The Squirrel (Novasquirrel)
  - NES: Cheril Perils Classic (MojonTwins)
  - Oric/Atmos: Pulsoids (Twilighte)
  - Oric/Atmos: Oricium (Jose Maria Enguita)
  - Pokemini: Cortex (Simon Bradley)
  - Pokemini: Galactix (Lupin, Okkie & p0p)
  - Pokemini: Psychic Seeds (JustBurn, Palkone & Loather)
  - ScummVM: Beneath the Steel Sky (Revolution Software Ltd.)
  - ScummVM: Flight of the Amazon Queen (John Passfield & Steve Stamatiadis)
  - ScummVM: Lure of the Tempress (Revolution Software Ltd.)
  - ScummVM: Soltys (Laboratorium Komputerowe Avalon)
  - SG1000: Cheril Perils Classic (MojonTwins)
  - SNES: UWOL Quest For Money (PortableDev - MojonTwins)
  - Solarus: YarnTown (Max Braz)
  - TIC80: 8 Bit Panda (Bruno Oliveira)
  - TIC80: Shadow Over The Twelve Lands (Bruno Oliveira)
  - TIC80: Cauliflower Power (Librorumque)
  - TIC80: Todor Saved Ludmilla (Andraaspar)
  - TIC80: Secret Agents (msx80)
  - Vectrex: VecZ (LA1N.CH)
  - Vectrex: Thrust (Ville Krumlinde)
  - ZX Spectrum: Cray-5 (RetroWorks)
  - ZX Spectrum: The Sword of Ianna (RetroWorks)
  - ZX Spectrum: Gommy, Medieval Defender (RetroWorks)
  - ZX Spectrum: Genesis, Dawn of the Day (RetroWorks)
  - ZX Spectrum: Brunilda (Retroworks)
  INFO: Some of these games are available on physical supports.
        If you like it, own the appropriate hardware, and wish to support their authors,
        you can buy the physical package, most ofen for a very reasonable price.
  WARNING: Most of these games are still copyrighted by their respective authors.
           Some of them are free to distribute, some others are not.
           You're strongly invited to read carefully all text files and/or pdf in
           games folders. Thank you.

### Improvements
- Bump Buildroot to version 2020.02
- Bump KODI to Leia 18.7.1:
  - 4k x265 available on Pi4, x86/x64
  - Netflix plugin included (need manual activation)
- Bump cores/emulators:
  - Bump DosBox to r4290
  - Bump ScummVM and map D-pad when no analog available
  - Bump ResidualVM
  - Bump Dolphin emulator
  - Bump libretro-cores family
  - Bump oricatmos emulator on lastest version
  - Bump simcoupe emulator
  - Bump Amiberry to v3.1.3.1
  - Bump AdvanceMame to v3.9
  - Bump Theodore core (add emulation of Thomson TO7 and TO7/70 computers)
- Bump retroarch to v1.9.0
- Bump retroarch-cheats to v1.9.0
- Bump libretro-assets on last version
- Bump Odroid UBOOT to 2017/05 version
- Move x86/x64 graphic backend from GLES to OpenGL. Improve overall quality.
- Improved NetPlay:
  - Removed filtering of non-Recalbox players in netplay game lists
  - Add spectator mode management
  - Add password-protected game management (both player & spectator)
  - Add 15 pre-configured & editable passwords for quick selection
  - Available netplay games sorted by state/name
  - Add Recalbox icon in front of game names
  - Add Password icon for password-protected games
  - Add automatic core switch on client side to match host selected core
- Improve sound management:
  - Simplified output device selection
  - Volume control is working for boot-videos
- Rename Fba_libretro to Fbneo (updated to the new official logo)
- Rename 4do to Opera (newer libretro 3do core)
- Manage GameCube Bios
- Improve internal Scraper (ScreenScraper):
  - Faster! Use your ScreenScraper's threads to parallelize workloads
  - Better! Lots of options, snaps video, and more...
  - Stronger! Improved reliability and strongness
- Improve overall Emulationstation's stability & reliability
- Optimize memory requirements of EmulationStation
- Optimize EmulationStation boot time (up to 20 times faster!)
- Improve pad processing in EmulationStation. Perform auto-mapping of 70% of commonly available pads (incluxing all 8BitDo)
- Improved Genre & Region processing in EmulationStation *
- Improved translations:
  - Fix some bad translations in FR, ES, PT, IT & DE texts
  - Fix missing translations in FR, ES, PT, IT & DE texts
- Faster-than-light CRC calculation for netplay
- Game sorting and Jump-to-letter are now unicode compatible
- Simplify emulator/core selection UI
- Set vice_x64sc as default c64 emulator + JiffyDOS support
- Improve the management of external screens and selection from recalbox.conf (by Chriskt78)
- Improve Apple IIGS slot detection & auto-boot
- Improve the following standalone emulators:
  - AdvanceMame: Support roms in sub-folders, integer scale & show FPS
  - Amiberry (Amiga): Center screen, show FPS (LED bar) & Support scanline shader
  - DosBox (DOS): Support retro & scanline shaders
  - GSPlus (Apple IIGS): Support scanline shader
  - Linapple (Apple II): Support all screen resolutions
  - Oricutron (Oric): Support scanline shader
  - Simcoupé (SAM Coupé): Support scanline shader & smooth rendering
  - ScummVM: Support scanline shader & smooth rendering
- Auto discover and configure multi-disk games for:
  - Amiberry (Amiga): up to 4 disks loaded at once
  - GSPlus (Apple IIGS): from 2 up to 32 disks loaded at once, depending on floppy types
  - Quasi88 (PC88): up to 6 disks loaded at once
- Improve GPI support (boot image, installation phases, ...)
- Improve shader management in EmulationStation (Add raw shader selection)
- Move shaders into user's SHARE partition
- Move Libretro's cheat into SHARE partition
- Improved WIFI & Bluetooth management

### Fixes
- Fix Odroid XU4 fan issue
- Fix Odroid XU4 sound issue
- Fix EMMC boot on XU4
- Fix x86/x64 Boot videos
- Fix music popup crashes
- Fix music loop play
- Fix netplay popup crashes
- Fix Apple2 not working on x86/x64
- Fix fullscreeen on Oric/Atmos and Apple2 on x86/x64
- Fix duplicate folders in Arcade systems
- Fix quit menu not being accessible when boot-on-gamelist is on
- Fix some bad behaviors in favorite management
- Fix reboot Emulationstation with webmanager
- Fix volume issues in boot video
- Fix REICAST bug on XU4
- Fix FPS show/hide in Retroarch
- Fix multiple audio issues
- Fix Gamelist reset when editing Metadata
- Fix multiple crashes in EmulationStation

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
