#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.daphne.daphneControllers as daphneGenerator
from configgen.generators.daphne.daphneGenerator import DaphneGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllers.controller as controllersConfig
from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator(fake_process):
    daphneGenerator.recalboxFiles.daphneInputIni = 'tests/tmp/dapinput.ini'
    return DaphneGenerator()


@pytest.fixture
def system():
    return Emulator(name='daphne', videoMode='16', ratio='auto', emulator='daphne', core='daphne')


@pytest.fixture
def system_xy():
    return Emulator(name='daphne', videoMode='1920x1080', ratio='auto', emulator='daphne', core='daphne')


def test_simple_generate_singe_file(emulator, system, mocker):
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
    return controllersConfig.Controller.LoadUserControllerConfigurations(
        p1index =-1, p1guid =uuid, p1name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p1devicepath ="", p1nbaxes = -1, p1nbhats = -1, p1nbbuttons = -1,
        p2index =-1, p2guid =uuid, p2name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p2devicepath ="", p2nbaxes = -1, p2nbhats = -1, p2nbbuttons = -1,
        p3index =-1, p3guid =uuid, p3name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p3devicepath ="", p3nbaxes = -1, p3nbhats = -1, p3nbbuttons = -1,
        p4index =-1, p4guid =uuid, p4name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p4devicepath ="", p4nbaxes = -1, p4nbhats = -1, p4nbbuttons = -1,
        p5index =-1, p5guid =uuid, p5name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p5devicepath ="", p5nbaxes = -1, p5nbhats = -1, p5nbbuttons = -1,
        p6index =-1, p6guid =uuid, p6name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p6devicepath ="", p6nbaxes = -1, p6nbhats = -1, p6nbbuttons = -1,
        p7index =-1, p7guid =uuid, p7name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p7devicepath ="", p7nbaxes = -1, p7nbhats = -1, p7nbbuttons = -1,
        p8index =-1, p8guid =uuid, p8name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p8devicepath ="", p8nbaxes = -1, p8nbhats = -1, p8nbbuttons = -1,
        p9index =-1, p9guid =uuid, p9name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p9devicepath ="", p9nbaxes = -1, p9nbhats = -1, p9nbbuttons = -1,
        p10index=-1, p10guid=uuid, p10name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p10devicepath="", p10nbaxes= -1, p10nbhats= -1, p80nbbuttons= -1
    )


def test_simple_generate_other_rom(emulator, system, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/hypseus', 'test', 'vldp',
                             '-framefile', 'path/to/test/test.txt',
                             '-fullscreen',
                             '-useoverlaysb', '2',
                             '-datadir', '/usr/share/daphne',
                             '-homedir', '/recalbox/share/roms/daphne'
                             ]


def test_simple_generate_rpi_resolution(emulator, system_xy, mocker):
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
