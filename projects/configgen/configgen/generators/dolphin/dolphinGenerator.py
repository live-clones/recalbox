#!/usr/bin/env python
import Command
import recalboxFiles
import os.path
from generators.Generator import Generator
import dolphinControllers
from settings.iniSettings import IniSettings
from settings.keyValueSettings import keyValueSettings

class DolphinGenerator(Generator):

    # Game Ratio
    GAME_RATIO = \
    {
       "auto": 0,
       "16/9": 1, 
       "4/3": 2,
       "squarepixel": 3,
    }

    # Gamecube languages
    GAMECUBE_LANGUAGES = \
    {
        "en": 0,
        "de": 1,
        "fr": 2,
        "es": 3,
        "it": 4,
        "du": 5,
    }

    # Wii languages
    WII_LANGUAGES = \
    {
        "jp": 0,
        "en": 1,
        "de": 2,
        "fr": 3,
        "es": 4,
        "it": 5,
        "du": 6,
        "zh": 7, # traditional chinese 8, ignored
        "kr": 9,
    }

    SECTION_GENERAL   = "General"
    SECTION_INTERFACE = "Interface"
    SECTION_CORE      = "Core"
    SECTION_ANALYTICS = "Analytics"
    SECTION_BTPASSTHROUGH = "BluetoothPassthrough"
    SECTION_SETTINGS  = "Settings"
    SECTION_HARDWARE = "Hardware"
    SECTION_NETPLAY = "NetPlay"

    # Get keyboard layout
    @staticmethod
    def GetLanguage():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        # Try to obtain from keyboard layout, then from system language, then fallback to us
        kl = conf.getOption("system.kblayout", conf.getOption("system.language", "en")[0:2]).lower()
        return kl

    # Get selected ratio
    @staticmethod
    def GetRatio():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        hasRatio = conf.getOption("gamecube.ratio", "$")
        return hasRatio

    # Get netplay username
    @staticmethod
    def GetNetplay():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        speudo = conf.getOption("global.netplay.nickname", "$")
        return speudo

    def mainConfiguration(self):
        # Get Languages
        language = self.GetLanguage()
        gamecubeLanguage = self.GAMECUBE_LANGUAGES[language] if language in self.GAMECUBE_LANGUAGES else 0
        wiiLanguage = self.WII_LANGUAGES[language] if language in self.WII_LANGUAGES else 0
        # Get Netplay UserName
        nickname = self.GetNetplay()
        
        # Load Configuration
        dolphinSettings = IniSettings(recalboxFiles.dolphinIni, True)
        dolphinSettings.loadFile(True)

        # Interface
        dolphinSettings.setOption(self.SECTION_INTERFACE, "Language", wiiLanguage)
        dolphinSettings.setOption(self.SECTION_INTERFACE, "LanguageCode", language)
        dolphinSettings.setOption(self.SECTION_INTERFACE, "AutoHideCursor", "True")
        dolphinSettings.setOption(self.SECTION_INTERFACE, "ConfirmStop", "False")
        # Core
        dolphinSettings.setOption(self.SECTION_CORE, "SelectedLanguage", gamecubeLanguage)
        dolphinSettings.setOption(self.SECTION_CORE, "WiimoteContinuousScanning", "True")
        dolphinSettings.setOption(self.SECTION_CORE, "WiiKeyboard", "False")
        # General
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPaths", "2")
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPath0", "/recalbox/share/roms/gamecube")
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPath1", "/recalbox/share/roms/wii")
        # Analytics
        dolphinSettings.setOption(self.SECTION_ANALYTICS, "Enabled", "True")
        dolphinSettings.setOption(self.SECTION_ANALYTICS, "PermissionAsked", "True")
        # BluetoothPasstrough
        dolphinSettings.setOption(self.SECTION_BTPASSTHROUGH, "Enable", "False")
        # Netplay
        dolphinSettings.setOption(self.SECTION_NETPLAY, "Nickname", nickname)
        dolphinSettings.setOption(self.SECTION_NETPLAY,  "UseUPNP", "True")

        # Save configuration
        dolphinSettings.saveFile()

    def gfxConfiguration(self, system):
        # Get Ratio
        ratio = self.GetRatio()
        gameRatio = self.GAME_RATIO[ratio] if ratio in self.GAME_RATIO else 0
        
        # Load Configuration
        gfxSettings = IniSettings(recalboxFiles.dolphinGFX, True)
        gfxSettings.loadFile(True)
 
        # FPS
        gfxSettings.setOption(self.SECTION_SETTINGS, "ShowFPS", "True" if system.config['showFPS'] == 'true' else "False")
        # Vsync
        gfxSettings.setOption(self.SECTION_HARDWARE, "VSync", "True")
        # Ratio
        gfxSettings.setOption(self.SECTION_SETTINGS, "AspectRatio", gameRatio)

        # Save configuration
        gfxSettings.saveFile()

    def generate(self, system, rom, playersControllers, demo, recalboxSettings):
        if not system.config['configfile']:
            # Controllers
            dolphinControllers.generateControllerConfig(system, playersControllers)

            self.mainConfiguration()
            self.gfxConfiguration(system)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-platform", "xcb", "-e", rom]
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])
        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.CONF, "XDG_DATA_HOME":recalboxFiles.SAVES})
