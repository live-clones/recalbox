#!/usr/bin/env python
import pytest
import os
from configgen.Emulator import Emulator
import configgen.generators.reicast.reicastGenerator as reicastGenerator
import configgen.generators.reicast.reicastControllers as reicastControllers
from configgen.generators.reicast.reicastGenerator import ReicastGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    reicastGenerator.recalboxFiles.reicastConfig = 'tests/tmp/reicast/emu.cfg'
    reicastGenerator.recalboxFiles.reicastConfigInit = 'tests/resources/reicast/emu.cfg'
    reicastControllers.recalboxFiles.reicastCustom = 'tests/tmp/reicast'
    reicastControllers.recalboxFiles.reicastConfig = 'tests/tmp/reicast/emu.cfg'
    os.makedirs('tests/tmp/reicast', exist_ok=True)
    return ReicastGenerator()


@pytest.fixture
def system():
    return Emulator(name='dreamcast', videoMode='1920x1080', ratio='auto', emulator='reicast', core='reicast')


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


def test_simple_generate_reicast(emulator, system, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/reicast.elf', 'path/to/test']
