#!/usr/bin/env python
import pytest
import shutil
import os
from configgen.Emulator import Emulator
import configgen.generators.moonlight.moonlightGenerator as moonlightGenerator
from configgen.generators.moonlight.moonlightGenerator import MoonlightGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    moonlightGenerator.recalboxFiles.moonlightCustom = 'tests/tmp/moonlight'
    moonlightGenerator.recalboxFiles.moonlightConfig = 'tests/tmp/moonlight/moonlight.conf'
    moonlightGenerator.recalboxFiles.moonlightGamelist = 'tests/tmp/moonlight/gamelist.txt'
    os.makedirs(moonlightGenerator.recalboxFiles.moonlightCustom, exist_ok=True)
    shutil.copyfile('tests/resources/moonlight/gamelist.txt', 'tests/tmp/moonlight/gamelist.txt')
    return MoonlightGenerator()


@pytest.fixture
def system():
    return Emulator(name='moonlight', videoMode='1920x1080', ratio='auto', emulator='moonlight', core='moonlight')


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


def test_simple_generate_moonlight(emulator, system, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('somegame_'))
    assert command.array == ['/usr/bin/moonlight', 'stream',
                             '-config', 'tests/tmp/moonlight/moonlight.conf',
                             '-app', 'SOMEGAME']
