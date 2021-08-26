#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
from configgen.generators.scummvm.scummvmGenerator import ScummVMGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    return ScummVMGenerator()


@pytest.fixture
def system():
    return Emulator(name='scummvm', videoMode='1920x1080', ratio='auto', emulator='scummvm', core='scummvm')


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


def test_simple_generate_scummvm(emulator, system, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == 'default'
    assert command.array == ['/usr/bin/scummvm', '--fullscreen',
                             '--subtitles', '--joystick=0',
                             '--filtering',
                             '--extrapath=/usr/share/scummvm',
                             '--savepath=/recalbox/share/saves/scummvm',
                             '--path=path/to', 'test']


def test_simple_generate_scummvm_romdir(emulator, system, mocker, controller_configuration):
    mocker.patch('os.path.isdir', return_value=True)
    mocker.patch('glob.glob', return_value=['path/to/test/thegame.scummvm'])
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == 'default'
    assert command.array == ['/usr/bin/scummvm', '--fullscreen',
                             '--subtitles', '--joystick=0',
                             '--filtering',
                             '--extrapath=/usr/share/scummvm',
                             '--savepath=/recalbox/share/saves/scummvm',
                             '--path=path/to/test', 'thegame']
