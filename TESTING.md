# Testing
All changes that must be tested will be documented in this file.
Every case must be checked before a release.

## [Next]
- [ ] Amiberry Bump => Deeply retest amiberry: test all rom types, check default configuration, ...

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