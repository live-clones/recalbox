#!/usr/bin/env python
import pytest
import shutil
from configgen.Emulator import Emulator
import configgen.generators.ppsspp.ppssppControllers as ppssppControllers
import configgen.generators.ppsspp.ppssppConfig as ppssppConfig
import configgen.generators.ppsspp.ppssppGenerator as ppssppGenerator
from configgen.generators.ppsspp.ppssppGenerator import PPSSPPGenerator
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.settings.unixSettings import UnixSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    ppssppGenerator.recalboxFiles.ppssppControls = 'tests/tmp/ppsspp-gamecontrollerdb.cfg'
    ppssppConfig.recalboxFiles.ppssppConfig = 'tests/tmp/ppsspp.ini'
    ppssppConfig.ppssppSettings = UnixSettings('tests/tmp/ppsspp.ini', separator=' ')
    ppssppControllers.recalboxFiles.ppssppControlsInit = 'tests/resources/ppsspp-controls.ini'
    ppssppControllers.recalboxFiles.ppssppControlsIni = 'tests/tmp/ppsspp-controls.ini'
    shutil.copyfile('tests/resources/ppsspp-controls.ini', 'tests/tmp/ppsspp-controls.ini')
    return PPSSPPGenerator()


@pytest.fixture
def system_gliden64():
    return Emulator(name='psp', videoMode='1920x1080', ratio='auto', emulator='ppsspp', core='ppsspp')


@pytest.fixture
def controller_configuration():
    uuid = "060000004c0500006802000000010000"
    return controllersConfig.Controller.loadControllerConfig("0", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*")


def test_simple_generate_ppsspp(emulator, system_gliden64, mocker, controller_configuration):
    command = emulator.generate(system_gliden64, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/PPSSPPSDL', 'path/to/test',
                             ]
