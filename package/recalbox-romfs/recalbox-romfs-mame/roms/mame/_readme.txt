## RECALBOX - SYSTEM MAME ##

Put your mame roms in this directory.

Rom files must have a ".zip" extension.

Recalbox is using libretro mame2003 as default core. So, compatible roms must come from a 0.78 mame romset

Are also included in Recalbox:
- libretro core mame2000: 0.37b5 mame romset
- libretro core mame2010 (not available on rpi0/1): 0.139 mame romset
- libretro core mame2015 0.160 mame romset
- librerto core mame 0.219 mame romset
- standalone emulator advancemame: 0.106 mame romset

So, if you want to use a core different of the default one, you must edit your recalbox.conf file following instructions of this page:
https://github.com/recalbox/recalbox-os/wiki/recalbox.conf-%28EN%29

You can use clrmamepro available at http://mamedev.emulab.it/clrmamepro/ and use the .dat or .xml file in bios/mame* directory to check your roms.

Special files for mame2003 core :
- Add your samples files in /recalbox/share/bios/mame2003/samples/
- Update the hiscore.dat file in /recalbox/share/bios/mame2003/ if you want latest highscores (http://highscore.mameworld.info/download.htm)
- Download history.dat in /recalbox/share/bios/mame2003/ to enable ingame history menu (http://www.arcade-history.com/index.php?page=download)