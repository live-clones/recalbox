from configgen.generators.Generator import Generator


class TI99Generator(Generator):

    IS_TRUE = ("1", "true")

    def isEnabled(self, system, key):
        recalbox = system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    def generate(self, system, playersControllers, recalboxSettings, args):

        # Make save dir
        import os
        import configgen.recalboxFiles as recalboxFiles
        snapshotFolder: str = os.path.join(recalboxFiles.SAVES, "ti994a", "snapshots")
        diskFolder: str = os.path.join(recalboxFiles.SAVES, "ti994a", "disks")
        if not os.path.exists(snapshotFolder): os.makedirs(name=snapshotFolder, exist_ok=True)
        if not os.path.exists(diskFolder): os.makedirs(name=diskFolder, exist_ok=True)

        # build disks
        for i in range(1, 4):
            diskFile = os.path.join(diskFolder, "savedisk{}.dsk".format(i))
            if not os.path.exists(diskFile):
                os.system("/usr/bin/ti99sim/disk --create=SSDD {}".format(diskFile))

        # Build options array
        from typing import List
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.config['emulator']],
                                   "--joystick1=1",
                                   "--dsk1=" + os.path.join(diskFolder, "savedisk1.dsk"),
                                   "--dsk2=" + os.path.join(diskFolder, "savedisk2.dsk"),
                                   "--dsk3=" + os.path.join(diskFolder, "savedisk3.dsk")]

        # Bilinear filtering
        if self.isEnabled(system, "smooth"):
            commandArray.append("--bilinear")

        # HK+Start
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                if "hotkey" in controller.inputs: commandArray.append("--hotkey={}".format(controller.inputs["hotkey"].id))
                if "start"  in controller.inputs: commandArray.append("--start={}".format(controller.inputs["start"].id))

        # PAL/NTSC
        romPath: str = args.rom.lower()
        if "/pal/" in romPath:
            commandArray.append("--PAL")
        if "/ntsc/" in romPath:
            commandArray.append("--NTSC")

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        commandArray.extend([args.rom]) #, args.rom+".img"])

        import configgen.Command as Command
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
