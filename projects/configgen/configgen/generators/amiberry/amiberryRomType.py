#!/usr/bin/env python
from enum import IntEnum


class RomType(IntEnum):

    UNKNOWN = 0  # Unknown
    DISK = 1     # Disk ADF or IPF, file zipped or 7zipped (ADF only)
    WHDL = 2     # WHDL Folder, zipped-folder, lha'ed-folder
    CDROM = 3    # Multiple CD formats
    HDDFS = 4    # HDD FileSystem
    HDF = 5      # Harddisk file
    PACKAGE = 6  # RP9 all-in-one package

    @staticmethod
    def SeekForExtension(path: str, pattern: str) -> bool:
        import fnmatch
        import os
        for _, _, filenames in os.walk(path):
            for _ in fnmatch.filter(filenames, pattern):
                return True
        return False

    @staticmethod
    def Identify(rom: str) -> (str, int, bool):
        import os
        romGame, romExt = os.path.splitext(rom)

        romType = RomType.UNKNOWN
        romHasUAE = False
        romExt = romExt.lower()

        # If the file is a config file, then search for the game file
        VALID_CONFIGURATIONS = (".uae",)
        if romExt in VALID_CONFIGURATIONS:
            if os.path.isdir(romGame):
                romExt = ""
                rom = romGame
            elif os.path.isdir(romGame + ".hd"):
                romExt = ".hd"
                rom = romGame + ".hd"
            else:
                VALID_EXTENSIONS = (".iso", ".bin", ".cue", ".ccd", ".nrg", ".mds", ".chd", ".adf", ".ipf", ".lha", ".lzh", ".lzx", ".zip", ".7z", ".rp9", ".hdf")
                for ext in VALID_EXTENSIONS:
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
            # Zip & 7z
            elif romExt in (".zip", ".7z"):
                import subprocess
                fileList = subprocess.check_output(['unzip', '-l', rom] if romExt == ".zip" else ['7zr', 'l', rom], encoding="utf-8").lower()
                if ".slave" in fileList:
                    romType = RomType.WHDL
                elif ".ipf" in fileList or ".adf" in fileList:
                    romType = RomType.DISK
                elif ".iso" in fileList or ".bin" in fileList or ".cue" in fileList or ".ccd" in fileList or \
                     ".nrg" in fileList or ".mds" in fileList or ".chd" in fileList:
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
