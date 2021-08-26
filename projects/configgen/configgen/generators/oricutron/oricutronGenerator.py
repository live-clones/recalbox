import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.settings.keyValueSettings import keyValueSettings


class OricutronGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        settings = keyValueSettings(recalboxFiles.oricutronConfig)
        settings.loadFile(True)

        # Set default joystick interface to "ijk" if not defined
        joyInterface = settings.getOption("joyinterface", "ijk")
        settings.setOption("joyinterface", joyInterface)

        # default joystick are keyboard 1 & 2
        settings.setOption("joystick_a", "kbjoy1")
        settings.setOption("joystick_b", "none")
        settings.setOption("telejoy_a", "kbjoy1")
        settings.setOption("telejoy_b", "none")

        # controller settings
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                settings.setOption("joystick_a", "sdlnamedjoy")
                settings.setOption("joystick_name_a", controller.configName)
                settings.setOption("sdljoy1_hkey", controller.inputs["hotkey"].id)
                settings.setOption("sdljoy1_start", controller.inputs["start"].id)
                settings.setOption("sdljoy1_fire1", controller.inputs["a"].id)
                settings.setOption("sdljoy1_fire2", controller.inputs["b"].id)
            if controller.player == "2":
                settings.setOption("joystick_b", "sdlnamedjoy")
                settings.setOption("joystick_name_b", controller.configName)
                settings.setOption("sdljoy2_hkey", controller.inputs["hotkey"].id)
                settings.setOption("sdljoy2_start", controller.inputs["start"].id)
                settings.setOption("sdljoy2_fire1", controller.inputs["a"].id)
                settings.setOption("sdljoy2_fire2", controller.inputs["b"].id)

        # force values
        # cpu = subprocess.check_output(['uname', '-m']).lower()
        # settings.setOption("fullscreen", "yes" if "x86" in cpu else "yes")
        settings.setOption("fullscreen", "yes")
        settings.setOption("rendermode", "soft")

        # default values
        settings.setDefaultOption("machine", "atmos")
        settings.setDefaultOption("lightpen", "yes")
        settings.setDefaultOption("rampattern", "1")
        settings.setDefaultOption("serial", "modem")
        settings.setDefaultOption("serial_address", "$31c")
        settings.setDefaultOption("aratio", "yes")

        settings.setOption("scanlines", "no")
        if system.ShaderSet == 'scanlines':
            settings.setOption("scanlines", "yes")

        # Erase disk/tape
        settings.setOption("diskimage", "")
        settings.setOption("tapeimage", "")
        # Select disk drive
        drive = settings.getOption("disktype", "microdisc")  # microdisc is the original Oric drive.
        if drive == "none":
            drive = "microdisc"  # force drive
            settings.setOption("disktype", drive)

        settings.saveFile()

        # tape/disk selection
        romType = args.rom[-4:].lower()
        if romType == ".tap":
            arguments = ["--tape", args.rom]
        else:
            arguments = ["--disk", args.rom]

        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "--turbotape", "on",  # One of the only options not available in config file
                        "--vsynchack", "on"  # This one too
                        ]
        commandArray.extend(arguments)

        if system.HasArgs: commandArray.extend(system.Args)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
