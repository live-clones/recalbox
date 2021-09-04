from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.Command import Command
from configgen.settings.keyValueSettings import keyValueSettings


class XroarGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        # Make save dir
        import os
        import configgen.recalboxFiles as recalboxFiles
        snapshotFolder: str = os.path.join(recalboxFiles.SAVES, args.system, "snapshots")
        printerFolder: str = os.path.join(recalboxFiles.SAVES, args.system, "printer")
        if not os.path.exists(snapshotFolder): os.makedirs(name=snapshotFolder, exist_ok=True)
        if not os.path.exists(printerFolder): os.makedirs(name=printerFolder, exist_ok=True)

        # Global parameters
        from typing import List
        snapFile: str = os.path.basename(args.rom) + ".sna"
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.Emulator],
                                   "-vo", "sdl", "-ui", "sdl", "-ao", "pulse",
                                   "-rompath", os.path.join(recalboxFiles.BIOS, args.system),
                                   "-snappath", os.path.join(snapshotFolder, snapFile)]

        # Config file?
        from configgen.settings.configOverriding import buildOverrideChain
        configList = buildOverrideChain(args.rom, ".xroar.config")
        if len(configList) > 0:
            commandArray.extend(["-c", configList[len(configList) - 1]])
        else:
            # Default machine?
            defaultMachine: str = "coco"
            if args.system == "dragon": defaultMachine = "dragon64"
            if args.system == "trs80coco": defaultMachine = "coco2b"

            # Try identify machine folder
            from typing import Dict
            machines: Dict[str, str] = \
            {
                "/dragon/": "dragon64",
                "/dragon32/": "dragon32",
                "/dragon64/": "dragon64",
                "/tano/": "tano",
                "/dragon200/": "dragon200e",
                "/dragon200e/": "dragon200e",
                "/coco/": "coco",
                "/coco-pal/": "coco",
                "/cocous/": "cocous",
                "/coco-ntsc/": "cocous",
                "/coco2/": "coco2b",
                "/coco2b/": "coco2b",
                "/coco2-pal/": "coco2b",
                "/coco2b-pal/": "coco2b",
                "/coco2us/": "coco2bus",
                "/coco2bus/": "coco2bus",
                "/coco2-ntsc/": "coco2bus",
                "/coco2b-ntsc/": "coco2bus",
                "/mx/": "mx1600",
                "/mx1600/": "mx1600",
            }
            for key in machines:
                if key in args.rom:
                    defaultMachine = machines[key]
                    break
            commandArray.extend(["-default-machine", defaultMachine])

        # Printer output
        printerFile = os.path.basename(args.rom) + ".txt"
        commandArray.extend(["-lp-file", os.path.join(printerFolder, printerFile)])

        # Bilinear filtering
        if system.Smooth:
            commandArray.extend(["-gl-filter", "linear"])

        # HK+Start
        for controller in playersControllers.values():
            if controller.PlayerIndex == 1:
                if controller.HasHotkey: commandArray.extend(["-joy-start", str(controller.Hotkey.Id)])
                if controller.HasStart : commandArray.extend(["-joy-hotkey", str(controller.Start.Id)])

        # Add extra arguments
        if system.HasArgs: commandArray.extend(system.Args)

        # finally add rom
        commandArray.extend([args.rom])

        return Command(videomode=system.VideoMode, array=commandArray)
