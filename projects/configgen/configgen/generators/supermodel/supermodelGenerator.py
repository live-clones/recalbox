#!/usr/bin/env python
from typing import List

from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class SupermodelGenerator(Generator):

    SECTION_GLOBAL = "Global"

    ## -----AUDIO-----
    @staticmethod
    def GetMusicVolume(_) -> List[str]:
        musicVolume: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        MusicVol = supermodelSettings.getInt("music-volume", 100)
        print("MusicVol:{} {}".format(MusicVol, recalboxFiles.supermodelConfigFile))
        if MusicVol != 100:
            musicVolume.append("-music-volume={}".format(MusicVol))
        return musicVolume

    @staticmethod
    def GetNoSound(_) -> List[str]:
        noSound: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        soundState = supermodelSettings.getInt("no-sound", 0)
        if soundState == 1:
            noSound.append("-no-sound")
        return noSound

    @staticmethod
    def GetVolume(_) -> List[str]:
        Volume: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        Vol = supermodelSettings.getInt("sound-volume", 100)
        if Vol != 100:
            Volume.append("-sound-volume={}".format(Vol))
        return Volume

    @staticmethod
    def GetDsb(_) -> List[str]:
        DSB: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        currentDsb = supermodelSettings.getInt("no-dsb", 0)
        if currentDsb == 1:
            DSB.append("-no-dsb")
        return DSB

    @staticmethod
    def GetFlipStereo(_) -> List[str]:
        flipStereo: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        SwitchStereo = supermodelSettings.getInt("flip-stereo", 0)
        if SwitchStereo == 1:
            flipStereo.append("-flip-stereo")
        return flipStereo

    @staticmethod
    def GetBalance(_) -> List[str]:
        BalanceSound: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        balance = supermodelSettings.getInt("balance", 0)
        if balance != 0:
            BalanceSound.append("-balance={}".format(balance))
        return BalanceSound

    ## added in version r818 define new or old sound engine 
    @staticmethod
    def GetSoundEngine(_) -> List[str]:
        SoundEngine: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        soundType = supermodelSettings.getInt("sound-engine", -1)
        if soundType == 0:
            SoundEngine.append("-new-scsp")
        else:
            SoundEngine.append("-legacy-scsp")
        return SoundEngine

    ## -----VIDEO-----
    @staticmethod
    def GetResolution(_) -> List[str]:
        ## get current resolution.
        reso: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        getConfig = supermodelSettings.getString("resolution", "")
        if getConfig == "auto":
            from configgen.utils.videoMode import getCurrentResolution
            width, height = getCurrentResolution()
            currentResolution = ("{}".format(width)) + "," + ("{}".format(height))
            reso.append("-fullscreen")
            reso.append("-res=" + currentResolution)
        elif getConfig == "none":
            reso.append("-fullscreen")
        else:
            reso.append("-fullscreen")
            reso.append("-res=" + getConfig)
        return reso

    ## added in version r835 :
    ## -wide-bg When wide-screen mode is enabled, also expand the 2D background layer to screen width
    @staticmethod
    def GetScreenRatio(system: Emulator) -> List[str]:
        ratio: List[str] = []
        if system.Ratio == "16/9":
            ratio.append("-wide-screen")
            ratio.append("-wide-bg")
        return ratio

    @staticmethod
    def GetThrottle(_) -> List[str]:
        Throttle: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        throttle: int = supermodelSettings.getInt("no-throttle", 0)
        if throttle == 1:
            Throttle.append("-no-throttle")
        return Throttle

    @staticmethod
    def GetMultiTexture(_) -> List[str]:
        Texture: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        MultiTexture = supermodelSettings.getInt("multi-texture", 0)
        if MultiTexture == 1:
            Texture.append("-multi-texture")
        else:
            Texture.append("-no-multi-texture")
        return Texture

    @staticmethod
    def GetRendering(_) -> List[str]:
        Rendering: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        QuadRendering: int = supermodelSettings.getInt("quad-rendering", 0)
        if QuadRendering == "1":
            Rendering.append("-quad-rendering")
        return Rendering

    @staticmethod
    def GetEngine(_) -> List[str]:
        # Set new3d engine only if have a nvidia driver else use legacy3d engine
        # Is nVidia driver on?
        import platform
        arch = platform.machine()
        hasNVidia = False
        if arch == "x86_64":
            # check if have nvidia driver
            import os
            if os.path.exists("/etc/modprobe.d/blacklist.conf"):
                with open("/etc/modprobe.d/blacklist.conf") as f:
                    for line in f:
                        if "blacklist nouveau" in line:
                            hasNVidia = True
                            break
        return [] if hasNVidia else ['-legacy3d']

    @staticmethod
    def GetCrosshairs(_) -> List[str]:
        SetCrosshairs: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        Crosshairs: int = supermodelSettings.getInt("crosshairs", 0)
        ## on default set 3 (for players one and two) if enabled in config file
        if Crosshairs == 1:
            SetCrosshairs.append("-crosshairs=1")
        elif Crosshairs == 2:
            SetCrosshairs.append("-crosshairs=3")
        return SetCrosshairs

## Disabled because FPS show only on statut bar
#    ## -----CORE-----
#    @staticmethod
#    def GetShowsFps(system):
#        ShowFps = []
#        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
#        supermodelSettings.loadFile(True)
#        showfps = supermodelSettings.getOption("show-fps", "")
#        if showfps == "1" :
#            ShowFps.append("-show-fps")
#        return ShowFps

    @staticmethod
    def GetThreads(_) -> List[str]:
        Thread: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        ThreadStatut: int = supermodelSettings.getInt("multi-threading", 0)
        if ThreadStatut == 1:
            Thread.append("-no-gpu-thread")
        elif ThreadStatut == 2:
            Thread.append("no-threads")
        else:
            Thread.append("-gpu-multi-threaded")
        return Thread

    @staticmethod
    def GetPpcFrequency(_) -> List[str]:
        PpcFrequency: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        Frequency: int = supermodelSettings.getInt("ppc-frequency", 50)
        if Frequency != "50":
            PpcFrequency.append("-ppc-frequency={}".format(Frequency))
        return PpcFrequency

## Disabled because make crash on few games
    @staticmethod
    def GetSavesState(_) -> List[str]:
        savespath: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        model3saves = "/recalbox/share/saves/model3/"
        savespath.append("-load-state=" + model3saves)
        return savespath

    @staticmethod
    def GetLogsPath(_) -> List[str]:
        logpath: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        model3logs = recalboxFiles.supermodelRooFolder + "/Supermodel.log"
        logpath.append("-log-output=" + model3logs)
        return logpath

    @staticmethod
    def GetLogsLevel(_) -> List[str]:
        loglevel: List[str] = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        loglevelset = supermodelSettings.getString("log-level", "")
        loglevel.append("-log-level=" + loglevelset)
        return loglevel

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        if not system.HasConfigFile:
            # Controllers
            import configgen.generators.supermodel.supermodelControllers as supermodelControllers
            supermodelControllers.generateControllerConfig(self, playersControllers)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], args.rom]
        ## add other cmd option for more configuration
        ## -----AUDIO------
        commandArray.extend(self.GetMusicVolume(system))
        commandArray.extend(self.GetVolume(system))
        commandArray.extend(self.GetFlipStereo(system))
        commandArray.extend(self.GetDsb(system))
        commandArray.extend(self.GetBalance(system))
        commandArray.extend(self.GetSoundEngine(system))
        commandArray.extend(self.GetNoSound(system))
        ## -----VIDEO------
        commandArray.extend(self.GetResolution(system))
        commandArray.extend(self.GetScreenRatio(system))
        commandArray.extend(self.GetThrottle(system))
        commandArray.extend(self.GetMultiTexture(system))
        commandArray.extend(self.GetRendering(system))
        commandArray.extend(self.GetEngine(system))
        commandArray.extend(self.GetCrosshairs(system))
        ## -----CORE------
#        commandArray.extend(self.GetShowsFps(system))
        commandArray.extend(self.GetThreads(system))
        commandArray.extend(self.GetPpcFrequency(system))
        commandArray.extend(self.GetSavesState(system))
        commandArray.extend(self.GetLogsPath(system))
        commandArray.extend(self.GetLogsLevel(system))

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
