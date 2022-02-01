#!/usr/bin/env python
from typing import Dict

from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.emulatorlauncher import recalboxFiles
from configgen.generators.Generator import Generator
from configgen.settings.iniSettings import IniSettings
from configgen.settings.keyValueSettings import keyValueSettings

class Pcsx2Generator(Generator):

    pcsx2RootFolder = recalboxFiles.HOME + '/.config/PCSX2'
    pcsx2ConfigFileReg = pcsx2RootFolder + '/PCSX2-reg.ini'
    pcsx2ConfigFileGSdx = pcsx2RootFolder + '/GSdx.ini'
    pcsx2ConfigFilePAD = pcsx2RootFolder + '/PAD.ini'
    pcsx2ConfigFilePCSX2_ui = pcsx2RootFolder + '/PCSX2_ui.ini'
    pcsx2ConfigFilePCSX2_vm = pcsx2RootFolder + '/PCSX2_vm.ini'
    pcsx2ConfigFileSPU2 = pcsx2RootFolder + '/SPU2.ini'

    SECTION_DEFAULT = "default"
    # Core
    SECTION_VM_EMUCORE_GS = "EmuCore/GS"
    SECTION_VM_EMUCORE_SPEEDHACKS = "EmuCore/Speedhacks"
    # UI
    SECTION_UI_NONE = ""
    SECTION_UI_GSWINDOWS = "GSWindow"
    SECTION_UI_POPUPDISABLERS = "PopupDisablers"
    SECTION_UI_FILENAMES = "Filenames"
    # Sound (SPU2.ini)
    SECTION_SPU2_MIXING = "MIXING"
    SECTION_SPU2_OUTPUT = "OUTPUT"
    SECTION_SPU2_SDL = "SDL"

    # Bios names
    BIOS_USA = "ps2-0230a-20080220.bin"
    BIOS_EU  = "ps2-0230e-20080220.bin"
    BIOS_JP  = "ps2-0230j-20080220.bin"
    BIOS_HK  = "ps2-0230h-20080220.bin"

    @staticmethod
    def __ConfigurePad(playersControllers: ControllerPerPlayer):
        with open(Pcsx2Generator.pcsx2ConfigFilePAD, "w") as f:
            if 1 in playersControllers: f.write("uid[0] = {}\n".format(playersControllers[1].SdlIndex))
            if 2 in playersControllers: f.write("uid[1] = {}\n".format(playersControllers[2].SdlIndex))
            if 1 in playersControllers: f.write("SDL2 = {}\n".format(playersControllers[1].generateSDLGameDBLine(False)))
            if 2 in playersControllers: f.write("SDL2 = {}\n".format(playersControllers[2].generateSDLGameDBLine(False)))

    @staticmethod
    def __CheckPs2Bios(rom: str) -> str:
        import os.path
        def Exists(romPath: str) -> bool:
            return os.path.exists( '/recalbox/share/bios/ps2/' + romPath)

        # Try to get Redump region
        lrom = rom.lower()
        # EU region
        if   "europe" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "france" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "ireland" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "germany" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "belgium" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "italy" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "greece" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "croatia" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "denmark" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "spain" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "portugal" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "poland" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "finland" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "russia" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "austria" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "netherlands" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "switzerland" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "norway" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "sweden" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "scandinavia" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "australia" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "india" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "south africa" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        # USA region
        elif "canada" in lrom and Exists(Pcsx2Generator.BIOS_USA): return Pcsx2Generator.BIOS_USA
        elif "brazil" in lrom and Exists(Pcsx2Generator.BIOS_USA): return Pcsx2Generator.BIOS_USA
        elif "latin america" in lrom and Exists(Pcsx2Generator.BIOS_USA): return Pcsx2Generator.BIOS_USA
        # JP region
        elif "japan" in lrom and Exists(Pcsx2Generator.BIOS_JP): return Pcsx2Generator.BIOS_JP
        elif "korea" in lrom and Exists(Pcsx2Generator.BIOS_JP): return Pcsx2Generator.BIOS_JP
        elif "asia" in lrom and Exists(Pcsx2Generator.BIOS_JP): return Pcsx2Generator.BIOS_JP
        # CH
        elif "china" in lrom and Exists(Pcsx2Generator.BIOS_JP): return Pcsx2Generator.BIOS_HK
        elif "taiwan" in lrom and Exists(Pcsx2Generator.BIOS_JP): return Pcsx2Generator.BIOS_HK
        # Shortest name at the end
        elif "uk" in lrom and Exists(Pcsx2Generator.BIOS_EU): return Pcsx2Generator.BIOS_EU
        elif "usa" in lrom and Exists(Pcsx2Generator.BIOS_USA): return Pcsx2Generator.BIOS_USA

        # Check if PS2 bios bin in folders
        for bios in (Pcsx2Generator.BIOS_USA, Pcsx2Generator.BIOS_EU, Pcsx2Generator.BIOS_JP, Pcsx2Generator.BIOS_HK):
            if Exists(bios):
                return bios
        return ""

    def __ConfigureReg(self):
        reg = keyValueSettings(Pcsx2Generator.pcsx2ConfigFileReg)
        reg.loadFile(True)
        
        # Override Reg Configuration
        reg.setString("DocumentsFolderMode", "User")
        reg.setString("CustomDocumentsFolder", "/usr/bin/PCSX2")
        reg.setString("UseDefaultSettingsFolder", "enabled")
        reg.setString("SettingsFolder", "/recalbox/share/system/.config/PCSX2")
        reg.setString("Install_Dir", "/usr/bin/PCSX2")
        reg.setString("RunWizard", "0")

        # Save configuration
        reg.saveFile()

    def __ConfigureCore(self):
        import multiprocessing
        NbCpu = multiprocessing.cpu_count()

        # Override VM Configuration
        vm = IniSettings(Pcsx2Generator.pcsx2ConfigFilePCSX2_vm, True)
        vm.loadFile(True)

        vm.setString(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "FrameLimitEnable", "enabled")
        vm.setString(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "FramerateNTSC", "59.94")
        vm.setInt(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "FrameratePAL", 50)
        vm.setString(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "FrameSkipEnable", "disabled")
        vm.setInt(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "FramesToDraw", 2)
        vm.setInt(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "FramesToSkip", 2)
        vm.setString(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "LimitScalar", "1.00")
        vm.setString(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "SynchronousMTGS", "disabled")
        vm.setInt(Pcsx2Generator.SECTION_VM_EMUCORE_GS, "VsyncQueueSize", 2)
        vm.setString(Pcsx2Generator.SECTION_VM_EMUCORE_SPEEDHACKS, "vuThread", "enabled" if NbCpu > 3 else "disabled")

        # Save configuration
        vm.saveFile()
 
    @staticmethod
    def __ConfigureUI(system: Emulator, rom: str):
        # Game Ratio
        GAME_RATIO: Dict[str, str] = \
        {
            "16/9": "16:9",
            "4/3": "4:3",
        }

        gameRatio = GAME_RATIO[system.Ratio] if system.Ratio in GAME_RATIO else "4:3"

        # override UI Configuration
        ui = IniSettings(Pcsx2Generator.pcsx2ConfigFilePCSX2_ui, True)
        ui.defineBool("1", "0").loadFile(True)

        ui.setString(Pcsx2Generator.SECTION_UI_NONE, "AskOnBoot", "disabled")
        ui.setString(Pcsx2Generator.SECTION_UI_GSWINDOWS, "AspectRatio", gameRatio)
        ui.setString(Pcsx2Generator.SECTION_UI_POPUPDISABLERS, "Dialog.ComponentsConfig.Overrides", "disabled,ok")
        ui.setString(Pcsx2Generator.SECTION_UI_FILENAMES, "BIOS", Pcsx2Generator.__CheckPs2Bios(rom))

        # Save configuration
        ui.saveFile()

    @staticmethod
    def __ConfigureSound():
        # override SPU2 Configuration
        spu2 = IniSettings(Pcsx2Generator.pcsx2ConfigFileSPU2, True)
        spu2.defineBool("1", "0").loadFile(True)

        # Configura pulseaudio
        spu2.setString(Pcsx2Generator.SECTION_SPU2_OUTPUT, "Output_Module", "SDLAudio")
        spu2.setString(Pcsx2Generator.SECTION_SPU2_SDL, "HostApi", "pulseaudio")
        spu2.setBool(Pcsx2Generator.SECTION_SPU2_MIXING, "Interpolation", True)
        spu2.setBool(Pcsx2Generator.SECTION_SPU2_MIXING, "Disable_Effects", False)

        # Save configuration
        spu2.saveFile()

    @staticmethod
    def __ConfigureGraphicsPlugin(system: Emulator):
        import multiprocessing
        NbCpu = multiprocessing.cpu_count()

        # override GSdx Configuration
        gs = keyValueSettings(Pcsx2Generator.pcsx2ConfigFileGSdx, True)
        gs.defineBool("1", "0").loadFile(True)

        gs.setBool("vsync", True)
        gs.setBool("UserHacks", True)
        gs.setInt("extrathreads", NbCpu - 2)
        gs.setInt("extrathreads_height", NbCpu - 2)
        # overide user hack to default state
        gs.setBool("UserHacks_SkipDraw", False)
        gs.setBool("UserHacks_SkipDraw_Offset", False)

        gs.setString("osd_fontname", "/usr/share/fonts/dejavu/DejaVuSansMono.ttf")
        gs.setBool("osd_indicator_enabled", system.ShowFPS)
        gs.setBool("osd_monitor_enabled", system.ShowFPS)

        # Save configuration
        gs.saveFile()

    @staticmethod
    def __GetGSPluginPath() -> str:
        pathPlugins = "--gs=/usr/bin/PCSX2/plugins/"
        import os
        with open("/proc/cpuinfo") as f:
            for line in f.readlines():
                if line.startswith("flags"):
                    if "avx2" in line: return os.path.join(pathPlugins, "libGSdx-AVX2.so")
                    if "sse4" in line: return os.path.join(pathPlugins, "libGSdx-SSE4.so")
        return os.path.join(pathPlugins, "libGSdx.so")

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        if not system.HasConfigFile:
            # Controllers
            # pcsx2Controllers.generateControllerConfig(system, playersControllers, self)

            # Systems and other Configurations
            self.__ConfigureGraphicsPlugin(system)
            self.__ConfigureSound()
            self.__ConfigureUI(system, args.rom)
            self.__ConfigureCore()
            self.__ConfigureReg()
            self.__ConfigurePad(playersControllers)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "--fullscreen", "--fullboot",
                        #self.__GetGSPluginPath(),
                        args.rom]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
