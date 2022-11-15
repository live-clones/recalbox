#!/usr/bin/env python
import os.path
from typing import List

from configgen import recalboxFiles
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.settings.iniSettings import IniSettings


class ViceGenerator(Generator):

    VICE_SETTINGS_FILE_PATTERN = recalboxFiles.HOME + "/.config/vice/"
    VICE_JOYSTICK_MAPPING = recalboxFiles.HOME + "/.config/vice/joystick.vjm"

    # Main entry of the module
    # Return command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        # Figure out what emulator to run
        if system.Name == "vic20":
            coreToRun: str = "xvic"
            systemSection: str = "VIC20"
        else:
            coreToRun: str = system.Core
            # Try identify machine folder
            from typing import Dict
            machines: Dict[str, str] = \
            {
                "/commodore64/": "x64",
                "/commodore 64/": "x64",
                "/commodore-64/": "x64",
                "/c64sc/": "x64sc",
                "/commodore64sc/": "x64sc",
                "/commodore 64sc/": "x64sc",
                "/commodore-64sc/": "x64sc",
                "/scpu64/": "xscpu64",
                "/supercpu64/": "xscpu64",
                "/supercpu 64/": "xscpu64",
                "/supercpu-64/": "xscpu64",
                "/super-cpu-64/": "xscpu64",
                "/super cpu 64/": "xscpu64",
                "/commodoresupercpu64/": "xscpu64",
                "/commodore supercpu64/": "xscpu64",
                "/commodore-supercpu64/": "xscpu64",
                "/commodore super cpu64/": "xscpu64",
                "/commodore-super-cpu64/": "xscpu64",
                "/commodore super cpu 64/": "xscpu64",
                "/commodore-super-cpu-64/": "xscpu64",
                "/c64dtv/": "x64dtv",
                "/commodore64dtv/": "x64dtv",
                "/commodore 64dtv/": "x64dtv",
                "/commodore-64dtv/": "x64dtv",
                "/c128/": "x128",
                "/commodore128/": "x128",
                "/commodore 128/": "x128",
                "/commodore-128/": "x128",
                "/cplus4/": "xplus4",
                "/commodoreplus4/": "xplus4",
                "/commodore plus4/": "xplus4",
                "/commodore plus 4/": "xplus4",
                "/commodore-plus4/": "xplus4",
                "/commodore-plus-4/": "xplus4",
                "/vic20/": "xvic",
                "/commodorevic20/": "xvic",
                "/commodore vic20/": "xvic",
                "/commodore-vic20/": "xvic",
                "/commodore-vic-20/": "xvic",
                "/pet/": "xpet",
                "/commodorepet/": "xpet",
                "/commodore pet/": "xpet",
                "/commodore-pet/": "xpet",
                "/cbm2/": "xcbm2",
                "/cbm 2/": "xcbm2",
                "/cbm-2/": "xcbm2",
                "/cbm5x0/": "xcbm5x0",
                "/cbm 5x0/": "xcbm5x0",
                "/cbm-5x0/": "xcbm5x0",
            }
            romLowerCase: str = args.rom.lower()
            for key in machines:
                if key in romLowerCase:
                    coreToRun = machines[key]
                    break

            # Get section from core
            sections: Dict[str, str] = \
            {
                "x64": "C64",
                "x64sc": "C64SC",
                "x64dtv": "C64DTV",
                "xscpu64": "SCPU64",
                "c128": "c128",
                "xplus4": "PLUS4",
                "xvic": "VIC20",
                "xpet": "PET",
                "xcbm5x0": "CBM-II-5x0",
                "xcbm2": "CBM-II",
            }
            systemSection: str = sections[coreToRun]

        # Settings file
        settingsFile: str = self.VICE_SETTINGS_FILE_PATTERN + coreToRun + ".cfg"
        settings: IniSettings = IniSettings(settingsFile, False)
        settings.defineBool('1', '0') \
                .loadFile(True)

        # Hard coded
        settings.setBool(systemSection, "SaveResourcesOnExit", True)
        settings.setString(systemSection, "SoundDeviceName", '"pulse"')
        settings.setInt(systemSection, "JoyDevice1", 4)
        settings.setInt(systemSection, "JoyDevice2", 4)
        settings.setString(systemSection, "JoyMapFile", self.VICE_JOYSTICK_MAPPING)

        # Set graphics options
        self.SetGraphicOptions(coreToRun, systemSection, settings, system)
        # Set controllers
        self.SetJoystickFile(args.rom, playersControllers)

        # Save
        settings.saveFile()

        # Override
        overrideSettingsFile: str = self.VICE_SETTINGS_FILE_PATTERN + coreToRun + ".override.cfg"
        overrideSettings: IniSettings = IniSettings(overrideSettingsFile, False)
        overrideSettings.defineBool('1', '0') \
                        .loadFile(True)
        from configgen.settings.configOverriding import buildOverrideChain
        pathSettings: List[str] = buildOverrideChain(args.rom, ".vice.conf")
        # Override with path settings
        for pathSetting in pathSettings:
            overrideSettings.changeSettingsFile(pathSetting)
            overrideSettings.loadFile(False)
        # Save
        overrideSettings.changeSettingsFile(overrideSettingsFile)
        overrideSettings.saveFile()

        commandArray = ["/usr/bin/vice/" + coreToRun,
                        "-config", settings.getSettingsFile()]
        if not overrideSettings.isEmpty():
          commandArray.extend(["-addconfig", overrideSettings.getSettingsFile()])
        commandArray.extend(["-autostart", args.rom])

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode="default", array=commandArray)

    @staticmethod
    def SetGraphicOptions(coreToRun: str, section: str, settings: IniSettings, system: Emulator):

        prefixes: List[str] = []

        if coreToRun in ("x64", "xscpu64", "x64sc", "x64dtv", "x128", "xcmb5x0"):
            prefixes.append("VICII")
        if coreToRun == "xplus4":
            prefixes.append("TED")
        if coreToRun == "xvic":
            prefixes.append("VIC")
        if coreToRun == "x128":
            prefixes.append("VDC")
        if coreToRun in ("xpet", "xcmb2"):
            prefixes.append("Crtc")

        for prefix in prefixes:
            settings.setString(section, prefix + "FullscreenDevice", '"SDL"')
            settings.setInt(section, prefix + "SDLFullscreenMode", 0)
            settings.setInt(section, prefix + "Fullscreen", 1)
            settings.setBool(section, prefix + "FullscreenStatusbar", system.ShowFPS)
            settings.setBool(section, prefix + "Filter", system.Smooth)

        settings.setBool(section, "SDLGLFilter", system.Smooth)

    def SetJoystickFile(self, rom: str,playersControllers: ControllerPerPlayer):

        # Try rom specific config
        sourcevjm: str = rom + ".vjm"
        if not os.path.exists(sourcevjm):
            # Fallback to folder config
            sourcevjm: str = os.path.join(os.path.dirname(rom), ".vjm")
        # Joystick override exists?
        if os.path.exists(sourcevjm):
            with open(sourcevjm, 'r') as sourcefile:
                with open(self.VICE_JOYSTICK_MAPPING, 'w') as destinationfile:
                    destinationfile.writelines(sourcefile.readlines())
        else:
            # No override, build the configuration
            lines: List[str] = ["# Generated by Recalbox",
                                "!CLEAR"]

            c64joystick: int = 1
            for player, controller in playersControllers.items():
                # Force axis configuration as VICE does not allow tuned configurations
                lines.append("{} 0 0 1 {} 8".format(controller.SdlIndex, c64joystick))
                lines.append("{} 0 1 1 {} 4".format(controller.SdlIndex, c64joystick))
                lines.append("{} 0 2 1 {} 2".format(controller.SdlIndex, c64joystick))
                lines.append("{} 0 3 1 {} 1".format(controller.SdlIndex, c64joystick))
                # Force hat configuration as VICE does not allow tuned configurations
                lines.append("{} 2 0 1 {} 1".format(controller.SdlIndex, c64joystick))
                lines.append("{} 2 1 1 {} 2".format(controller.SdlIndex, c64joystick))
                lines.append("{} 2 2 1 {} 4".format(controller.SdlIndex, c64joystick))
                lines.append("{} 2 3 1 {} 8".format(controller.SdlIndex, c64joystick))
                # Configure buttons
                if controller.HasUp    and controller.Up.IsButton   : lines.append("{} 1 {} 1 {} 1".format(controller.SdlIndex, controller.Up.Id, c64joystick))
                if controller.HasDown  and controller.Down.IsButton : lines.append("{} 1 {} 1 {} 2".format(controller.SdlIndex, controller.Down.Id, c64joystick))
                if controller.HasLeft  and controller.Left.IsButton : lines.append("{} 1 {} 1 {} 4".format(controller.SdlIndex, controller.Left.Id, c64joystick))
                if controller.HasRight and controller.Right.IsButton: lines.append("{} 1 {} 1 {} 8".format(controller.SdlIndex, controller.Right.Id, c64joystick))
                if controller.HasB     and controller.B.IsButton    : lines.append("{} 1 {} 1 {} 16".format(controller.SdlIndex, controller.B.Id, c64joystick))
                if controller.HasA     and controller.A.IsButton    : lines.append("{} 1 {} 1 {} 32".format(controller.SdlIndex, controller.A.Id, c64joystick))
                if controller.HasX     and controller.X.IsButton    : lines.append("{} 1 {} 1 {} 64".format(controller.SdlIndex, controller.X.Id, c64joystick))
                # Player 1 specific controls
                if player == 1:
                    if controller.HasStart  and controller.Start.IsButton : lines.append("{} 1 {} 5 Virtual keyboard".format(controller.SdlIndex, controller.Start.Id))  # Start = Virtual keyboard
                    if controller.HasHotkey and controller.Hotkey.IsButton: lines.append("{} 1 {} 5 Quit emulator".format(controller.SdlIndex, controller.Hotkey.Id)) # HK = Quit emulator
                    if controller.HasY      and controller.Y.IsButton     : lines.append("{} 1 {} 2 7 7".format(controller.SdlIndex, controller.Y.Id, c64joystick))      # Y = Run/Stop
                    if controller.HasA      and controller.A.IsButton     : lines.append("{} 1 {} 2 0 1".format(controller.SdlIndex, controller.A.Id, c64joystick))      # A = enter
                    if controller.HasX      and controller.X.IsButton     : lines.append("{} 1 {} 2 7 4".format(controller.SdlIndex, controller.X.Id, c64joystick))      # X = space
                    if controller.HasL1     and controller.L1.IsButton    : lines.append("{} 1 {} 2 3 1".format(controller.SdlIndex, controller.L1.Id, c64joystick))     # L1 = Y
                    if controller.HasR1     and controller.R1.IsButton    : lines.append("{} 1 {} 2 4 7".format(controller.SdlIndex, controller.R1.Id, c64joystick))     # R1 = N

            os.makedirs(os.path.dirname(self.VICE_JOYSTICK_MAPPING), exist_ok=True)
            with open(self.VICE_JOYSTICK_MAPPING, 'w') as destinationfile:
                for line in lines:
                    destinationfile.write(line + '\n')