from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class OricutronGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        settings = keyValueSettings(recalboxFiles.oricutronConfig)
        settings.loadFile(True)\
                .defineBool('yes', 'no')

        # Set default joystick interface to "ijk" if not defined
        joyInterface = settings.getString("joyinterface", "ijk")
        settings.setString("joyinterface", joyInterface)

        # default joystick are keyboard 1 & 2
        settings.setString("joystick_a", "kbjoy1") \
                .setString("joystick_b", "none") \
                .setString("telejoy_a", "kbjoy1") \
                .setString("telejoy_b", "none")

        # controller settings
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.PlayerIndex == 1:
                settings.setString("joystick_a", "sdlnamedjoy") \
                        .setString("joystick_name_a", controller.DeviceName)
                if controller.HasHotkey: settings.setInt("sdljoy1_hkey",  controller.Hotkey.Id)
                if controller.HasStart : settings.setInt("sdljoy1_start", controller.Start.Id)
                if controller.HasA     : settings.setInt("sdljoy1_fire1", controller.A.Id)
                if controller.HasB     : settings.setInt("sdljoy1_fire2", controller.B.Id)
            if controller.PlayerIndex == 2:
                settings.setString("joystick_b", "sdlnamedjoy") \
                        .setString("joystick_name_b", controller.DeviceName)
                if controller.HasHotkey: settings.setInt("sdljoy2_hkey",  controller.Hotkey.Id)
                if controller.HasStart : settings.setInt("sdljoy2_start", controller.Start.Id)
                if controller.HasA     : settings.setInt("sdljoy2_fire1", controller.A.Id)
                if controller.HasB     : settings.setInt("sdljoy2_fire2", controller.B.Id)

        # force values
        settings.setString("fullscreen", "yes") \
                .setString("rendermode", "soft")

        # default values
        settings.setDefaultString("machine", "atmos") \
                .setDefaultBool  ("lightpen", True) \
                .setDefaultInt   ("rampattern", 1) \
                .setDefaultString("serial", "modem") \
                .setDefaultString("serial_address", "$31c") \
                .setDefaultBool  ("aratio", True)

        settings.setBool("scanlines", False)
        if system.ShaderSet == 'scanlines':
            settings.setString("scanlines", "yes")

        # Erase disk/tape
        settings.setString("diskimage", "") \
                .setString("tapeimage", "")
        # Select disk drive
        drive = settings.getString("disktype", "microdisc")  # microdisc is the original Oric drive.
        if drive == "none":
            drive = "microdisc"  # force drive
            settings.setString("disktype", drive)

        settings.saveFile()

        # tape/disk selection
        romType = args.rom[-4:].lower()
        arguments = ["--tape" if romType == ".tap" else "--disk", args.rom]

        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "--turbotape", "on",  # One of the only options not available in config file
                        "--vsynchack", "on"  # This one too
                        ]
        commandArray.extend(arguments)
        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
