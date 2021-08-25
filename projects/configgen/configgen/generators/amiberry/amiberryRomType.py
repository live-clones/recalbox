#!/usr/bin/env python
import fnmatch
import os
import subprocess


class RomType:

    UNKNOWN = 0  # Unknown
    DISK = 1     # Disk ADF or IPF, file zipped or 7zipped (ADF only)
    WHDL = 2     # WHDL Folder, zipped-folder, lha'ed-folder
    CDROM = 3    # Multiple CD formats
    HDDFS = 4    # HDD FileSystem
    HDF = 5      # Harddisk file
    PACKAGE = 6  # RP9 all-in-one package

    VALID_EXTENSIONS = ( ".iso", ".bin", ".cue", ".ccd", ".nrg", ".mds", ".chd", ".adf", ".ipf", ".lha", ".lzh", ".lzx", ".zip", ".7z", ".rp9", ".hdf" )
    VALID_CONFIGURATIONS = ( ".uae", )

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
                for ext in RomType.VALID_EXTENSIONS:
                    if os.path.exists(romGame + ext):
                        romExt = ext
                        rom = romGame + ext
                        break
            if romExt == ".uae":
                romHasUAE = True

        # Identify "rom" file/folder
        # Amiberry 2.24 supports:
        # - native WHDL (folder, 7z, zip, lha).
        # - Disk images ADF or IPF using libcapsimage library (file, 7z, zip)
        # - HDDFS folder (uncompressed)
        # - HD Files
        # - CD images cue/mds/ccd/iso/bin/img/chd (file, 7z, zip)
        # - Package files rp9 (file)

        if not os.path.isdir(rom):
            # File
            if romExt == ".ipf" or romExt == ".adf":
                romType = RomType.DISK
            elif romExt == ".lha" or romExt == ".lzh" or romExt == ".lzx":
                romType = RomType.WHDL
            elif romExt in (".iso", ".bin", ".cue", ".ccd", ".nrg", ".mds", ".chd"):
                romType = RomType.CDROM
            elif romExt == ".hdf":
                romType = RomType.HDF
            # Zip
            elif romExt == ".zip":
                fileList = subprocess.check_output(['unzip', '-l', rom], encoding="utf-8").lower()
                if fileList.find(".slave") >= 0:
                    romType = RomType.WHDL
                elif fileList.find(".ipf") >= 0 or fileList.find(".adf") >= 0:
                    romType = RomType.DISK
                elif fileList.find(".iso") >= 0 or fileList.find(".bin") >= 0 or \
                        fileList.find(".cue") >= 0 or fileList.find(".ccd") >= 0 or \
                        fileList.find(".nrg") >= 0 or fileList.find(".mds") >= 0 or \
                        fileList.find(".chd") >= 0:
                    romType = RomType.CDROM
                elif fileList.find(".hdf") >= 0:
                    romType = RomType.HDF
            # 7z
            elif romExt == ".7z":
                fileList = subprocess.check_output(['7zr', 'l', rom], encoding="utf-8").lower()
                if fileList.find(".slave") >= 0:
                    romType = RomType.WHDL
                elif fileList.find(".ipf") >= 0 or fileList.find(".adf") >= 0:
                    romType = RomType.DISK
                elif fileList.find(".iso") >= 0 or fileList.find(".bin") >= 0 or \
                        fileList.find(".cue") >= 0 or fileList.find(".ccd") >= 0 or \
                        fileList.find(".nrg") >= 0 or fileList.find(".mds") >= 0 or \
                        fileList.find(".chd") >= 0:
                    romType = RomType.CDROM
                elif fileList.find(".hdf") >= 0:
                    romType = RomType.HDF
            elif romExt == ".rp9":
                romType = RomType.PACKAGE
        else:
            # folder
            _, folderExt = os.path.splitext(rom)
            if folderExt == ".hd":
                romType = RomType.HDDFS
            elif RomType.SeekForExtension(rom, "*.Slave"):
                romType = RomType.WHDL
            else:
                romType = RomType.HDDFS

        # Check configuration file if required
        if not romHasUAE:
            if os.path.exists(romGame + ".uae"):
                romHasUAE = True

        return rom, romType, romHasUAE
