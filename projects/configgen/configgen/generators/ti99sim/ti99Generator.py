from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.Command import Command


class TI99Generator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:

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
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.Emulator],
                                   "--joystick1=1",
                                   "--dsk1=" + os.path.join(diskFolder, "savedisk1.dsk"),
                                   "--dsk2=" + os.path.join(diskFolder, "savedisk2.dsk"),
                                   "--dsk3=" + os.path.join(diskFolder, "savedisk3.dsk")]

        # Bilinear filtering
        if system.Smooth:
            commandArray.append("--bilinear")

        # HK+Start
        for controller in playersControllers.values():
            if controller.PlayerIndex == 1:
                if controller.HasHotkey: commandArray.append("--hotkey={}".format(controller.Hotkey.Id))
                if controller.HasStart : commandArray.append("--start={}".format(controller.Start.Id))

        # PAL/NTSC
        romPath: str = args.rom.lower()
        if "/pal/" in romPath:  commandArray.append("--PAL")
        if "/ntsc/" in romPath: commandArray.append("--NTSC")

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.extend([args.rom]) #, args.rom+".img"])

        return Command(videomode=system.VideoMode, array=commandArray)
