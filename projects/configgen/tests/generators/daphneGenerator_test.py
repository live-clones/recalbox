#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.daphne.daphneControllers as daphneGenerator
from configgen.generators.daphne.daphneGenerator import DaphneGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig
from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator(mocker, fake_process):
    daphneGenerator.recalboxFiles.daphneInputIni = 'tests/tmp/dapinput.ini'
    return DaphneGenerator()


@pytest.fixture
def system():
    return Emulator(name='daphne', videoMode='16', ratio='auto', emulator='daphne', core='daphne')


@pytest.fixture
def system_xy():
    return Emulator(name='daphne', videoMode='1920x1080', ratio='auto', emulator='daphne', core='daphne')


def test_simple_generate_singe_file(emulator, system, fake_process, mocker):
    mocker.patch("os.path.exists", return_value=True)
    command = emulator.generate(system, dict(), keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/hypseus', 'singe', 'vldp',
                             '-framefile', 'path/to/test/test.txt',
                             '-fullscreen',
                             '-script', 'path/to/test/test.singe',
                             '-blend_sprites',
                             '-datadir', '/usr/share/daphne',
                             '-homedir', '/recalbox/share/roms/daphne'
                             ]


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


def test_simple_generate_other_rom(emulator, system, fake_process, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/hypseus', 'test', 'vldp',
                             '-framefile', 'path/to/test/test.txt',
                             '-fullscreen',
                             '-useoverlaysb', '2',
                             '-datadir', '/usr/share/daphne',
                             '-homedir', '/recalbox/share/roms/daphne'
                             ]


def test_simple_generate_rpi_resolution(emulator, system_xy, fake_process, mocker):
    mocker.patch("configgen.utils.architecture.Architecture", return_value="rpi3")
    command = emulator.generate(system_xy, dict(), keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/hypseus', 'test', 'vldp',
                             '-framefile', 'path/to/test/test.txt',
                             '-fullscreen',
                             '-useoverlaysb', '2',
                             '-datadir', '/usr/share/daphne',
                             '-homedir', '/recalbox/share/roms/daphne',
                             '-x', '1920',
                             '-y', '1080'
                             ]
