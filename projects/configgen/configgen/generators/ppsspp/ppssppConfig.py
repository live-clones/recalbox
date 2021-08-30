#!/usr/bin/env python
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator

def writePPSSPPConfig(system: Emulator):
    from configgen.settings.iniSettings import IniSettings
    settings = IniSettings(recalboxFiles.ppssppConfig, True)
    settings.loadFile(True)
    # Display FPS
    settings.setInt("CPU", 'ShowFPSCounter', 3 if  system.ShowFPS else 0) # 1 for Speed%, 2 for FPS, 3 for both
    settings.saveFile()
