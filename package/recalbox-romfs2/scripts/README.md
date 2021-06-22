# ROMFS v2

Requirements: python 3.8+

## Create new system

Use this feature to create a new system in the romfs.

Usage: `python3 romfs.py -systems <systems root folder> -create <new system short name>`

Example:
- `python3 romfs.py -systems ../systems -create snes`
- `python3 ~/recalbox/packages/recalbox-romfs2/scripts/romfs.py -systems ~/recalbox/packages/recalbox-romfs2/systems -create snes`

This will create a new folder in `recalbox-romfs2/systems` with a `roms` subfolder and a `system.ini` file.
The `roms` subfolder contains readme files and optional pre-installed games.
The `system.ini` is the system descriptor: edit this file accordingly.

When pre-installed games are copied to the target, contents of `systems/<system>/roms` are copied into `/recalbox/share_init/roms/<system>` 

## Generate system descriptor

Generate the final system descriptor `systemlist.xml` into the target `/recalbox/share_init/system/.emulationstation/` from all `system.ini`

Usage: `python3 romfs.py -systems <systems root folder> -buildsystems <target file path>`

Example:
- `python3 romfs.py -systems ../systems -buildsystems ../output.xml`
- `python3 ~/recalbox/packages/recalbox-romfs2/scripts/romfs.py -systems ~/recalbox/packages/recalbox-romfs2/systems -buildsystems ~/recalbox/output/target/recalbox/share_init/system/.emulationstation/systemlist.xml`

## Copy to target

Populate the final `share_init/roms` folder with all pre-installed games and all readme files.

Usage: `python3 romfs.py -systems <systems root folder> -installroms <target roms path>`

Example:
- `python3 romfs.py -systems ../systems -installroms ~/recalbox/output/target/recalbox/share_init/roms`
- `python3 ~/recalbox/packages/recalbox-romfs2/scripts/romfs.py -systems ~/recalbox/packages/recalbox-romfs2/systems -installroms ~/recalbox/output/target/recalbox/share_init/roms`

| Tables   |      Are      |  Cool |
|----------|:-------------:|------:|
| col 1 is |  left-aligned | $1600🖱️ ⌨️ 🎮 🕹️ 🔫|
| col 2 is |    centered   |   $12 |
| col 3 is | right-aligned |    $1 |