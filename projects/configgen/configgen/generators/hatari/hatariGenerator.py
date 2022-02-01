import os
from typing import List, Dict

from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
import configgen.recalboxFiles as recalboxFiles
from configgen.settings.iniSettings import IniSettings
import configgen.settings.configOverriding as configOverriding
from configgen.settings.keyValueSettings import keyValueSettings


class HatariGenerator(Generator):

    __ORIGIN_CONFIG_FILE = os.path.join(recalboxFiles.HOME, ".hatari/hatari.cfg.origin")
    __TARGET_CONFIG_FILE = os.path.join("/tmp", "hatari.cfg")

    __ROM_FOLDER = os.path.join(recalboxFiles.ROMS, "atarist")

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        # Load configuration
        configFile = IniSettings(self.__ORIGIN_CONFIG_FILE)
        configFile.defineBool("TRUE", "FALSE") \
                  .loadFile(True)

        # Set configuration
        self.SetMachineAndBios(configFile, args.rom)
        self.SetDirectories(configFile, args.rom)
        self.SetDisplayOptions(configFile, args.verbose)
        self.SetAutoQuit(configFile)
        self.SetJoystick(configFile, playersControllers)

        # Overrides
        overrides = configOverriding.buildOverrideChain(args.rom, ".hatari.cfg")
        for override in overrides:
            configFile.changeSettingsFile(override)
            configFile.loadFile(False)

        # Save configfile
        configFile.changeSettingsFile(self.__TARGET_CONFIG_FILE)
        configFile.saveFile()

        # Build parameters
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.Emulator],
                                   '--configfile', self.__TARGET_CONFIG_FILE]

        # Add extra arguments
        if system.HasArgs: commandArray.extend(system.Args)

        # finally add rom
        commandArray.extend([args.rom])

        return Command(videomode=system.VideoMode, array=commandArray)

    @staticmethod
    def SetJoystick(config: IniSettings, controllers: ControllerPerPlayer):
        for player, controller in controllers.items():
            if player == 1:  # Joystick 0 post is for mouse, so just start from 1 (= player 1)
                joystick: str = "Joystick{}".format(player)
                # Type
                config.setInt(joystick, "nJoyId", player - 1) # TODO: Replace with SdlIndex
                config.setInt(joystick, "nJoystickMode", 1) # Real joystick
                # Buttons
                if controller.HasHotkey: config.setInt(joystick, "nHotKey"   , controller.Hotkey.Id)
                if controller.HasStart : config.setInt(joystick, "nStartKey" , controller.Start.Id)
                if controller.HasA     : config.setInt(joystick, "nButton1"  , controller.A.Id)
                if controller.HasB     : config.setInt(joystick, "nButton2"  , controller.B.Id)
                if controller.HasX     : config.setInt(joystick, "nButton3"  , controller.X.Id)
                # Axis/Hats are automatically managed by the emulator. Add only buitton DPad
                if controller.HasUp    and controller.Up.IsButton   : config.setInt(joystick, "nDPadUp"   , controller.Up.Id)
                if controller.HasRight and controller.Right.IsButton: config.setInt(joystick, "nDPadRight", controller.Right.Id)
                if controller.HasDown  and controller.Down.IsButton : config.setInt(joystick, "nDPadDown" , controller.Down.Id)
                if controller.HasLeft  and controller.Left.IsButton : config.setInt(joystick, "nDPadLeft" , controller.Left.Id)

    @staticmethod
    def SetAutoQuit(config: IniSettings):
        config.setBool("Log", "bConfirmQuit", False)

    @staticmethod
    def SetDisplayOptions(config: IniSettings, verbose: bool):
        config.setBool("Screen", "bAllowOverscan", False)
        config.setBool("Screen", "bAspectCorrect", True)
        config.setBool("Screen", "bUseSdlRenderer", True)
        config.setBool("Screen", "bUseVsync", True)
        config.setBool("Screen", "bFullScreen", True)
        config.setBool("Screen", "bShowStatusbar", verbose)

    def SetDirectories(self, config: IniSettings, rom: str):
        config.setString("Floppy", "szDiskImageDirectory", self.__ROM_FOLDER)
        config.setString("HardDisk", "szHardDiskDirectory", self.__ROM_FOLDER)
        config.setString("Memory", "szAutoSaveFileName", os.path.join(recalboxFiles.SAVES, "atarist/AutoSave.sav"))
        config.setString("Memory", "szMemoryCaptureFileName", os.path.join(recalboxFiles.SAVES, "atarist", os.path.basename(rom) + '.sav'))
        config.setString("Printer", "szPrintToFileName", os.path.join(recalboxFiles.SAVES, "atarist", os.path.basename(rom) + '.prn'))
        config.setString("Sound", "szYMCaptureFileName", os.path.join(recalboxFiles.SAVES, "atarist", os.path.basename(rom) + '.wav'))
        config.setString("Video", "AviRecordFile", os.path.join(recalboxFiles.SAVES, "atarist", os.path.basename(rom) + '.avi'))
        for i in range(0, 8):
            config.setString("ACSI", "sDeviceFile{}".format(i), self.__ROM_FOLDER)
            config.setString("SCSI", "sDeviceFile{}".format(i), self.__ROM_FOLDER)
        for i in range(0, 2):
            config.setString("IDE", "sDeviceFile{}".format(i), self.__ROM_FOLDER)

    @staticmethod
    def SetMachineAndBios(config: IniSettings, rom: str):
        # Machine list
        MACHINE_ST = 0
        MACHINE_MEGA_ST = 1
        MACHINE_STE = 2
        MACHINE_MEGA_STE = 3
        MACHINE_TT = 4
        MACHINE_FALCON = 5

        # Try identify machine folder
        machines: Dict[str, int] = \
        {
            "atari-st": MACHINE_ST,
            "/st/": MACHINE_ST,
            "atarimegast": MACHINE_MEGA_ST,
            "atari-megast": MACHINE_MEGA_ST,
            "atari-mega-st": MACHINE_MEGA_ST,
            "megast": MACHINE_MEGA_ST,
            "mega-st": MACHINE_MEGA_ST,
            "atariste": MACHINE_STE,
            "atari-ste": MACHINE_STE,
            "/ste/": MACHINE_STE,
            "atarimegaste": MACHINE_MEGA_STE,
            "atari-megaste": MACHINE_MEGA_STE,
            "atari-mega-ste": MACHINE_MEGA_STE,
            "megaste": MACHINE_MEGA_STE,
            "mega-ste": MACHINE_MEGA_STE,
            "ataritt": MACHINE_TT,
            "atari-tt": MACHINE_TT,
            "/tt/": MACHINE_TT,
            "falcon": MACHINE_FALCON,
            "atari-falcon": MACHINE_FALCON,
        }
        defaultMachine: int = MACHINE_ST
        romLower: str = rom.lower()
        for key in machines:
            if key in romLower:
                defaultMachine: int = machines[key]
                break
        config.setInt("System", "nMachineType", defaultMachine)
        print("MACHINE: {}".format(defaultMachine))

        # Select bios
        subdir: str = "atarist"
        biosFile = os.path.join(recalboxFiles.BIOS, subdir, "st.img")
        # ST/MEGAST use both st.img
        if   defaultMachine == MACHINE_FALCON  : biosFile = os.path.join(recalboxFiles.BIOS, subdir, "falcon.img")
        elif defaultMachine == MACHINE_MEGA_STE: biosFile = os.path.join(recalboxFiles.BIOS, subdir, "megaste.img")
        elif defaultMachine == MACHINE_TT      : biosFile = os.path.join(recalboxFiles.BIOS, subdir, "tt.img")
        elif defaultMachine == MACHINE_STE     : biosFile = os.path.join(recalboxFiles.BIOS, subdir, "ste.img")
        if not os.path.exists(biosFile):
            biosFile = os.path.join(recalboxFiles.BIOS, subdir, "tos.img")
            if not os.path.exists(biosFile):
                biosFile = os.path.join(recalboxFiles.BIOS, "tos.img")
                if not os.path.exists(biosFile):
                    biosFile = os.path.join(recalboxFiles.BIOS_INIT, subdir, "emutos.img")
        if "emutos" in rom:
            biosFile = os.path.join(recalboxFiles.BIOS_INIT, subdir, "emutos.img")
        config.setString("ROM", "szTosImageFileName", str(biosFile))
        print("BIOS   : {}".format(biosFile))
