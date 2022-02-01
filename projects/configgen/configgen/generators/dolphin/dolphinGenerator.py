#!/usr/bin/env python
from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
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
    def GetLanguage() -> str:
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        # Try to obtain from keyboard layout, then from system language, then fallback to us
        kl = conf.getString("system.kblayout", conf.getString("system.language", "en")[0:2]).lower()
        return kl

    @staticmethod
    def GetEmulationstationLanguage() -> str:
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        # Try to obtain from system language, then fallback to US or 
        esLanguage = conf.getString("system.language", "$")[0:2].upper()
        return esLanguage

    # Get netplay username
    @staticmethod
    def GetNetplay() -> str:
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        speudo = conf.getString("global.netplay.nickname", "$")
        return speudo

    # Check if Gamecube bios IPL.bin in folders
    @staticmethod
    def CheckGamecubeIpl() -> str:
        ipl0 = "/recalbox/share/bios/gamecube/EUR/IPL.bin"
        ipl1 = "/recalbox/share/bios/gamecube/JP/IPL.bin"
        ipl2 = "/recalbox/share/bios/gamecube/USA/IPL.bin"
        # if os.path? return "True" set "False" for disable "SkipIpl=Video boot intro"
        import os.path
        if os.path.exists(ipl0) or os.path.exists(ipl1) or os.path.exists(ipl2):
            return "False"
        else:
            return "True"

    # GameCube Controller Adapter for Wii U in Dolphin controllers if realgamecubepads=1 set in recalbox.conf
    @staticmethod
    def SetGamecubeWiiuAdapter() -> str:
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        adapterStatus = conf.getString("gamecube.realgamecubepads", "$")
        return adapterStatus

    # Set Wii sensor bar position in SYSCONF if wii.sensorbar.position=0 (bottom) or =1 (top) set in recalbox.conf
	# (source : https://wiibrew.org/wiki//shared2/sys/SYSCONF#BT_Settings)
    @staticmethod
    def SetSensorBarPosition():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        sensorBarPosition = conf.getString("wii.sensorbar.position", "$")
        keyValue = '\x00'.encode('utf-8') if sensorBarPosition == '0' else '\x01'.encode('utf-8')
        keyString = "BT.BAR".encode('utf-8')
        import os.path
        if os.path.exists(recalboxFiles.dolphinSYSCONF):
            with open(recalboxFiles.dolphinSYSCONF, 'rb+') as sysconf:
                buf = sysconf.read()
                keyAddr = buf.find(keyString) + len(keyString)
                sysconf.seek(keyAddr)
                sysconf.write(keyValue)

    def mainConfiguration(self, system: Emulator, args):
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
        from configgen.settings.iniSettings import IniSettings
        dolphinSettings = IniSettings(recalboxFiles.dolphinIni, True)
        dolphinSettings.loadFile(True) \
                       .defineBool('True', 'False')

        # Resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet and resolution.selfProcess:
            dolphinSettings.setString(self.SECTION_DISPLAY, "FullscreenResolution", resolution.string)
            dolphinSettings.setBool(self.SECTION_ANALYTICS, "Fullscreen", True)
        # Logging
        dolphinSettings.setBool(self.SECTION_DSP, "CaptureLog", args.verbose)
        # Force audio backend on Pulse Audio
        dolphinSettings.setString(self.SECTION_DSP, "Backend", "Pulse")
        # Analytics
        dolphinSettings.setBool(self.SECTION_ANALYTICS, "Enabled", True)
        dolphinSettings.setBool(self.SECTION_ANALYTICS, "PermissionAsked", True)
        # BluetoothPasstrough
        dolphinSettings.setBool(self.SECTION_BTPASSTHROUGH, "Enabled", False)
        # Core
        dolphinSettings.setInt(self.SECTION_CORE, "SelectedLanguage", gamecubeLanguage)
        dolphinSettings.setBool(self.SECTION_CORE, "WiimoteContinuousScanning", True)
        dolphinSettings.setBool(self.SECTION_CORE, "WiiKeyboard", False)
        dolphinSettings.setString(self.SECTION_CORE, "SkipIpl", biosGamecube)
        dolphinSettings.setInt(self.SECTION_CORE, "SIDevice0", 12 if padsgamecube == '1' else 6)
        dolphinSettings.setInt(self.SECTION_CORE, "SIDevice1", 12 if padsgamecube == '1' else 6)
        dolphinSettings.setInt(self.SECTION_CORE, "SIDevice2", 12 if padsgamecube == '1' else 6)
        dolphinSettings.setInt(self.SECTION_CORE, "SIDevice3", 12 if padsgamecube == '1' else 6)
        dolphinSettings.setInt(self.SECTION_CORE, "SlotA", 8)
        dolphinSettings.setInt(self.SECTION_CORE, "SlotB", 8)
        # GameList
        dolphinSettings.setBool(self.SECTION_GAMELIST, "ColumnID", True)
        # General
        dolphinSettings.setInt(self.SECTION_GENERAL, "ISOPaths", 2)
        dolphinSettings.setString(self.SECTION_GENERAL, "ISOPath0", "/recalbox/share/roms/gamecube")
        dolphinSettings.setString(self.SECTION_GENERAL, "ISOPath1", "/recalbox/share/roms/wii")
        dolphinSettings.setBool(self.SECTION_GENERAL, "RecursiveISOPaths", True)
        # Interface
        dolphinSettings.setString(self.SECTION_INTERFACE, "LanguageCode", language)
        dolphinSettings.setBool(self.SECTION_INTERFACE, "AutoHideCursor", True)
        dolphinSettings.setBool(self.SECTION_INTERFACE, "ConfirmStop", False)
        dolphinSettings.setInt(self.SECTION_INTERFACE, "Language", wiiLanguage)
        # Netplay
        dolphinSettings.setString(self.SECTION_NETPLAY, "Nickname", nickname)
        dolphinSettings.setBool(self.SECTION_NETPLAY, "UseUPNP", True)
        dolphinSettings.setString(self.SECTION_NETPLAY, "IndexName", nickname)
        dolphinSettings.setString(self.SECTION_NETPLAY, "TraversalChoice", "traversal")
        dolphinSettings.setBool(self.SECTION_NETPLAY, "UseIndex", True)
        dolphinSettings.setString(self.SECTION_NETPLAY, "IndexRegion", lobbyServer)

        # Save configuration
        dolphinSettings.saveFile()
        
        # Set Wii sensor bar position
        self.SetSensorBarPosition()


    def gfxConfiguration(self, system: Emulator):
        # Get Ratio
        gameRatio = self.GAME_RATIO[system.Ratio] if system.Ratio in self.GAME_RATIO else 0
        
        # Load Configuration
        from configgen.settings.iniSettings import IniSettings
        gfxSettings = IniSettings(recalboxFiles.dolphinGFX, True)
        gfxSettings.loadFile(True)

        # Hardware
        gfxSettings.setBool(self.SECTION_HARDWARE, "VSync", True)
        # Settings
        gfxSettings.setBool(self.SECTION_SETTINGS, "ShowFPS", system.ShowFPS)
        gfxSettings.setInt(self.SECTION_SETTINGS, "AspectRatio", gameRatio)
        gfxSettings.setBool(self.SECTION_SETTINGS, "HiresTextures", True)
        gfxSettings.setBool(self.SECTION_SETTINGS, "CacheHiresTextures", True)

        # Save configuration
        gfxSettings.saveFile()

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        if not system.HasConfigFile:
            # Controllers
            import configgen.generators.dolphin.dolphinControllers as dolphinControllers
            dolphinControllers.generateControllerConfig(system, playersControllers, recalboxOptions)

            self.mainConfiguration(system, args)
            self.gfxConfiguration(system)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], "-e", args.rom]
        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.CONF, "XDG_DATA_HOME":recalboxFiles.SAVES})
