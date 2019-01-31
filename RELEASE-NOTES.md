# Release notes

## Work in Progress - Unreleased

## News
- Raspberry Pi3b+ & CM3 compatibility
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
- New Capcom RB video intro
- Add support for Mayflash GameCube adapter
- Add 7z file support for many libretro cores
- Add overlay possibility for advmame core

### Improvements
- KODI updated to 17.6
- Youtube plugin for KODI updated to 6.3.1 
- Joypads management updated, mostly impacting joypads advertising as a complete keyboard.
- Preconfigured gamepads file rewritten
- x86: Now requires a CPU that can handle at least SSE and SS2 (Core2duo and higher)
- x86: Add support for newer AMD GPU
- Emulator updated! Amiberry (Amiga) - Huge up! Add supports of IPF, RP9, CD, zip, 7z & Retroarch pad configuration - Available also on Rpi1 and Odroid XU4.
- Emulator updated! Theodore (Thomson) - Supports now TO8, TO8D, TO9, TO9+ & MO5 machines
- Emulator updated! FBA Libretro core updated to latest version. Recommended Romset: 0.2.97.44
- Emulator updated! Hatari (Atari ST) updated to latest version.
- Emulator updated! DoxBox updated to December 2018 version.
- Emulator updated! 4DO updated to lastest revision. Also available on Rpi3.
- GPIO arcade driver upgraded: now support I2C and GPIO reconfiguring from the command line
- EmulationStation: Now automatically reboots once the upgrade is ready
- "overlays" folder and roms subfolders automatically created
- Updated all MAME hiscore/cheat files in bios folder - Thanks to olivierdroid92!

### Fixes
- bluetooth detection on Odroid XU4
- bluetooth recovery on Odroid XU4
- EmulationStation: better cyrillic display
- EmulationStation: fix scraped folders display
- MSX Emulator: Fixed Bluemsx core
- PS3/PS4 pads are working now
- SSH is now working when moving the share on Fat32/exFat partitions

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
