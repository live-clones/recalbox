#!/usr/bin/env python
import Command
import ConfigParser
import recalboxFiles
import supermodelControllers
from generators.Generator import Generator
from settings.iniSettings import IniSettings
from settings.keyValueSettings import keyValueSettings
from utils.videoMode import *

class SupermodelGenerator(Generator):

    SECTION_GLOBAL = "Global"

    ## -----AUDIO-----
    @staticmethod
    def GetMusicVolume(system):
        musicVolume = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        MusicVol = supermodelSettings.getOption("music-volume", "")
        if MusicVol == "100" :
            None
        else:
            musicVolume.append("-music-volume="+MusicVol)
        return musicVolume

    @staticmethod
    def GetNoSound(system):
        noSound = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        soundState = supermodelSettings.getOption("no-sound", "")
        if soundState == "1" :
            noSound.append("-no-sound")
        return noSound

    @staticmethod
    def GetVolume(system):
        Volume = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        Vol = supermodelSettings.getOption("sound-volume", "")
        if Vol == "100" :
            None
        else:
            Volume.append("-sound-volume="+Vol)
        return Volume

    @staticmethod
    def GetDsb(system):
        DSB = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        currentDsb = supermodelSettings.getOption("no-dsb", "")
        if currentDsb == "1" :
            DSB.append("-no-dsb")
        return DSB

    @staticmethod
    def GetFlipStereo(system):
        flipStereo = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        SwitchStereo = supermodelSettings.getOption("flip-stereo", "")
        if SwitchStereo == "1" :
            flipStereo.append("-flip-stereo")
        return flipStereo

    @staticmethod
    def GetBalance(system):
        BalanceSound = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        balance = supermodelSettings.getOption("balance", "")
        if balance == "0":
            None
        else:
            BalanceSound.append("-balance="+balance)
        return BalanceSound

    ## added in version r818 define new or old sound engine 
    @staticmethod
    def GetSoundEngine(system):
        SoundEngine = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        soundType = supermodelSettings.getOption("sound-engine", "")
        if soundType == "0":
            SoundEngine.append("-new-scsp")
        else:
            SoundEngine.append("-legacy-scsp")
        return SoundEngine

    ## -----VIDEO-----
    @staticmethod
    def GetResolution(videoConfig):
        ## get current resolution.
        reso = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        width, height = getCurrentResulution()
        currentResolution = ("{}".format(width))+","+("{}".format(height))
        getConfig = supermodelSettings.getOption("resolution", "")
        if getConfig == "auto" :
            reso.append("-fullscreen")
            reso.append("-res="+currentResolution)
        elif getConfig == "none" :
            reso.append("-fullscreen")
        else :
            reso.append("-fullscreen")
            reso.append("-res="+getConfig)
        return reso

    @staticmethod
    def GetScreenRatio(system):
        ratio = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        screenRatio = supermodelSettings.getOption("screen-ratio", "")
        if screenRatio == "1" :
            ratio.append("-wide-screen")
        elif screenRatio == "2" :
            ratio.append("-stretch")
        return ratio

    @staticmethod
    def GetThrottle(system):
        Throttle = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        throttle = supermodelSettings.getOption("no-throttle", "")
        if throttle == "1" :
            Throttle.append("-no-throttle")
        return Throttle

    @staticmethod
    def GetMultiTexture(system):
        Texture = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        MultiTexture = supermodelSettings.getOption("multi-texture", "")
        if MultiTexture == "1" :
            Texture.append("-multi-texture")
        else:
            Texture.append("-no-multi-texture")
        return Texture

    @staticmethod
    def GetRendering(system):
        Rendering = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        QuadRendering = supermodelSettings.getOption("quad-rendering", "")
        if QuadRendering == "1" :
            Rendering.append("-quad-rendering")
        return Rendering

    @staticmethod
    def GetEngine(system):
        ## set current engine new3d work but in "intel" use legacy3d engine
        legacy = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        legacy3d = supermodelSettings.getOption("3d-engine", "")
        if legacy3d == "1" :
            legacy.append("-legacy3d") 
        return legacy

    @staticmethod
    def GetCrosshairs(system):
        SetCrosshairs = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        Crosshairs = supermodelSettings.getOption("crosshairs", "")
        ## on default set 3 (for playres one and two) if enabled in config file 
        if Crosshairs == "1" :
            SetCrosshairs.append("-crosshairs=1")
        elif Crosshairs == "2" :
            SetCrosshairs.append("-crosshairs=3")
        return SetCrosshairs

## show only on statut bar 
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
    def GetGpuThread(system):
        gpuThread = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        GpuThreadStatut = supermodelSettings.getOption("no-gpu-thread", "")
        if GpuThreadStatut == "1" :
            gpuThread.append("-no-gpu-thread")
        return gpuThread

    @staticmethod
    def GetThreads(system):
        Threads = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        ThreadsStatut = supermodelSettings.getOption("no-threads", "")
        if ThreadsStatut == "1" :
            Threads.append("-no-threads")
        return Threads

    @staticmethod
    def GetPpcFrequency(system):
        PpcFrequency = []
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        Frequency = supermodelSettings.getOption("ppc-frequency", "")
        if Frequency == "50":
            None
        else:
            PpcFrequency.append("-ppc-frequency="+Frequency)
        return PpcFrequency

    @staticmethod
    def GetSavesState(system):
        savespath =[]
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        model3saves = "/recalbox/share/saves/model3/"
        savespath.append("-load-state="+model3saves)
        return savespath

    @staticmethod
    def GetLogsPath(system):
        logpath =[]
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        model3logs = "/recalbox/share/system/configs/model3/Supermodel.log"
        logpath.append("-log-output="+model3logs)
        return logpath

    @staticmethod
    def GetLogsLevel(system):
        loglevel =[]
        supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
        supermodelSettings.loadFile(True)
        loglevelset = supermodelSettings.getOption("log-level", "")
        loglevel.append("-log-level="+loglevelset)
        return loglevel

    def generate(self, system, playersControllers, recalboxSettings, args):
        if not system.config['configfile']:

            # Controllers
            supermodelControllers.generateControllerConfig(self, Generator, playersControllers, system)

            commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], args.rom]
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
        commandArray.extend(self.GetGpuThread(system))
        commandArray.extend(self.GetThreads(system))
        commandArray.extend(self.GetPpcFrequency(system))
        commandArray.extend(self.GetSavesState(system))
        commandArray.extend(self.GetLogsPath(system))
        commandArray.extend(self.GetLogsLevel(system))

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
