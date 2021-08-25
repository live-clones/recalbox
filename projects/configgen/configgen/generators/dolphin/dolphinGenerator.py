#!/usr/bin/env python
import os.path
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
import configgen.generators.dolphin.dolphinControllers as dolphinControllers
from configgen.settings.iniSettings import IniSettings
from configgen.settings.keyValueSettings import keyValueSettings

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
    ## Setting on dolphin.ini
    SECTION_ANALYTICS = "Analytics"
    SECTION_BTPASSTHROUGH = "BluetoothPassthrough"
    SECTION_CONTROLS = "Controls"
    SECTION_CORE = "Core"
    SECTION_GAMELIST = "GameList"
    SECTION_GENERAL = "General"
    SECTION_INTERFACE = "Interface"
    SECTION_NETPLAY = "NetPlay"
    SECTION_DISPLAY = "Display"
    SECTION_DSP = "DSP"
    ## Setting on GFX.ini
    SECTION_SETTINGS = "Settings"
    SECTION_HARDWARE = "Hardware"

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
        ipl0 = "/recalbox/share/bios/gamecube/EUR/IPL.bin"
        ipl1 = "/recalbox/share/bios/gamecube/JP/IPL.bin"
        ipl2 = "/recalbox/share/bios/gamecube/USA/IPL.bin"
        # if os.path? return "True" set "False" for disable "SkipIpl=Video boot intro"
        if os.path.exists(ipl0) or os.path.exists(ipl1) or os.path.exists(ipl2):
            return "False"
        else:
            return "True"

    # GameCube Controller Adapter for Wii U in Dolphin controllers if realgamecubepads=1 set in recalbox.conf
    @staticmethod
    def SetGamecubeWiiuAdapter():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        adapterStatus = conf.getOption("gamecube.realgamecubepads", "$")
        return adapterStatus

    # Set Wii sensor bar position in SYSCONF if wii.sensorbar.position=0 (bottom) or =1 (top) set in recalbox.conf
	# (source : https://wiibrew.org/wiki//shared2/sys/SYSCONF#BT_Settings)
    @staticmethod
    def SetSensorBarPosition():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        sensorBarPosition = conf.getOption("wii.sensorbar.position", "$")
        keyValue = '\x00'.encode('utf-8') if sensorBarPosition == '0' else '\x01'.encode('utf-8')
        keyString = "BT.BAR".encode('utf-8')
        if os.path.exists(recalboxFiles.dolphinSYSCONF):
            with open(recalboxFiles.dolphinSYSCONF, 'rb+') as sysconf:
                buf = sysconf.read()
                keyAddr = buf.find(keyString) + len(keyString)
                sysconf.seek(keyAddr)
                sysconf.write(keyValue)

    def mainConfiguration(self, system, args):
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
        ## set gamecube wiiu adapter
        padsgamecube = self.SetGamecubeWiiuAdapter()
        
        # Load Configuration
        dolphinSettings = IniSettings(recalboxFiles.dolphinIni, True)
        dolphinSettings.loadFile(True)

        # Resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.config['videomode'])
        if resolution.isSet and resolution.selfProcess:
            dolphinSettings.setOption(self.SECTION_DISPLAY, "FullscreenResolution", resolution.string)
            dolphinSettings.setOption(self.SECTION_ANALYTICS, "Fullscreen", "True")
        # Logging
        dolphinSettings.setOption(self.SECTION_DSP, "CaptureLog", "True" if args.verbose else "False")
        # Force audio backend on Pulse Audio
        dolphinSettings.setOption(self.SECTION_DSP, "Backend", "Pulse" )
        # Analytics
        dolphinSettings.setOption(self.SECTION_ANALYTICS, "Enabled", "True")
        dolphinSettings.setOption(self.SECTION_ANALYTICS, "PermissionAsked", "True")
        # BluetoothPasstrough
        dolphinSettings.setOption(self.SECTION_BTPASSTHROUGH, "Enabled", "False")
        # Core
        dolphinSettings.setOption(self.SECTION_CORE, "SelectedLanguage", gamecubeLanguage)
        dolphinSettings.setOption(self.SECTION_CORE, "WiimoteContinuousScanning", "True")
        dolphinSettings.setOption(self.SECTION_CORE, "WiiKeyboard", "False")
        dolphinSettings.setOption(self.SECTION_CORE, "SkipIpl", biosGamecube)
        dolphinSettings.setOption(self.SECTION_CORE, "SIDevice0", "12" if padsgamecube == '1' else "6")
        dolphinSettings.setOption(self.SECTION_CORE, "SIDevice1", "12" if padsgamecube == '1' else "6")
        dolphinSettings.setOption(self.SECTION_CORE, "SIDevice2", "12" if padsgamecube == '1' else "6")
        dolphinSettings.setOption(self.SECTION_CORE, "SIDevice3", "12" if padsgamecube == '1' else "6")
        dolphinSettings.setOption(self.SECTION_CORE, "SlotA", "8")
        dolphinSettings.setOption(self.SECTION_CORE, "SlotB", "8")
        # GameList
        dolphinSettings.setOption(self.SECTION_GAMELIST, "ColumnID", "True")
        # General
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPaths", "2")
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPath0", "/recalbox/share/roms/gamecube") 
        dolphinSettings.setOption(self.SECTION_GENERAL, "ISOPath1", "/recalbox/share/roms/wii")
        dolphinSettings.setOption(self.SECTION_GENERAL, "RecursiveISOPaths", "True")
        # Interface
        dolphinSettings.setOption(self.SECTION_INTERFACE, "LanguageCode", language)
        dolphinSettings.setOption(self.SECTION_INTERFACE, "AutoHideCursor", "True")
        dolphinSettings.setOption(self.SECTION_INTERFACE, "ConfirmStop", "False")
        dolphinSettings.setOption(self.SECTION_INTERFACE, "Language", wiiLanguage)
        # Netplay
        dolphinSettings.setOption(self.SECTION_NETPLAY, "Nickname", nickname)
        dolphinSettings.setOption(self.SECTION_NETPLAY, "UseUPNP", "True")
        dolphinSettings.setOption(self.SECTION_NETPLAY, "IndexName", nickname)
        dolphinSettings.setOption(self.SECTION_NETPLAY, "TraversalChoice", "traversal")
        dolphinSettings.setOption(self.SECTION_NETPLAY, "UseIndex", "True")
        dolphinSettings.setOption(self.SECTION_NETPLAY, "IndexRegion", lobbyServer)

        # Save configuration
        dolphinSettings.saveFile()
        
        # Set Wii sensor bar position
        self.SetSensorBarPosition()


    def gfxConfiguration(self, system):
        # Get Ratio
        self.system = system
        recalbox = self.system.config
        gameRatio = self.GAME_RATIO[recalbox['ratio']] if recalbox['ratio'] in self.GAME_RATIO else 0
        
        # Load Configuration
        gfxSettings = IniSettings(recalboxFiles.dolphinGFX, True)
        gfxSettings.loadFile(True)

        # Hardware
        gfxSettings.setOption(self.SECTION_HARDWARE, "VSync", "True")
        # Settings
        gfxSettings.setOption(self.SECTION_SETTINGS, "ShowFPS", "True" if system.config['showFPS'] == 'true' else "False")
        gfxSettings.setOption(self.SECTION_SETTINGS, "AspectRatio", gameRatio)
        gfxSettings.setOption(self.SECTION_SETTINGS, "HiresTextures", "True")
        gfxSettings.setOption(self.SECTION_SETTINGS, "CacheHiresTextures", "True")

        # Save configuration
        gfxSettings.saveFile()

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        if not system.config['configfile']:
            # Controllers
            dolphinControllers.generateControllerConfig(system, playersControllers)

            self.mainConfiguration(system, args)
            self.gfxConfiguration(system)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-e", args.rom]
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])
        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.CONF, "XDG_DATA_HOME":recalboxFiles.SAVES})
