# Testing

This file is in french as all our beta testers are french.


A chaque modification du système, les développeurs ajoutent une ou plusieurs lignes dans ce fichier.

Les beta testeurs cochent les cases lors des sessions de tests.

Toutes les cases doivent être cochées avant la release stable.

Pour les pending features, elles ne seront activées que si les beta testeurs les valident selon les critères de qualité de Recalbox.

## [NEXT]
- [ ] Check recalbox.log contains script and type information (#1920)
- [ ] Checking .bin files are seen in the gamegear system (#1860)
- [ ] Checking .d64 and .tap files are seen in the vic20 system (#1858)
- [ ] Test Supermodel and gamecube, wii ratio global and by system (#447)
- [X] While ES is stopped, create a file in /tmp/upgradefailed and check if the error message is displayed when starting ES (#1772)
- [X] Grub options still accessible on PC (#1750)
- [ ] Test uae4arm on any arm board (including CD platforms)(#1771)
- [ ] Bump Hypseus (#1611)
  - [ ] Test hypseus on all boards
  - [ ] Test singe games
  - [ ] Test overlay working with dragon lair's
- [ ] Test a sharp x68000 game and see if there is no message "failed to set last used disc" (#1775)
- [ ] Bumped px68k (Sharp x68000) (#1775)
- [ ] Get an es_systems.cfg for your board (7.2.2 max) and compare emulators/core available in this file to those available in this beta. They should all be there :) (#1686)
- [ ] Test stella2014 on rpizero and rpi2, should be full fps (#1761)
- [ ] Bumped Stella : Test stella core on all plateforms (#1761)
- [ ] Test genesisplusgx wide screen core all plateforms (#1783)
- [ ] Default Overlays (#1787)
  - [ ] Configuration => global.overlays should work, system overlaod too (snes.overlays=0)
  - [ ] Board => should not show overlays on gpicase, goadvance
  - [ ] Screens => should not show overlays when in 4/3 resolution
  - [ ] Ratio => should not show overlays when game ratio is 16/9 or 16/10
  - [ ] Custom overlays => custom overlays (in share) should always have priority
- [ ] Check that the retroachievement value is present in the system.xml file (#1799)
- [ ] Bumped libretro snes9x2002 core, test especially on rpi zero (#1757)
- [ ] Bumped libretro snes9x2005 core, test especially on rpi zero (#1757)
- [ ] Bumped libretro picodrive core, test especially mega cd chd support (#1757)
- [ ] Bumped Flycast : Test flycast core on all plateforms (#1737)
- [ ] Test random path support for emulationstation themes (myst theme) (#1793)
- [ ] Test bluetooth pairing still work (#1800, #1482)
- [ ] EmulationStation should always start (no phantom screen) (#1774)
  - [ ] if ES does not start, check pulseaudio is started with: `ps aux |grep pulseaudio` and post `/var/log/messages` log
- [ ] Video playback on odroidxu4 is not laggy (#1811)
- [ ] Test Tic-80 game downloader works (#1829)
- [ ] Test that recalbox.local and http://recalbox works again (#1826)
- [ ] Test pisnes core works correctly (#1838)
- [ ] Check that ES starts on odroidxu4 (#1815)
  - [ ] Check standalone emulators starts on odroidxu4 (#1815)
- [ ] Check that ES starts on rpi fresh installation (#1815)
- [ ] Check that ES starts on odroidxu4 (#1815)
  - [ ] Check standalone emulators starts on odroidxu4 (#1815)
- [ ] Check that ES starts on rpi fresh installation (#1815)
  - [ ] /dev/dri/ should exists on rpi4
- [ ] Custom overlays (dtbo found in /boot/overlays/) should be present (#1867)
- [ ] Sprite limits options should not be set anymore for retroarch cores (check retroarch-core-options.cfg) (#1842)
- [ ] Test Saturn on RPI4
- [ ] Bumped libretro kronos. No loss of fps (#1849) and remove build x86

- [ ] Test DS4 joystick works right after starting a rom (#1845)
- [ ] Bumped libretro fbneo (#1859)
  - [ ] Test the floppy disk change on FDS
  - [ ] Check fbneo core works on all boards. No loss of fps, even on pizero.
- [ ] Test Netplay lobby window
  - [ ] Enter/Exit the lobby window: should be almost "instant" now
  - [ ] Test manual refresh
  - [ ] Once the netplay is on, newly added romw should be hashed automatically in the next reboot
  - [ ] When setting Netplay on, ES should check if some roms need to be hashed: Set netplay off, then add roms, then refresh lists, then set netplay on
- [ ] Check there are no slowdown on x86 and x86_64 due to new compression method (#1848)
- [ ] Test vanilla image on rpi1 and rpi zero, experience should be good (#1841)
- [ ] Check no message "sram appears to be faulty" for game demons of asterborg on megadrive (#1844)
  - [ ] Check the libretro picodrive core from the bump.
- [ ] Bumped Scummvm version 2.4.0. Check that there is no regression (#1884)

  - [ ] Test EmuTOS bios for Atari ST (#1868)
- [ ] Check out the game Rick Dangerous (xrick) (#1891)
  - [ ] Bump core libretro Mame in 0.235. Test on x86 and x64 (#1889)
- [ ] Test the TI-99/4A system & emulator. Only cartridges (.ctg) are supported (#1856)
- [ ] Test libretro-MGBA
- [ ] Text Libretro PX68K and test .m3u multi-disk
- [ ] Bump libretro dosbox-pure core to 0.16 (#1644)
- [ ] Test P2K v2
  - [ ] Test Mouse emulation on many systems (both joystick & dpad)
  - [ ] Test keystroke sequences (type a whole word in a game or in a micro-computer basic)
  - [ ] Test keystroke combinations (you may try CONTROL+C in some 8bit games)
- [ ] Test the TRS-80 Color Computer system (XRoar emulator)
- [ ] Test the Dragon32/64 system (XRoar emulator)
- [ ] Test wifi connection by WPS is working (#1909)
- [ ] Check if the text "No comment available" is correctly spelled (#1911)
- [ ] Test libretro-MGBA  
- [ ] Test Oricutron
- [ ] Test SimCoupé
- [ ] Test gamelink overload for gb and gbc (put two player games in the gamelink folder. Tetris example) (#1808)
- [ ] Enable hat.wpaf.enabled recalbox.conf key and set a supported RPI board name (#1832)
  - [ ] hat configuration should be set accordingly (usually /boot/recalbox-user-config.txt)
  - [ ] fan should be controlled by wpaf
- [ ] Check there is no Dreamcast/Atomiswave/Naomi/Naomi GD on x86 (#1902)

## [7.2.2]
- [X] Test ppsspp saves are copied in share on upgrade (#1773)
- [ ] Test standalone mupen64plus on rpi and odroidxu4 boards (#1712)
- [ ] Test libretro-parallel-n64 with default conf on rpi and odroidxu4 boards (#1712)
- [ ] Test mupen64plus rice with default conf on odroidgo2 and odroidxu4 boards (#1712)
- [ ] Test moonlight on rpi4/kms (#1628)
- [ ] Test if time not change if you boot on recalbox or windows (#1791)
- [ ] Test the n64 kubii pad on the mupen64 standalone core (#1512)
- [ ] Test mupen64plus gliden64_20 on rpi3 (#1700)
- [ ] Test to change resolution when using mupen64plus (standalone)
- [ ] Test mupen64plus on GoA (standalone)
- [ ] Test mupen64plus + rice on RPI4 (standalone)
- [ ] Test PSP save path (in share/saves instead of share/bios)
- [ ] Test the game castlevania and other on the different boards (# 1756)
- [ ] Test mupen64plus glide64mk2 on rpi4, odroidxu4, x86, x86_64 (#1766)

## [7.2.2-Beta2]
- [ ] Test upgrade from stock 7.2.1 to 7.2.2 on XU4 (#1734)
- [ ] Test libretro-mupen64plus-nx works on x86 and x86_64 (#1407)
- [ ] Test there are no regressions for libretro-mupen64plus-nx on all platforms (#1407)
- [ ] Test RA for libretro swanstation (#1744)
- [ ] Test in libretro-mame push select with wiimote in lightgun games if the menu doesn't not appear (#1787)
- [ ] Test there are no regression for fbneo 1.0.0.02 (#1688)


## [7.2.2-Beta1]
- [ ] Check sound of boot video (#1596)
- [ ] Check sound is still working on PC
- [ ] Check OGST on xu4 works right after activating it in /boot/config.ini (#1678)
- [ ] Check PC x86_64 can boot from EFI (#1710)
- [ ] Check rtl8812au/rtl8821au wifi drivers (#1718)
- [ ] Check Cudy W650 works on rpi2, rpi3, odroixu4, x86 and x86_64
- [ ] Check HID bluetooth device maps to HCI (#1704)
- [ ] Test that update type has been removed from the menu in ES (#1673)
- [ ] Check overlay dc rpi3 (#1695)


## [7.2.1-Reloaded]
- [ ] Check that games are not hidden when upgrading to 7.2.1 from 7.2
- [ ] Run any game on PPSSPP rpi4 and check is vsync is on.
- [ ] Test odroidxu4 fan speeds are set on boot (#1662)
- [ ] Run games from the search window when its video snap is playing. Should not crash anymore.
- [ ] Switch one theme option, then cancel the ES reboot. Leave the menu. Enter the option menu again. Switch another option and let ES to reboot. Then check both options are OK
- [ ] Switch one theme option, let ES to reboot, then check if the option is ok
- [ ] Switch theme and let ES to reboot, then check the theme is displayed properly
- [ ] Hide a game with metadata. Relaunch ES or Reboot. Display hidden game and check if your last hidden game stall have its metadata
- [ ] Ensure updates check is ON in update menu. Run Gameclip screensaver immediately after ES is ready. Wait for 2 minutes max: Update popup should not appear until you exit the screensaver
- [ ] Test pixel perfect option: set/unset the option, quit then reenter the menu, it should be displayed properly
- [ ] Test reicast vanished from rpi4, x86, x86_64 and odroidxu4
- [ ] Test forgetting all BT devices, especially non-connected devices (switch off/standby)
- [ ] Test arcade view system all on this system check: mame, naomi, naomigd, atomiswave, model3, daphné, neogeo, fbneo
- [ ] Test on a game like "god of wars ghost of sparta" the intro video must be fluid
- [ ] Check if boot videos play full speed on rpi1, 2 and 3 (#1627)
- [ ] Check extensions of WII (*.rvz) & SCV (*.0) roms
- [ ] Check internal scraper default language
- [ ] Check ES won't freeze anymore when running a game while a vidoe is playing
- [ ] Check ES won't crash anymore when loading heavy themes
- [ ] Check ES display a popup when changing the HOSTNAME entry in network menu
- [ ] Check ES fetches an IPv4 when connecting WIFI (SSIF/Key or WPS)
- [ ] Check core libretro swanstation for psx (#1640) and check bios swanstation for psx
- [ ] Test ports CaveStory for x86 and x86_64 (#1665)
- [ ] Tester que les videos fonctionnent dans kodi sous rpi4
- [ ] Tester un dongle USB realtek sur l'odroidgo2 (module r8188eu)
- [ ] Check rtl88x2bu realtek wifi driver works again
- [ ] Check bluetooth pairing works as expected
- [ ] Test pad 8bitdo with lastest firmware (mode start+b) (#1624)
- [ ] Test volume is at max on boot for all outputs (#1634)
- [ ] Test gpsp (GBA) runs full speed on rpi0 (#1626)


## [7.2-Reloaded]
## Pending Features
- [ ] Bump Libretro-mame => test core with Romset 0.230
- [ ] Supermodel => Model3 emulator => test with romset 0.230 and test bypass configgen option add 'model3.configfile=dummy' in 'recalbox.conf' and check if '/recalbox/share/system/configs/model3/supermodel.ini' is not rewritten
- [ ] colecovision Netplay with FBNeo core
- [ ] Test & evaluate MelonDS on Pi4
- [ ] GameClip => Test gameclip screensaver with some available videos => Test all options
- [ ] GameClip => Test gameclip screensaver with no video (fresh install?) or by selecting only systems with no videos at all
- [ ] New slides during install => Fresh install on all patforms. All platforms must show the 8 slides, except on GPI which keeps the "creating share" video
- [ ] Test & evaluate libretro-mupen64plus-next on odroidxu4
- [ ] Add Super Cassette Vision core => Test this core and report bugs/improvements to Maaax
- [ ] Add libretro neocd core => test this core with cue/bin, cue/iso and chd
- [ ] Test Nintendo switch joycons and combined joycons
- [ ] DosBox Pure added => test this new core
- [ ] Pulseaudio - Check audio works everywhere and switch automatically when new output (jack, bluetooth) is connected
- [ ] Revalider tous les menus sur la liste système
- [ ] Revalider tous les menus sur les listes de jeux
- [ ] Test Craft (Minecraft clone) on ports (pc only)
- [ ] Odroid GO Super => check it works (uses same image as Go Advanced)
- [ ] Test and validate Paralleln64 on Pi3
- [ ] Test Watara Supervision emulator on all boards
- [ ] Test Elektronika Emulator emulator on all boards
- [ ] Test vibrations work with pcsx_rearmed (must be enabled if upgrading,
      check `pcsx_rearmed_vibration` in file system/configs/retrarch/core/retroarch-core-options.cfg)
- [ ] Test Videopack system
- [ ] Test Vic20 system
- [ ] Test all sound modes
- [ ] Test externalscreen.sh with a first screen that is 2K/4K but has not FullHD resolution

## Non Regression
- [ ] Vérifier que le settings "Show FPS" fonctionne correctement
- [ ] Vérifier que la nouvelle façon de gerer les O/C fonctionne correctement (verifier les vitesses réelles en SSH)
- [ ] Réécriture des menus => vérifier l'affichage, et le fonctionnement de chaque entrée dans les menus recalbox
- [ ] Amiberry Bump => Deeply retest amiberry: test all rom types, check default configuration, ...
- [ ] Fix glxinfo not found => mesa3d-demos : test on ssh command glxinfo
- [ ] Port Bump => xrick, tyrquake, reminiscense, prboom, mrboom, ecwolf, dinothawr, 2048 : test all cores
- [ ] New roms management => Test option in game menu to show/hide preinstalled games
- [ ] New roms management => Fresh install: Test all ports
- [ ] Added rpi-400 dts => check pi400 boots correctly
- [ ] mupen64plus bump => test standalone core deeply on all boards
- [ ] Enabled mupen64plus with rice for odroidgo2 => test it works thoroughly
- [ ] Check that mupen64plus standalone with gliden64 fully works on rpi2 and rpi3
- [ ] Bump Stella => Deeply retest Stella core
- [ ] BR2020.11 - Check hyperion still works (ian57 a le matos)
- [ ] BR2020.11 - Test libretro-fmsx
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
- [ ] BR2020.11 - Check reicast has no significant slowdown (compiled without lto)
- [ ] BR2020.11 - Test libretro-flycast
- [ ] Verify wm8960 audio hat works for all boards with kernel 5.10 (ian57 a le matos)
- [ ] BR2020.11 - Check splash video on rpi1, 2 and 3
- [ ] BR2020.11 - Check xu4 works perfectly (freeze, video problem, usb problem). Check `dmesg` for errors
- [ ] BR2020.11 - Check reicast-old has no significant slowdown on Odroid XU4(compiled without lto)
- [ ] BR2020.11 - Check that Odroid GO2 boots correctly
- [ ] BR2020.11 - Check that PC with intel chipsets works properly
- [ ] BR2020.11 - Check Xorg AMDGPU driver works properly (see with david)
- [ ] BR2020.11 - Check Xorg OpenChrome driver works properly (see with david)
- [ ] BR2020.11 - Check libretro-mame works properly (0.226)
- [ ] BR2020.11 - Check libretro-melonds works properly
- [ ] BR2020.11 - Test pcsx_rearmed on rpi1 works
- [ ] Test rtl8189fs, rtl8821au and rtl88x2bu wifi drivers work properly on rpi1, rpi2, odroidxu4, x86 and x86_64
- [ ] BR2020.11 - Check nvidia drivers version 390 works properly
- [ ] BR2020.11 - Check nvidia drivers version 440 works properly
- [ ] BR2020.11 - Check bluetooth pairing
- [ ] BR2020.11 - Check PC Legacy boot works (GRUB loading... boot loop)
- [ ] Bump Hatari => Retest all game format on Atari ST core
- [ ] RB should start successfully on any video output of the rpi4 (hdmi0 or hdmi1, switching needs reboot)
- [ ] Headphone output should be selectable in ES on rpi4
- [ ] Bump Beetle-Saturn: check launching virtual cop 1 and 2 (europe versions) + additional random saturn games to confirm bump and patch are globally ok
- [ ] Patch Retroarch: check 2 lightgun players games is possible now on x86_64 and still ok on board as Pi2/3/4 and XU4
- [ ] Patch Flycast: check that new crosshair for lightgun game is now not transparent/blend - best visibility in games as The Maze of kings
- [ ] Lightgun feature: check fun by your prefered lightgun games in priority ;-) check one game in 1 player and/or 2 players and by "lightgun" system  - check documentation also.
- [ ] Offline/online upgrade still works (check config.txt and recalbox-user-config.txt)
  - [ ] begin an upgrade download and remove ethernet cable while download => no upgrade + clean files
  - [ ] begin an upgrade download and poweroff during the download => no upgrade + clean files
  - [ ] put an image and a sha1sum file with wrong sha1 sum in /boot => no upgrade + clean files
  - [ ] put an image in share (offline upgrade) => no upgrade
  - [ ] put an image and a sha1sum file with wrong sha1 sum in share => no upgrade
  - [ ] put an image and a sha1sum file with good sha1 sum in share => UPGRADE
  - [ ] check /boot/recalbox-user-config.txt exists and /boot/config.txt includes it (rpi only)
  - [ ] add or change a parameter in /boot/recalbox-user-config.txt and check it is taken into account (rpi only)
- [ ] Pulseaudio - Check audio works in PPSSPP (#1511)
- [ ] Check if recalbox can connect on open network (passwordless SSID)
- [ ] Check 8Bitdo SN30 pro / SF30 pro mapping
- [ ] Check Palmos palmos52-en-t3.rom is correctly recognized
- [ ] Check that no `/var/db/dhcpcd/eth0.lease: No such file or directory` appears in /var/log
- [ ] Bump Libretro-uae: Check new CHD support on amigacd32 & amigacdtv
- [ ] Vérifier que le pi4 ne démarre pas en 4k sur un ecran 4k
- [ ] Check Odroid GO 2 image compatibility
  - [ ] Check Odroid GO 2 has no performance regression
  - [ ] Check Odroid GO 2 with uboot < 3/12/2020 still works after offline upgrade (boot.ini rewrite)
  - [ ] Check boot logo are displayed correctly
- [ ] Check Odroid GO 2 shaders now work
- [ ] Check RetroArch shortcuts all work on Odroid GO 2
- [ ] Check boot video is not played while slides are displayed after changing external share
- [ ] Vérifier la vitesse du core duckstation (PSX) sur Go2 et GoS
- [ ] Check libretro-paralleln64 works on Odroid GO 2 & GO Super
- [ ] Vérifier que les GO2/GO3 bootent quelque soit le firmware de la console
- [ ] Bump Fbneo => Test games namco for exemple the outfoxies (the different versions)
- [ ] Tester Paralleln64 on Pi4
- [ ] Check the order of the O / C options (Pi1,Pi2,PI3,Pi3B+,PI4)
- [ ] Update es_bios.xml (add bios Elektronika BK)
- [ ] vérifier que le son est correct sur rpi2 et rpi3
  - [ ] `pacmd list-modules |grep -A1 'name: <module-udev-detect>' |grep -q tsched=0 && echo "OK" ||echo "PAS OK"`
- [ ] vérifier que l'option "multichannel output" n'est plus visible sur les PI (0 à 4)
- [ ] vérifier que le son est bien routé vers headphone ou HDMI (avec ou sans distortion)
- [ ] Vérifier que la fonction d'oubli de manettes bluetooth soit opérationnelle
- [ ] Check fix bios md5 for DMG_BOOT.bin (nintendo gb) and CGB_BOOT.bin (nintendo gbc)
- [ ] Bump Np2kai => Test game for PC9801
- [ ] Vérifier que le screenshot fonctionne sur toutes les boards
- [ ] Check update screenshot mixv2recalbox of ports (2048, Dinothawr, Mrboom, Quake, Rick Dangerous)
- [ ] Bump Quasi88 => Test game for PC8800
- [ ] Tester le driver nVidia 460.67
- [ ] Tester PPSSPP (psp) sur la Go2/GoS (doit être plus rapide)
- [ ] Vérifier que le service bluetooth démarre et récup ses anciens settings (sur mise à jour uniquement)
  - [ ] Vérifier qu'un appairage fonctionne
  - [ ] Vérifier qu'un appareil précédement appairé (avant la mise à jour) est connecté
- [ ] Update lightgun.xml => Test Point Blank for fbneo (lightgun)
- [ ] Vérifier que le GPi émet du son (#1476)
- [ ] Vérifier que le ventilateur s'éteint à l'arrêt du nespi4case
- [ ] Vérifier que Killer instinct fonctionne à 60fps sur libretro-mame
- [ ] Vérifier que le gpicase dispose des profiles définis dans rpi-analog.conf
  - [ ] `test $(pacmd list-cards |grep -e '^\s*output:' |wc -l) -eq 2 && echo "ok" || echo "pas ok"`
  - [ ] `udevadm info -qall -p /sys/class/sound/card0 |grep -q PULSE_PROFILE_SET=rpi-analog.conf && echo "ok" || echo "pas ok"`
- [ ] Vérifier que l'odroidxu4 dispose des profiles définis dans odroidxu4-hdmi.conf
  - [ ] `pacmd list-cards |grep -e '^\s*output:' |grep -q 'Digital Stereo Output' && echo "ok" || echo "pas ok"`
  - [ ] `udevadm info -qall -p /sys/class/sound/card0 |grep -q PULSE_PROFILE_SET=odroidxu4-hdmi.conf && echo "ok" || echo "pas ok"`
- [ ] Check the path of the bios neogeocd in the bios checker (ES)
- [ ] Check x86 and x86_64 fresh install with nvidia GFX card works
- [ ] Check GPIcase config has been reconfigured
- [ ] Check bluetooth can pair device on first boot, again for b25
- [ ] Check sound output is HDMI on all rpi boards after a fresh-install
- [ ] Check OgS boot correctly after update from 7.1.1 OgS


## Technique
- [ ] Packages that require libgo2 compiles well
- [ ] /usr/lib/libgo2.so should provides go2_ symbols
- [ ] Validate libretro-mupen64plus-next compiles on all boards
- [ ] BR2020.11 - Test kodi-audiodecoder-timidity (how ?)
- [ ] BR2020.11 - Check that kernel patchs are applied on linux and (cutom) linux-headers
- [ ] BR2020.11 - Check that hardware works properly on x86 and x86_64 (no kernel and defconfig modified)
- [ ] Test S024kdetector still does its job
- [ ] Check if WPA-PSK-SHA256 key mgmt works (`wpa_cli status |grep -q key_mgmt=WPA2-PSK-SHA256 && echo "OK"`)
- [ ] Check that there are no regression with kernel 5.10 on rpi boards
- [ ] Tester que le GPi ou NesPi4 case s'installent correctement (via installation fraiche)


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
