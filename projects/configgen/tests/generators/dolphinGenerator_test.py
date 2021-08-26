#!/usr/bin/env python
import pytest
import shutil
from configgen.Emulator import Emulator
import configgen.generators.dolphin.dolphinControllers as dolphinControllers
import configgen.generators.dolphin.dolphinGenerator as dolphinGenerator
from configgen.generators.dolphin.dolphinGenerator import DolphinGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    dolphinGenerator.recalboxFiles.dolphinIni = 'tests/tmp/dolphin.ini'
    dolphinGenerator.recalboxFiles.dolphinSYSCONF = 'tests/tmp/dolphin.SYSCONF'
    dolphinGenerator.recalboxFiles.dolphinGFX = 'tests/tmp/dolphin-GFX.ini'
    dolphinControllers.recalboxFiles.dolphinHKeys = 'tests/tmp/dolphin-Hotkeys.ini'
    dolphinControllers.recalboxFiles.dolphinConfig = 'tests/tmp'
    shutil.copyfile("tests/resources/dolphin.SYSCONF", "tests/tmp/dolphin.SYSCONF")
    return DolphinGenerator()


@pytest.fixture
def system_wii():
    return Emulator(name='wii', videoMode='1920x1080', ratio='auto', emulator='dolphin', core='dolphin')


@pytest.fixture
def system_gamecube():
    return Emulator(name='gamecube', videoMode='1920x1080', ratio='auto', emulator='dolphin', core='dolphin')


@pytest.fixture
def controller_configuration():
    uuid = "060000004c0500006802000000010000"
    return controllersConfig.Controller.loadControllerConfig("-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*")


def test_simple_generate_wii(emulator, system_wii, controller_configuration, mocker):
    command = emulator.generate(system_wii, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/dolphin-emu', '-e', 'path/to/test']


def test_simple_generate_gamecube(emulator, system_gamecube, controller_configuration, mocker):
    command = emulator.generate(system_gamecube, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/dolphin-emu', '-e', 'path/to/test']
