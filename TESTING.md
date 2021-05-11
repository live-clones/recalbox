# Testing

This file is in french as all our beta testers are french.


A chaque modification du système, les développeurs ajoutent une ou plusieurs lignes dans ce fichier.

Les beta testeurs cochent les cases lors des sessions de tests.

Toutes les cases doivent être cochées avant la release stable.

Pour les pending features, elles ne seront activées que si les beta testeurs les valident selon les critères de qualité de Recalbox.

## [NEXT]
- [ ] Check sound of boot video (#1596)
- [ ] Check sound is still working on PC
- [ ] Check OGST on xu4 works right after activating it in /boot/config.ini (#1678)
- [ ] Check PC x86_64 can boot from EFI (#1710)

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
