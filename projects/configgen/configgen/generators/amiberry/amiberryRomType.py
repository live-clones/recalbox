#!/usr/bin/env python
import fnmatch
import glob
import os
import subprocess


class RomType:

    UNKNOWN = 0
    DISK = 1
    WHDL = 2
    CDROM = 3
    HDD = 4
    PACKAGE = 5

    VALID_EXTENSIONS = [".iso", ".bin", ".cue", ".ccd", ".nrg", ".mds", ".chd", ".adf", ".ipf", ".lha", ".lzh", ".lzx", ".zip", ".7z", ".rp9"]
    VALID_CONFIGURATIONS = [".uae"]

    def __init__(self):
        raise Exception("Use static methods!")

    @staticmethod
    def SeekForExtension(path, pattern):
        for _, _, filenames in os.walk(path):
            for _ in fnmatch.filter(filenames, pattern):
                return True
        return False

    @staticmethod
    def Identify(rom):
        romGame, romExt = os.path.splitext(rom)

        romType = RomType.UNKNOWN
        romHasUAE = False
        romExt = romExt.lower()

        # If the file is a config file, then search for the game file
        if romExt in RomType.VALID_CONFIGURATIONS:
            if os.path.isdir(romGame):
                romExt = ""
                rom = romGame
            elif os.path.isdir(romGame + ".hd"):
                romExt = ".hd"
                rom = romGame + ".hd"
            else:
                for f in glob.glob(romGame + ".*"):
                    romGame, romExt = os.path.splitext(f)
                    if romExt in RomType.VALID_EXTENSIONS:
                        rom = romGame + romExt
                        break
            if romExt == ".uae":
                romHasUAE = True

        # Identify "rom" file/folder
        # Amiberry 2.24 supports:
        # - native WHDL (folder, 7z, zip, lha).
        # - Disk images ADF or IPF using libcapsimage library (file, 7z, zip)
        # - HDD folder (uncompressed)
        # - CD images cue/mds/ccd/iso/bin/img/chd (file, 7z, zip)
        # - Package files rp9 (file)

        if not os.path.isdir(rom):
            # File
            if romExt == ".ipf" or romExt == ".adf":
                romType = RomType.DISK
            elif romExt == ".lha" or romExt == ".lzh" or romExt == ".lzx":
                romType = RomType.WHDL
            elif romExt in [".iso", ".bin", ".cue", ".ccd", ".nrg", ".mds", ".chd"]:
                romType = RomType.CDROM
            # Zip
            elif romExt == ".zip":
                fileList = subprocess.check_output(['unzip', '-l', rom]).lower()
                if fileList.find(".slave") >= 0:
                    romType = RomType.WHDL
                elif fileList.find(".ipf") >= 0 or fileList.find(".adf") >= 0:
                    romType = RomType.DISK
                elif fileList.find(".iso") >= 0 or fileList.find(".bin") >= 0 or \
                        fileList.find(".cue") >= 0 or fileList.find(".ccd") >= 0 or \
                        fileList.find(".nrg") >= 0 or fileList.find(".mds") >= 0 or \
                        fileList.find(".chd") >= 0:
                    romType = RomType.CDROM
            # 7z
            elif romExt == ".7z":
                fileList = subprocess.check_output(['7zr', 'l', rom]).lower()
                if fileList.find(".slave") >= 0:
                    romType = RomType.WHDL
                elif fileList.find(".ipf") >= 0 or fileList.find(".adf") >= 0:
                    romType = RomType.DISK
                elif fileList.find(".iso") >= 0 or fileList.find(".bin") >= 0 or \
                        fileList.find(".cue") >= 0 or fileList.find(".ccd") >= 0 or \
                        fileList.find(".nrg") >= 0 or fileList.find(".mds") >= 0 or \
                        fileList.find(".chd") >= 0:
                    romType = RomType.CDROM
            elif romExt == ".rp9":
                romType = RomType.PACKAGE
        else:
            # folder
            _, folderExt = os.path.splitext(rom)
            if folderExt == ".hd":
                romType = RomType.HDD
            elif RomType.SeekForExtension(rom, "*.Slave"):
                romType = RomType.WHDL
            else:
                romType = RomType.HDD

        # Check configuration file if required
        if not romHasUAE:
            if os.path.exists(romGame + ".uae"):
                romHasUAE = True

        return rom, romType, romHasUAE


