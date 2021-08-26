# Exists to allow direct overriding of Emulator properties
# for test purpose only
from __future__ import annotations

from configgen.Emulator import Emulator


class MutableEmulator(Emulator):

    def __init__(self, name: str, emulator: str, core: str, videoMode: str = 'CEA 4 HDMI', ratio: str = 'auto'):
        super().__init__(name, emulator, core, videoMode, ratio)

    def SetShaderSet(self, shaderSet: str) -> MutableEmulator:
        self._shaderSet = shaderSet
        return self

    def SetSmooth(self, smooth: bool) -> MutableEmulator:
        self._smooth = smooth
        return self