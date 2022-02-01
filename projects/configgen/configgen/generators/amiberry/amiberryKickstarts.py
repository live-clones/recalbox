#!/usr/bin/env python
import configgen.recalboxFiles as recalboxFiles
from configgen.generators.amiberry.amiberryRomType import RomType
from configgen.generators.amiberry.amiberrySubSystems import SubSystems


class KickstartManager:

    BIOS_LIST = \
    {
        # Kickstart v1.2 rev 33.180 (1986)(Commodore)(A500-A1000-A2000).rom
        "kick33180.A500":  (256, "KS ROM v1.2 (A500,A1000,A2000) rev 33.180 (256k) [315093-01]"),
        # Kickstart v1.3 rev 34.5 (1987)(Commodore)(A500-A1000-A2000-CDTV).rom
        "kick34005.A500":  (256, "KS ROM v1.3 (A500,A1000,A2000) rev 34.5 (256k) [315093-02]"),
        # Kickstart v2.04 rev 37.175 (1991)(Commodore)(A500+).rom
        "kick37175.A500":  (256, "KS ROM v2.04 (A500+) rev 37.175 (512k) [390979-01]"),
        # Kickstart v3.0 rev 39.106 (1992)(Commodore)(A1200)[!].rom
        "kick39106.A1200": (256, "KS ROM v3.0 (A1200) rev 39.106 (512k) [391523-01/391524-01]"),
        # Kickstart v3.1 rev 40.63 (1993)(Commodore)(A500-A600-A2000)[!].rom
        "kick40063.A600":  (256, "KS ROM v3.1 (A600) rev 40.63 (512k)"),
        # Kickstart v3.1 rev 40.68 (1993)(Commodore)(A1200).rom
        "kick40068.A1200": (256, "KS ROM v3.1 (A1200) rev 40.68 (512k) [391773-01/391774-01]"),
        # Kickstart v3.1 rev 40.68 (1993)(Commodore)(A4000).rom
        "kick40068.A4000": (256, "KS ROM v3.1 (A4000) rev 40.68 (512k)"),

        # Kickstart v1.3 rev 34.5 (1987)(Commodore)(A500-A2000-CDTV)[!].rom
        "kick34005.CDTV": (256, "CDTV Extended ROM rev 34.5 (256k)"),

        # Kickstart v3.1 rev 40.60 (1993)(Commodore)(CD32).rom
        "kick40060.CD32":  (512, "CD32 KS ROM v3.1 rev 40.60 (512k)"),
        # CD32 Extended-ROM rev 40.60 (1993)(Commodore)(CD32).rom 87746be2
        "kick40060.CD32.ext": (1024, "CD32 extended ROM rev 40.60 (512k)"),

        # AROS Kickstart 2019
        "kick02019.AROS": (256, "KS AROS 2019 Part#1 (512k)"),
        # AROS Extended Kickstart 2019
        "kick02019.AROS.ext": (256, "KS AROS 2019 Part#2 (512k)"),
    }

    BIOS_PER_SYSTEM = \
    {
        SubSystems.A600:
        [
            (   # Use the highest kickstart with advanced hardware
                [RomType.WHDL, RomType.HDF, RomType.PACKAGE, RomType.CDROM, RomType.HDDFS],
                ["kick37175.A500", "kick34005.A500", "kick33180.A500"]
            ),
            (   # Use kickstarts 1.3/1.2 to maximize compatibility when using disks
                [RomType.DISK],
                ["kick34005.A500", "kick33180.A500"]
            ),
        ],
        SubSystems.A1200:
        [
            (   # Use kickstart 3.1 first, then 3.0. Fallback on A4000 kickstart
                [RomType.WHDL, RomType.HDF, RomType.PACKAGE, RomType.CDROM, RomType.HDDFS, RomType.DISK],
                ["kick40068.A1200", "kick39106.A1200", "kick40068.A4000"]
            ),
        ],
        SubSystems.CD32:
        [
            (   # No choice for the CD32
                [RomType.CDROM],
                ["kick40060.CD32|kick40060.CD32.ext"]
            ),
        ],
        SubSystems.CDTV:
        [
            (   # No choice for the CD32
                [RomType.CDROM],
                ["kick34005.A500|kick34005.CDTV"]
            ),
        ],
    }

    def __init__(self):
        self.rom: str = ""
        self.ext: str = ""

    @staticmethod
    def GenerateWHDSymLinks(destinationfolder: str):
        for bios in KickstartManager.BIOS_LIST:
            import os
            biosPath = os.path.join(recalboxFiles.BIOS, bios)
            biosLink = os.path.join(destinationfolder, bios)
            if not os.path.exists(biosLink) and os.path.exists(biosPath):
                os.symlink(biosPath, biosLink)

    def GetKickstartsFor(self, subsystem: SubSystems, romtype: RomType):
        # Select best available kickstart
        biosFile = None
        extFile = None
        needExt = False
        tuppleArray = KickstartManager.BIOS_PER_SYSTEM[subsystem]
        import os
        for tupple in tuppleArray:
            romTypes, biosList = tupple
            if romtype in romTypes:
                for biosCouple in biosList:
                    bios = biosCouple
                    ext = None
                    if '|' in biosCouple:
                        bios, ext = biosCouple.split('|', 2)
                        needExt = ext is not None
                    biosPath = os.path.join(recalboxFiles.BIOS, bios)
                    if os.path.exists(biosPath):
                        biosFile = biosPath
                    if needExt:
                        extPath = os.path.join(recalboxFiles.BIOS, ext)
                        if os.path.exists(extPath):
                            extFile = extPath
                    if biosFile is not None:
                        break

        # Fallbacks
        if subsystem in SubSystems.COMPUTERS:
            # Fallback to AROS 2019?
            if biosFile is None:
                arosBios = os.path.join(recalboxFiles.BIOS, "kick02019.AROS")
                arosExt = os.path.join(recalboxFiles.BIOS, "kick02019.AROS.ext")
                if os.path.exists(arosBios) and os.path.exists(arosExt):
                    biosFile = arosBios
                    extFile = arosExt

            # Fallback to internal Aros bios
            if biosFile is None:
                self.rom = ":AROS"

        # Last check
        if biosFile is None:
            raise Exception("No BIOS found")
        # If an extened bios is required, it's not worth going further w/o it
        if extFile is None and needExt:
            raise Exception("Extended BIOS not found")

        self.rom = biosFile if biosFile is not None else ""
        self.ext = extFile if extFile is not None else ""

    @property
    def GetBIOS(self) -> str: return self.rom

    @property
    def NeedExtendedBIOS(self) -> bool: return len(self.ext) != 0

    @property
    def GetExtendedBIOS(self) -> str: return self.ext