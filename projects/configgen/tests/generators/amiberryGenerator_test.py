#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.amiberry.amiberryKickstarts as amiberryKickstarts
from configgen.generators.amiberry.amiberryGenerator import AmiberryGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllers.controller as controllersConfig
from tests.generators.FakeArguments import Arguments

amiberryKickstarts.recalboxFiles.BIOS = 'tests/resources/bios'


@pytest.fixture
def emulator(mocker, fake_process):
    fake_process.register_subprocess(
        ["unzip", "-l", "path/to/test.zip"], stdout=["zip result, test.slave"]
    )
    fake_process.register_subprocess(
        ["mkdir", "-p", "/recalbox/share/saves/amiga1200/whdl"]
    )
    fake_process.register_subprocess(
        ["cp", "-r", "/usr/share/amiberry/whdboot", "/tmp/amiga"]
    )
    fake_process.register_subprocess(
        ["ln", "-s", "/recalbox/share/saves/amiga1200/whdl", "/tmp/amiga/whdboot/save-data/Savegames"]
    )
    mocker.patch('os.removedirs')
    mocker.patch('os.symlink')
    return AmiberryGenerator()


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


@pytest.fixture(params=[
    pytest.lazy_fixture('system_a600'),
    pytest.lazy_fixture('system_a1200'),
    pytest.lazy_fixture('system_cd32'),
])
def system_hddiso(request):
    return request.param


@pytest.fixture(params=[
    pytest.lazy_fixture('system_a600'),
    pytest.lazy_fixture('system_a1200'),
])
def system_floppy(request):
    return request.param


@pytest.fixture
def system_a600():
    return Emulator(name='amiga600', videoMode='16', ratio='auto', emulator='amiberry', core='amiberry')


@pytest.fixture
def system_a1200():
    return Emulator(name='amiga1200', videoMode='16', ratio='auto', emulator='amiberry', core='amiberry')


@pytest.fixture
def system_cd32():
    return Emulator(name='amigacd32', videoMode='16', ratio='auto', emulator='amiberry', core='amiberry')


def test_simple_generate_rom_iso(emulator, system_hddiso, controller_configuration):
    command = emulator.generate(system_hddiso, controller_configuration, keyValueSettings("", False), Arguments('path/to/test.iso'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/amiberry', '-config', '/tmp/amiga/conf/default.uae']


def test_simple_generate_rom_zip(emulator, system_hddiso, controller_configuration):
    command = emulator.generate(system_hddiso, controller_configuration, keyValueSettings("", False), Arguments('path/to/test.zip'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/amiberry', '-autoload', 'path/to/test.zip']


def test_simple_generate_rom_adf(emulator, system_floppy):
    command = emulator.generate(system_floppy, dict(), keyValueSettings("", False), Arguments('path/to/test.adf'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/amiberry', '-config', '/tmp/amiga/conf/default.uae']


def test_simple_generate_rom_rp9(emulator, system_floppy):
    command = emulator.generate(system_floppy, dict(), keyValueSettings("", False), Arguments('path/to/test.rp9'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/amiberry', '-config', 'path/to/test.rp9', '-G']


def test_simple_generate_rom_lha(emulator, system_hddiso):
    command = emulator.generate(system_hddiso, dict(), keyValueSettings("", False), Arguments('path/to/test.lha'))
    assert command.videomode == '16'
    assert command.array == ['/usr/bin/amiberry', '-autoload', 'path/to/test.lha']
