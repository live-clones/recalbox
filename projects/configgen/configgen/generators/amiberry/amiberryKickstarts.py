#!/usr/bin/env python
import os
import recalboxFiles
from generators.amiberry.amiberryRomType import RomType
from generators.amiberry.amiberrySubSystems import SubSystems


class KickstartManager:

    def __init__(self):
        self.rom = None
        self.ext = None

    def GetKickstartsFor(self, subsystem, romtype):
        # Select best kickstart
        if subsystem == SubSystems.A600:
            if romtype == RomType.WHDL:
                self.rom = recalboxFiles.BIOS + "/kick20.rom"
            else:
                self.rom = recalboxFiles.BIOS + "/kick13.rom"
        elif subsystem == SubSystems.A1200:
            self.rom = recalboxFiles.BIOS + "/kick31.rom"
        elif subsystem == SubSystems.CD32:
            self.rom = recalboxFiles.BIOS + "/cd32.rom"
            self.ext = recalboxFiles.BIOS + "/cd32ext.rom"
        elif subsystem == SubSystems.CDTV:
            self.rom = recalboxFiles.BIOS + "/kick13.rom"
            self.ext = recalboxFiles.BIOS + "/cdtvext.rom"

        # Check kickstart availability
        if not os.path.exists(self.rom):
            self.rom = ":AROS"

        # If an extened bios is required, it's not worth going further w/o it
        if self.ext is not None:
            if not os.path.exists(self.ext):
                raise Exception("Extended BIOS not found")

    def GetBIOS(self):
        return self.rom

    def NeedExtendedBIOS(self):
        return self.ext is not None

    def GetExtendedBIOS(self):
        return self.ext
