# Testing
All changes that must be tested will be documented in this file.
Every case must be checked before a release.

## [Next]
- [ ] Bumped picodrive to fix rewind on megadrive => test the rewind on megadrive/picodrive
- [ ] Clock should update properly
- [ ] Battery icon should show and update properly on PC when battery data are available, especially for a pc with /sys/class/power_supply/BAT1
- [ ] Test audio output persistancy in ES => change output, reboot ES, check if still selected, once again
- [ ] Check that a P2K file is visible in UI => create a .p2k.cfg file in a rom directory, then check that p2k hints are available for all games in this folder
- [ ] Check network menu translations especially for WIFI (ON/OFF/WPS)
- [ ] Check regional system logo => switch theme, reboot, check that regionalized logo are shown properly (GB, SNES, ...)
- [ ] Fixed the composite MD calculations => Test if neogeozip bios files are giving the same MD5 with different zip compressions
- [ ] Kodi executable check => Check if all kodi menu are visible & working on random board, but GO2.
- [ ] Kodi executable check => Check if all kodi menu are NOT visible on GO2
- [ ] Virtual Keyboard => check if dpad + J1 move the wheel and j2 moves the cursor
- [ ] Fixed VIC20 extensions => Run games for VIC20 with any extension but .20 .40 .60 .a0 .b0.
- [ ] Fixed scrapper settings not being saved => Set global scrapper settings without running a global scrapping. Instead, run a single game scrapping and check results accordingly
- [ ] Fixed gamelist.xml being overwritten when edited from outside ES => Run ES, play a game, edit a gamelist from the network share, wait for ES to detect the change, let Es relaunch, then check if your modification is still there
