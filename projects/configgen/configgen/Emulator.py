from enum import Enum
from typing import List, Union

from configgen.crt.CRTConfigParser import CRTVideoStandard, CRTResolutionType, CRTScreenType, CRTRegion
from configgen.settings.keyValueSettings import keyValueSettings


class ExtraArguments:
    def __init__(self, hash:str, netplay:str, netplay_ip:str, netplay_port:str, netplay_playerpassword:str,
                 netplay_viewerpassword:str, netplay_vieweronly:str,
                 crtvideostandard:str, crtresolutiontype:str, crtscreentype:str, crtadaptor:str,
                 crtverticaloffset: int, crthorizontaloffset: int, crtviewportwidth: int, crtregion:str = "auto"):
        self.hash = hash
        self.netplay = netplay
        self.netplay_ip = netplay_ip
        self.netplay_port = netplay_port
        self.netplay_playerpassword = netplay_playerpassword
        self.netplay_viewerpassword = netplay_viewerpassword
        self.netplay_vieweronly = netplay_vieweronly
        self.crtvideostandard = crtvideostandard
        self.crtregion = crtregion
        self.crtresolutiontype = crtresolutiontype
        self.crtscreentype = crtscreentype
        self.crtadaptor = crtadaptor
        self.crtverticaloffset = crtverticaloffset
        self.crthorizontaloffset = crthorizontaloffset
        self.crtviewportwidth = crtviewportwidth


class Emulator:

    def __init__(self, name: str, emulator: str, core: str, videoMode: str='CEA 4 HDMI', ratio:str='auto'):
        self._name: str = name
        # Overriding vars - **Always define dezfault values here and ONLY here**
        self._emulator: str = emulator
        self._core: str = core
        self._ratio: str = ratio
        self._videoMode: str = videoMode
        self._configFile: str = ""
        self._shaderFile: str = ""
        self._shaderSet: str = ""
        self._showFPS: bool = False
        self._smooth: bool = True
        self._rewind: bool = False
        self._autoSave: bool = False
        self._integerScale: bool = False
        self._quitTwice: bool = False
        self._recalboxOverlays: bool = True
        self._translate: bool = False
        self._translateAPIKey: str = "RECALBOX"
        self._translateURL: str = "https://ztranslate.net/service?api_key={}"
        self._translateFrom: str = "auto"
        self._translateTo: str = "auto"
        self._extraArgs: str = ""  # Extra parameters from systemlist.xml commands
        self._configArgs: str = ""  # Extra parameters from recalbox.conf
        self._args: List[str] = [] # Array representation of ExtraArgs + ConfigArgs

        # Vars straight from recalbox.conf
        self._netplayNick: str = "Recalbox Anonymous"
        self._netplayMITM: str = ""
        self._specialKeys: str = ""
        self._retroachievements: bool = False
        self._retroachievementsHardcore: bool = False
        self._retroachievementsNickname: str = ""
        self._retroachievementsPassword: str = ""

        # Vars from arguments
        self._hash: str = "default"
        self._netplayHostMode: bool = False
        self._netplayIP: str = ""
        self._netplayPort: int = 55435
        self._netplayPlayerPassword: str = ""
        self._netplayViewerPassword: str = ""
        self._netplayViewerOnly: bool = False

        # CRT arguments
        self._crtvideostandard: CRTVideoStandard = CRTVideoStandard.AUTO
        self._crtregion: CRTRegion = CRTRegion.AUTO
        self._crtresolutiontype: CRTResolutionType = CRTResolutionType.Progressive
        self._crtscreentype: CRTScreenType = CRTScreenType.k15
        self._crtenabled: bool = False
        self._crtverticaloffset: int = 0
        self._crthorizontaloffset: int = 0
        self._crtviewportwidth: int = 0

        # Computed vars
        self._netplay: bool = False

    def __guessBestStringValue(self, recalboxConf: keyValueSettings, key: str, defaultValue: str) -> str:
        return recalboxConf.getString(self._name + '.' + key,
                                      recalboxConf.getString("global." + key, defaultValue))

    def __guessBestBoolValue(self, recalboxConf: keyValueSettings, key: str, defaultValue: bool) -> bool:
        result: str = recalboxConf.getString(self._name + '.' + key,
                                             recalboxConf.getString("global." + key, '1' if defaultValue else '0'))
        if result in ('1', "true", "True"): return True
        return False

    def configure(self, recalboxOptions: keyValueSettings, arguments: ExtraArguments):
        # Overriding vars
        #self._emulator: str          = self.__guessBestStringValue(recalboxOptions, "emulator", self._emulator)
        #self._core: str              = self.__guessBestStringValue(recalboxOptions, "core", self._core)
        self._ratio: str             = self.__guessBestStringValue(recalboxOptions, "ratio", self._ratio)
        self._videoMode: str         = self.__guessBestStringValue(recalboxOptions, "videomode", self._videoMode)
        self._shaderFile: str        = self.__guessBestStringValue(recalboxOptions, "shaders", self._shaderFile)
        self._shaderSet: str         = self.__guessBestStringValue(recalboxOptions, "shaderset", self._shaderSet)
        self._configFile: str        = self.__guessBestStringValue(recalboxOptions, "configfile", self._configFile)
        self._showFPS: bool          = self.__guessBestBoolValue  (recalboxOptions, "showfps", self._showFPS)
        self._smooth: bool           = self.__guessBestBoolValue  (recalboxOptions, "smooth", self._smooth)
        self._rewind: bool           = self.__guessBestBoolValue  (recalboxOptions, "rewind", self._rewind)
        self._autoSave: bool         = self.__guessBestBoolValue  (recalboxOptions, "autosave", self._autoSave)
        self._integerScale: bool     = self.__guessBestBoolValue  (recalboxOptions, "integerscale", self._integerScale)
        self._quitTwice: bool        = self.__guessBestBoolValue  (recalboxOptions, "quitpresstwice", self._quitTwice)
        self._recalboxOverlays: bool = self.__guessBestBoolValue  (recalboxOptions, "recalboxoverlays", self._recalboxOverlays)
        self._netplay: bool          = self.__guessBestBoolValue  (recalboxOptions, "netplay", self._netplay)
        self._translate: bool        = self.__guessBestBoolValue  (recalboxOptions, "translate", self._translate)
        self._translateAPIKey: str   = self.__guessBestStringValue(recalboxOptions, "translate.apikey", self._translateAPIKey)
        self._translateURL: str      = self.__guessBestStringValue(recalboxOptions, "translate.url", self._translateURL)
        self._translateFrom: str     = self.__guessBestStringValue(recalboxOptions, "translate.from", self._translateFrom)
        self._translateTo: str       = self.__guessBestStringValue(recalboxOptions, "translate.to", recalboxOptions.getString("system.language", self._translateTo))
        self._extraArgs: str         = self.__guessBestStringValue(recalboxOptions, "extra", self._extraArgs)
        self._configArgs: str        = self.__guessBestStringValue(recalboxOptions, "args", self._configArgs)

        # Vars straight from recalbox.conf
        self._specialKeys               = recalboxOptions.getString('system.emulators.specialkeys', self._specialKeys)
        self._netplayNick               = recalboxOptions.getString('global.netplay.nickname', self._netplayNick)
        self._netplayMITM               = recalboxOptions.getString('global.netplay.relay', self._netplayMITM)
        self._retroachievements         = recalboxOptions.getBool('global.retroachievements', self._retroachievements)
        self._retroachievementsHardcore = recalboxOptions.getBool('global.retroachievements.hardcore', self._retroachievementsHardcore)
        self._retroachievementsNickname = recalboxOptions.getString('global.retroachievements.username', self._retroachievementsNickname)
        self._retroachievementsPassword = recalboxOptions.getString('global.retroachievements.password', self._retroachievementsPassword)

        # Vars from arguments
        self._hash                  = arguments.hash
        self._netplayHostMode       = arguments.netplay == "host"
        self._netplayIP             = arguments.netplay_ip
        self._netplayPort           = arguments.netplay_port
        self._netplayPlayerPassword = arguments.netplay_playerpassword
        self._netplayViewerPassword = arguments.netplay_viewerpassword
        self._netplayViewerOnly     = arguments.netplay_vieweronly

        # CRT arguments
        self._crtvideostandard: CRTVideoStandard = CRTVideoStandard.fromString(arguments.crtvideostandard)
        self._crtregion: CRTRegion = CRTRegion.fromString(arguments.crtregion)
        self._crtresolutiontype: CRTResolutionType = CRTResolutionType.fromString(arguments.crtresolutiontype)
        self._crtscreentype: CRTScreenType = CRTScreenType.fromString(arguments.crtscreentype)
        self._crtenabled: bool = arguments.crtadaptor is not None and len(arguments.crtadaptor) > 0
        self._crtverticaloffset =arguments.crtverticaloffset
        self._crthorizontaloffset = arguments.crthorizontaloffset
        self._crtviewportwidth = arguments.crtviewportwidth

        # Computed vars
        self._netplay               = arguments.netplay in ("host", "client")

        # Generate args array
        self._args = []
        # Split extra args
        if len(self._extraArgs) != 0:
            self._args.extend(self._extraArgs.split())
        # Optional emulator args
        if len(self._configArgs) != 0:
            import shlex
            self._args.extend(shlex.split(self._configArgs))

        # Shaders
        if len(self._shaderSet) != 0 and self._shaderSet != 'none':
            import configgen.recalboxFiles as recalboxFiles
            shaderFile = recalboxFiles.shaderPresetRoot + '/' + self._shaderSet + '.cfg'
            shaderContent = keyValueSettings(shaderFile, False)
            shaderContent.loadFile(True)
            systemShader = shaderContent.getString(self._name, "")
            if len(systemShader) != 0:
                self._shaderFile = systemShader

    # Only for lightgun
    def ChangeEmulatorAndCore(self, emulator: str, core: str):
        self._emulator = emulator
        self._core = core

    @property
    def Name(self) -> str: return self._name

    @property
    def Emulator(self) -> str: return self._emulator

    @property
    def Core(self) -> str: return self._core

    @property
    def Ratio(self) -> str: return self._ratio

    @property
    def ShowFPS(self) -> bool: return self._showFPS

    @property
    def Smooth(self) -> bool: return self._smooth

    @property
    def Rewind(self) -> bool: return self._rewind

    @property
    def RecalboxOverlays(self) -> bool: return self._recalboxOverlays

    @property
    def Args(self) -> List[str]: return self._args

    @property
    def HasArgs(self) -> bool: return len(self._args) > 0

    @property
    def Hash(self) -> str: return self._hash

    @property
    def HasHash(self) -> bool: return len(self._hash) != 0

    @property
    def ConfigFile(self) -> str: return self._configFile

    @property
    def HasConfigFile(self) -> bool: return len(self._configFile) != 0

    @property
    def ShaderFile(self) -> str: return self._shaderFile

    @property
    def HasShaderFile(self) -> bool: return len(self._shaderFile) != 0

    @property
    def ShaderSet(self) -> str: return self._shaderSet

    @property
    def SpecialKeys(self) -> str: return self._specialKeys

    @property
    def VideoMode(self) -> str: return self._videoMode

    @property
    def Netplay(self) -> bool: return self._netplay

    @property
    def NetplayUsername(self) -> str: return self._netplayNick

    @property
    def NetplayIP(self) -> str: return self._netplayIP

    @property
    def NetplayPort(self) -> int: return self._netplayPort

    @property
    def NetplayHostMode(self) -> bool: return self._netplayHostMode

    @property
    def NetplayViewerOnly(self) -> bool: return self._netplayViewerOnly

    @property
    def NetplayPlayerPassword(self) -> str: return self._netplayPlayerPassword

    @property
    def NetplayViewerPassword(self) -> str: return self._netplayViewerPassword

    @property
    def NetplayMITM(self) -> str: return self._netplayMITM

    @property
    def HasNetplayMITM(self) -> bool: return len(self._netplayMITM) != 0

    @property
    def Retroachievements(self) -> bool: return self._retroachievements

    @property
    def RetroachievementsHardcore(self) -> bool: return self._retroachievementsHardcore

    @property
    def RetroachievementsUsername(self) -> str: return self._retroachievementsNickname

    @property
    def RetroachievementsPassword(self) -> str: return self._retroachievementsPassword

    @property
    def QuitTwice(self) -> bool: return self._quitTwice

    @property
    def IntegerScale(self) -> bool: return self._integerScale

    @property
    def AutoSave(self) -> bool: return self._autoSave

    @property
    def Translate(self) -> bool: return self._translate

    @property
    def TranslateAPIKey(self) -> str: return self._translateAPIKey

    @property
    def TranslateURL(self) -> str: return self._translateURL

    @property
    def TranslateFrom(self) -> str: return self._translateFrom

    @property
    def TranslateTo(self) -> str: return self._translateTo

    @property
    def CRTVideoStandard(self) -> CRTVideoStandard: return self._crtvideostandard

    @property
    def CRTRegion(self) -> CRTRegion: return self._crtregion

    @property
    def CRTResolutionType(self) -> CRTResolutionType: return self._crtresolutiontype

    @property
    def CRTScreenType(self) -> CRTScreenType: return self._crtscreentype

    @property
    def CRTEnabled(self) -> bool: return self._crtenabled

    @property
    def CRTVerticalOffset(self) -> int: return self._crtverticaloffset

    @property
    def CRTHorizontalOffset(self) -> int: return self._crthorizontaloffset

    @property
    def CRTViewportWidth(self) -> int: return self._crtviewportwidth
