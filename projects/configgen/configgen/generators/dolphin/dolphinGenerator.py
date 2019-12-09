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

    # Neplay servers
    NETPLAY_SERVERS = \
    {
        # Chine
        "CN": "CH",
        "TW": "CH",
        # Est Asia
        "KR": "EA",
        "RU": "EA",
        "JP": "EA",
        # Europe
        "GB": "EU",
        "DE": "EU",
        "FR": "EU",
        "ES": "EU",
        "IT": "EU",
        "PT": "EU",
        "TR": "EU",
        "SU": "EU",
        "NO": "EU",
        "NL": "EU",
        "PL": "EU",
        "HU": "EU",
        "CZ": "EU",
        "GR": "EU",
        "LU": "EU",
        "LV": "EU",
        "SE": "EU",
        # South America
        "BR": "SA",
        # North America
        "US": "NA",
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

    @staticmethod
    def GetEmulationstationLanguage():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        # Try to obtain from system language, then fallback to US or 
        esLanguage = conf.getOption("system.language", "$")[0:2].upper()
        return esLanguage

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

    # Check if Gamecube bios IPL.bin in folders
    @staticmethod
    def CheckGamecubeIpl():
        ipl0 = "/usr/share/dolphin-emu/sys/GC/EUR/IPL.bin"
        ipl1 = "/usr/share/dolphin-emu/sys/GC/JP/IPL.bin"
        ipl2 = "/usr/share/dolphin-emu/sys/GC/USA/IPL.bin"
        # if os.path? return "True" set "False" for disable "SkipIpl=Video boot intro"
        if os.path.exists(ipl0 or ipl1 or ipl2):
            return("False")
        else:
            return("True")

    def mainConfiguration(self):
        # Get Languages
        language = self.GetLanguage()
        systemLanguage = self.GetEmulationstationLanguage()
        gamecubeLanguage = self.GAMECUBE_LANGUAGES[language] if language in self.GAMECUBE_LANGUAGES else 0
        wiiLanguage = self.WII_LANGUAGES[language] if language in self.WII_LANGUAGES else 0
        # Get Netplay configs
        nickname = self.GetNetplay()
        lobbyServer = self.NETPLAY_SERVERS[systemLanguage] if systemLanguage in self.NETPLAY_SERVERS else None
        ## Get if have all IPL.bin in folders Gc bios
        biosGamecube = self.CheckGamecubeIpl()
        
        # Load Configuration
        dolphinSettings = IniSettings(recalboxFiles.dolphinIni, True)
        dolphinSettings.loadFile(True)

        # Interface
        dolphinSettings.setOption(self.SECTION_INTERFACE, "LanguageCode", language)
        dolphinSettings.setOption(self.SECTION_INTERFACE, "AutoHideCursor", "True")
        dolphinSettings.setOption(self.SECTION_INTERFACE, "ConfirmStop", "False")
        # Core
        dolphinSettings.setOption(self.SECTION_CORE, "SelectedLanguage", gamecubeLanguage) ## Game languages
        dolphinSettings.setOption(self.SECTION_INTERFACE, "Language", wiiLanguage) ## Game languages
        dolphinSettings.setOption(self.SECTION_CORE, "WiimoteContinuousScanning", "True")
        dolphinSettings.setOption(self.SECTION_CORE, "WiiKeyboard", "False")
        dolphinSettings.setOption(self.SECTION_CORE, "SkipIpl", biosGamecube) ##
        # General
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPaths", "2") ## Scan iso
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPath0", "/recalbox/share/roms/gamecube") ## Scan iso 
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPath1", "/recalbox/share/roms/wii") ## Scan iso
        # Analytics
        dolphinSettings.setOption(self.SECTION_ANALYTICS, "Enabled", "True")
        dolphinSettings.setOption(self.SECTION_ANALYTICS, "PermissionAsked", "True")
        # BluetoothPasstrough
        dolphinSettings.setOption(self.SECTION_BTPASSTHROUGH, "Enabled", "False")
        # Netplay
        dolphinSettings.setOption(self.SECTION_NETPLAY, "Nickname", nickname)
        dolphinSettings.setOption(self.SECTION_NETPLAY,  "UseUPNP", "True") ## force UpNp
        dolphinSettings.setOption(self.SECTION_NETPLAY, "IndexName", nickname) ## name of room in dolphin netplay server
        dolphinSettings.setOption(self.SECTION_NETPLAY, "TraversalChoice", "traversal")
        dolphinSettings.setOption(self.SECTION_NETPLAY, "UseIndex", "True") ## show room in dolphin lobby
        dolphinSettings.setOption(self.SECTION_NETPLAY, "IndexRegion", lobbyServer) ## choose server lobby 

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

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-e", rom]
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])
        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.CONF, "XDG_DATA_HOME":recalboxFiles.SAVES})
