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