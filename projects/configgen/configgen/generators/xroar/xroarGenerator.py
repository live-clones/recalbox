from configgen.generators.Generator import Generator


class XroarGenerator(Generator):

    IS_TRUE = ("1", "true")

    def isEnabled(self, system, key):
        recalbox = system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    def generate(self, system, playersControllers, recalboxSettings, args):

        # Make save dir
        import os
        import configgen.recalboxFiles as recalboxFiles
        snapshotFolder: str = os.path.join(recalboxFiles.SAVES, args.system, "snapshots")
        printerFolder: str = os.path.join(recalboxFiles.SAVES, args.system, "printer")
        if not os.path.exists(snapshotFolder): os.makedirs(name=snapshotFolder, exist_ok=True)
        if not os.path.exists(printerFolder): os.makedirs(name=printerFolder, exist_ok=True)

        # Global parameters
        from typing import List
        import configgen.recalboxFiles as recalboxFiles
        snapFile = os.path.basename(args.rom) + ".sna"
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.config['emulator']],
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
            defaultMachine = "coco"
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
        if self.isEnabled(system, "smooth"):
            commandArray.extend(["-gl-filter", "linear"])

        # HK+Start
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                if "hotkey" in controller.inputs: commandArray.extend(["-joy-start", controller.inputs["hotkey"].id])
                if "start"  in controller.inputs: commandArray.extend(["-joy-hotkey", controller.inputs["start"].id])

        # Add extra arguments
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        # finally add rom
        commandArray.extend([args.rom])

        import configgen.Command as Command
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
