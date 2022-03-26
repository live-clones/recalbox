#!/usr/bin/env python
from pathlib import Path

import pytest

import configgen.emulatorlauncher as emulatorlauncher
from configgen.Emulator import Emulator
from configgen.generators.libretro import libretroConfigurations
from configgen.settings.keyValueSettings import keyValueSettings


class FakeArguments:
    def __init__(self):
        self.p1index = 0
        self.p1guid = "030000001f08000001e4000010010000"
        self.p1name = "USB gamepad"
        self.p1nbaxes = 2
        self.p1nbhats = 0
        self.p1nbbuttons = 10
        self.p1devicepath = "/dev/input/event7"
        self.p2index = self.p2guid = self.p2name = self.p2devicepath = self.p2nbaxes = self.p2nbhats = self.p2nbbuttons = self.p3index = self.p3guid = self.p3name = self.p3devicepath = self.p3nbaxes = self.p3nbhats = self.p3nbbuttons = self.p4index = self.p4guid = self.p4name = self.p4devicepath = self.p4nbaxes = self.p4nbhats = self.p4nbbuttons = self.p5index = self.p5guid = self.p5name = self.p5devicepath = self.p5nbaxes = self.p5nbhats = self.p5nbbuttons = self.p6index = self.p6guid = self.p6name = self.p6devicepath = self.p6nbaxes = self.p6nbhats = self.p6nbbuttons = self.p7index = self.p7guid = self.p7name = self.p7devicepath = self.p7nbaxes = self.p7nbhats = self.p7nbbuttons = self.p8index = self.p8guid = self.p8name = self.p8devicepath = self.p8nbaxes = self.p8nbhats = self.p8nbbuttons = self.p9index = self.p9guid = self.p9name = self.p9devicepath = self.p9nbaxes = self.p9nbhats = self.p9nbbuttons = self.p10index = self.p10guid = self.p10name = self.p10devicepath = self.p10nbaxes = self.p10nbhats = self.p10nbbuttons = False
        self.system = "atari2600"
        self.rom = "path/to/test.zip"
        self.emulator = "libretro"
        self.core = "stella"
        self.ratio = "auto"
        self.demo = False
        self.netplay = self.netplay_vieweronly = False
        self.netplay_ip = ""
        self.netplay_port = 0
        self.netplay_playerpassword = self.netplay_viewerpassword = ""
        self.hash = self.extra = self.nodefaultkeymap = self.verbose = False

        self.crtvideostandard = ""
        self.crtregion = ""
        self.crtresolutiontype = ""
        self.crtscreentype = ""
        self.crtadaptor = ""
        self.crtverticaloffset = "0"
        self.crthorizontaloffset = "0"
        self.crtviewportwidth = "1840"
        self.crtscanlines = False


@pytest.mark.parametrize(
    'emulator',
    [
        "libretro", "mupen64plus", "reicast",   "dolphin",      "ppsspp",
        "amiberry", "daphne",      "scummvm",   "dosbox",       "advancemame",
        "simcoupe", "gsplus",      "oricutron", "linapple",     "kodi",
        "fba2x",    "moonlight",   "vice",      "pcsx_rearmed", "openbor",
        "solarus",  "pisnes",      "supermodel"
    ],
)
def test_getGenerator(emulator):
    generator = emulatorlauncher.getGenerator(emulator)
    assert isinstance(generator, object)


@pytest.mark.parametrize(
    "targetsystem,emulator",
    [
        ("3ds", {'emulator': "libretro", 'core': "citra_canary"}),
        ("64dd", {'emulator': "libretro", 'core': "parallel_n64"}),
    ],
)
def test_getDefaultEmulator(targetsystem, emulator):
    default_emulator: Emulator = emulatorlauncher.getDefaultEmulator(targetsystem, emulator['emulator'], emulator['core'])
    assert default_emulator.Core == emulator['core']
    assert default_emulator.Emulator == emulator['emulator']


def test_main(mocker):
    mocker.patch('configgen.emulatorlauncher.getGenerator')
    emulatorlauncher.recalboxFiles.savesDir = 'tests/tmp/savedir'
    arguments = FakeArguments()
    rc, wait = emulatorlauncher.main(arguments)
    assert rc < 0


def test_select_crt_emulator_when_crt_enabled(mocker):
    getGenerator = mocker.patch('configgen.emulatorlauncher.getGenerator')
    recalboxConf = keyValueSettings("")
    recalboxConf.setString("system.crt", "vga666")
    mocker.patch('configgen.emulatorlauncher.loadRecalboxSettings', return_value=(recalboxConf, False))

    arguments = FakeArguments()
    arguments.crtadaptor = "recalboxrgbdual"
    arguments.system = "snes"
    arguments.emulator = "pisnes"
    arguments.core = "anycore"
    emulatorlauncher.main(arguments)
    getGenerator.assert_called_with("libretro")
