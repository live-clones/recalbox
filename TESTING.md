# Testing
All changes that must be tested will be documented in this file.
Every case must be checked before a release.

## [Next]
- [ ] Amiberry Bump => Deeply retest amiberry: test all rom types, check default configuration, ...
- [ ] Packages that require libgo2 compiles well
- [ ] /usr/lib/libgo2.so should provides go2_ symbols
- [ ] New roms management => Test option in game menu to show/hide preinstalled games
- [ ] New roms management => Fresh install: Test all ports
- [ ] GameClip => Test gameclip screensaver with some available videos => Test all options
- [ ] GameClip => Test gameclip screensaver with no video (fresh install?) or by selecting only systems with no videos at all
- [ ] New slides during install => Fresh install on all patforms. All platforms must show the 8 slides, except on GPI which keeps the "creating share" video
- [ ] Kernel bump => test every system thing works on rpi1, rpi2 and rpi3
- [ ] Added rpi-400 dts => check pi400 boots correctly
+ [ ] mupen64plus bump => test standalone core deeply on all boards
+ [ ] Enabled mupen64plus with rice for odroidgo2 => test it works thoroughly
+ [ ] Check that mupen64plus standalone with gliden64 fully works on rpi2 and rpi3
+ [ ] Verify wm8960 audio hat works for all boards with kernel 5.4
- [ ] Bump Stella => Deeply retest Stella core
- [ ] Test & evaluate MelonDS on Pi4
- [ ] Test & evaluate libretro-mupen64plus-next on odroidxu4
- [ ] Validate libretro-mupen64plus-next compiles on all boards
- [ ] BR2020.11 - Check hyperion still works
- [ ] BR2020.11 - Test libretro-fmsx
- [ ] BR2020.11 - Test kodi-audiodecoder-timidity
- [ ] BR2020.11 - Test libretro-mupen64plus-nx on rpi2 (as libretro-mupen64plus is disabled)
- [ ] BR2020.11 - Test libretro-gpsp
- [ ] BR2020.11 - Test advancemame
- [ ] BR2020.11 - Test libretro-mu
- [ ] BR2020.11 - Check libretro-pcsx_rearmed works properly (bumped)
- [ ] BR2020.11 - Test moonlight-embedded
- [ ] BR2020.11 - Test openbor
- [ ] BR2020.11 - Test mpv (splash video) still works
- [ ] BR2020.11 - Test ppsspp
- [ ] BR2020.11 - Test libretro-81
- [ ] BR2020.11 - Check video (spash video) is displayed on boot


## [7.1-Reloaded]
- [X] Bumped picodrive to fix rewind on megadrive => test the rewind on megadrive/picodrive
- [X] Clock should update properly
- [X] Battery icon should show and update properly on PC when battery data are available, especially for a pc with /sys/class/power_supply/BAT1
- [X] Test audio output persistancy in ES => change output, reboot ES, check if still selected, once again
- [X] Check that a P2K file is visible in UI => create a .p2k.cfg file in a rom directory, then check that p2k hints are available for all games in this folder
- [X] Check network menu translations especially for WIFI (ON/OFF/WPS)
- [X] Check regional system logo => switch theme, reboot, check that regionalized logo are shown properly (GB, SNES, ...)
- [X] Fixed the composite MD calculations => Test if neogeozip bios files are giving the same MD5 with different zip compressions
- [X] Kodi executable check => Check if all kodi menu are visible & working on random board, but GO2.
- [X] Kodi executable check => Check if all kodi menu are NOT visible on GO2
- [X] Virtual Keyboard => check if dpad + J1 move the wheel and j2 moves the cursor
- [X] Fixed VIC20 extensions => Run games for VIC20 with any extension but .20 .40 .60 .a0 .b0.
- [X] Fixed scrapper settings not being saved => Set global scrapper settings without running a global scrapping. Instead, run a single game scrapping and check results accordingly
- [X] Fixed gamelist.xml being overwritten when edited from outside ES => Run ES, play a game, edit a gamelist from the network share, wait for ES to detect the change, let Es relaunch, then check if your modification is still there
- [X] Fixed puae kickstart copy => test WHDL with uae
- [X] Fixed puae cd32 & cdtv => run an amiga600 games with UAE, then run a CD32 game, then run a CDTV game (in this exact order). All games must work
- [X] Fixed VK on 1080p and more => Open VK and use DPAD's up & down to change charset several times. No glitches & no messed up screens should occur
- [X] Recalbox is back in Window's Network => Switch on recalbox, wait for ES, wait a few more seconds, a new machine called 'RECALBOX' should pop in your Network
- [X] Flashback core improved => Test flashback
- [X] PSP mapping generator rewritten => Test PSP games with all available pads
- [X] Pi400 support => Boot on Pi400 using Pi4 image
- [X] Check that kodi does not crash upon loading
- [X] Test all broke studio games!

WHDL with PUAE must be deeply tested. Some bioses still don't load.
