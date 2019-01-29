#!/usr/bin/env python
import os

import recalboxFiles
from settings.keyValueSettings import keyValueSettings


class AmiberryWHDConfig:

    def __init__(self, config):
        self.sourceConfig = config

    def generateWHDConfiguration(self, rom):
        gameName, _ = os.path.splitext(os.path.basename(rom))
        cleanGameName = gameName.lower()
        for c in cleanGameName:
            if not c.isalpha() and not c.isnumeric():
                cleanGameName = cleanGameName.replace(str(c), " ")
        whdlConfig = keyValueSettings(os.path.join(recalboxFiles.amiberryMountPoint, "whdboot/game-data", gameName + ".whd"))

        # Controls
        if self.sourceConfig.getOption("joyport0", "") == "mouse":
            whdlConfig.setOption("PRIMARY_CONTROL", "MOUSE")
        else:
            whdlConfig.setOption("PRIMARY_CONTROL", "JOYSTICK")
        if self.sourceConfig.getOption("joyport0", "") == "mouse":
            whdlConfig.setOption("PORT0", "MOUSE")
        elif self.sourceConfig.getOption("joyport0_mode", "") == "cd32joy" or "cd32" in rom.lower():
            whdlConfig.setOption("PORT0", "CD32")
        else:
            whdlConfig.setOption("PORT0", "JOY")
        if self.sourceConfig.hasOption("joyport1"):
            if self.sourceConfig.getOption("joyport1", "") == "mouse":
                whdlConfig.setOption("PORT1", "MOUSE")
            elif self.sourceConfig.getOption("joyport1_mode", "") == "cd32joy" or "cd32" in rom.lower():
                whdlConfig.setOption("PORT1", "CD32")
            else:
                whdlConfig.setOption("PORT1", "JOY")

        # CPU
        cpu = self.sourceConfig.getOption("cpu_model", "68000").upper();
        whdlConfig.setOption("CPU", cpu)
        whdlConfig.setOption("CPU_COMPATIBLE", self.sourceConfig.getOption("cpu_compatible", "true").upper())
        whdlConfig.setOption("JIT", self.sourceConfig.getOption("compfpu", "false").upper())
        if self.sourceConfig.getOption("cpu_speed", "real") == "max":
            whdlConfig.setOption("CLOCK", "MAX")
        elif cpu == "68000":
            whdlConfig.setOption("CLOCK", "7")
        elif cpu == "68020":
            whdlConfig.setOption("CLOCK", "14")
        elif cpu in ["68030", "68040"]:
            whdlConfig.setOption("CLOCK", "28")

        # Chipset
        whdlConfig.setOption("BLITTER", "NORMAL" if self.sourceConfig.getOption("waiting_blits", "automatic").upper() == "automatic" else "WAIT")
        whdlConfig.setOption("CHIPSET", self.sourceConfig.getOption("chipset", "ocs").upper())
        if " aga " in cleanGameName or cleanGameName.endswith(" aga"):
            whdlConfig.setOption("CHIPSET", "AGA")
        whdlConfig.setOption("FAST_COPPER", self.sourceConfig.getOption("fast_copper", "false").upper())

        # Graphics
        whdlConfig.setOption("NTSC", self.sourceConfig.getOption("NTSC", "false").upper())
        whdlConfig.setOption("LINE_DOUBLING", "TRUE" if self.sourceConfig.getOption("gfx_linemode", "single") ==  "double" else "FALSE")
        whdlConfig.setOption("SPRITES", { "none": "NONE", "playfields": "PLAYFIELD", "sprites": "SPRITES", "full": "FULL" }[self.sourceConfig.getOption("collision_level", "").upper()])
        whdlConfig.setOption("FAST_RAM", self.sourceConfig.getOption("fastmem_size", "0").upper())
        whdlConfig.setOption("Z3_RAM", self.sourceConfig.getOption("z3mem_size", "0").upper())

        # Save at last!
        whdlConfig.saveFile();
