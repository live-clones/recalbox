#!/usr/bin/env python
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.utils.resolutions import ResolutionParser


class KodiResolution(Generator):
    def transform(self, system: Emulator) -> str:
        resolution = ResolutionParser(system.VideoMode)
        if not resolution.isSet:
            return ""
        return "{}{}060.00000{}std".format("{}".format(resolution.width).rjust(5, "0"),
                                            "{}".format(resolution.height).rjust(5, "0"),
                                            "p" if resolution.progressive else "i")
