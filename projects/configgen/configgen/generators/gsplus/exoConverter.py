import argparse
import os
import shutil
from typing import List

from configgen.emulatorlauncher import recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings


def Convert(inPath: str, outPath: str, folders: List[str]):
    for folder in folders:
        source: str = os.path.join(inPath, folder)
        # Config.txt available?
        configList: List[str] = [f for f in os.listdir(source) if os.path.isfile(os.path.join(source, f)) and os.path.splitext(f)[1] == ".txt"]
        if len(configList) > 0:
            index: int = 1
            print("[eXoConverter] Configured game: " + folder)
            for config in configList:
                print("[eXoConverter]   Configuration #{}".format(index))
                settings: keyValueSettings = keyValueSettings(os.path.join(source, config), True)
                settings.loadFile()
                if settings.hasOption("s5d1"):
                    # Adjust rom files
                    for romkey in ("s5d1", "s5d2", "s6d1", "s6d2", "s7d1", "s7d2", "s7d3", "s7d4", "s7d5", "s7d6", "s7d7", "s7d8", "s7d9"):
                        romfile = settings.getString(romkey, "")
                        if romfile != "":
                            if romfile[0] != '#':
                                if not os.path.exists(os.path.join(source, romfile)): romfile = romfile[0:1].upper() + romfile[1:]
                                name, ext = os.path.splitext(romfile)
                                newName: str = '.' + folder + " (" + name + ")" + ext
                                settings.setString(romkey, newName)
                                try:
                                    shutil.copyfile(os.path.join(source, romfile), os.path.join(outPath, newName))
                                    print("[eXoConverter]     Copied : {}".format('.' + romfile))
                                except:
                                    print("[eXoConverter]     ERROR, file not found! {}".format(newName))
                    # Adjust bios
                    bios = settings.getString("g_cfg_rom_path", "")
                    if bios != "":
                        if   "ROM1" in bios: settings.setString("g_cfg_rom_path", "%1")
                        elif "ROM3" in bios: settings.setString("g_cfg_rom_path", "%3")
                        else: print("[eXoConverter]     Unknown bios! {}".format(bios))
                    # Save settings
                    destinationConfig = folder
                    if len(configList) > 1: destinationConfig += " (" + os.path.splitext(config)[0] + ")"
                    settings.changeSettingsFile(os.path.join(outPath, destinationConfig + ".gsplus"))
                    settings.saveFile()
                else:
                    print("[eXoConverter]     Not a valid config file : {}".format(config))
                index += 1
        else:
            print("[eXoConverter] Unconfigured game: " + folder)
            # Just copy known files
            files: List[str] = [f for f in os.listdir(source) if os.path.isfile(os.path.join(source, f))]
            for file in files:
                name, ext = os.path.splitext(file)
                if ext in (".po", ".2mg", ".hdv"):
                    shutil.copyfile(os.path.join(source, file), os.path.join(outPath, folder + " (" + name + ')' + ext))
                    print("[eXoConverter]   Copied file: " + file)
                else:
                    print("[eXoConverter]   Ignored file: " + file)
            pass


def GetGamesFolders(folderpath: str):
    folders = [f for f in os.listdir(folderpath) if os.path.isdir(os.path.join(folderpath, f))]
    return folders

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='arcade database script')
    parser.add_argument("-i", help="Source game path", type=str, required=True)
    parser.add_argument("-o", help="Destination game path", type=str, required=True)
    arguments = parser.parse_args()

    gameFolders = GetGamesFolders(arguments.i)
    print("[eXoConverter] {} Geme folder found".format(len(gameFolders)))
    Convert(arguments.i, arguments.o, gameFolders)
    pass