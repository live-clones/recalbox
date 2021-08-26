#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.libretro.libretroConfigurations as libretroConfigurations
import configgen.generators.libretro.libretroGenerator as libretroGenerator
import configgen.generators.libretro.libretroLightGuns as libretroLightGuns
from configgen.generators.libretro.libretroGenerator import LibretroGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    libretroConfigurations.recalboxFiles.retroarchRoot = 'tests/tmp/ra'
    libretroConfigurations.recalboxFiles.retroarchCustom = 'tests/tmp/ra-custom.cfg'
    libretroConfigurations.recalboxFiles.retroarchCoreCustom = 'tests/tmp/ra-core-custom.cfg'
    libretroGenerator.recalboxFiles.BIOS = 'tests/tmp/bios/'
    libretroLightGuns.esLightGun = 'tests/resources/lightgun.cfg'
    libretroLightGuns.GAME_INFO_PATH = 'tests/resources/es_state.inf'
    """fake_process.register_subprocess(
        "udevadm info -e | grep /dev/input/mouse", stdout=["E: DEVNAME=/dev/input/mouse0"]
    )
    fake_process.register_subprocess(
        "cat /proc/bus/input/devices | grep -ni mouse0", stdout=["171:H: Handlers=event10 mouse0"]
    )
    fake_process.register_subprocess(
        "udevadm info /dev/input/mouse0 | grep -ni S:", stdout=["4:S: input/by-path/pci-0000:00:15.1-platform-i2c_designware.1-mouse"]
    )
    fake_process.register_subprocess(
        "udevadm info /dev/input/mouse0 | grep -ni ID_INPUT_MOUSE", stdout=["13:E: ID_INPUT_MOUSE=1"]
    )"""

    return LibretroGenerator()


@pytest.fixture
def emulator_mayflash():
    libretroConfigurations.recalboxFiles.retroarchRoot = 'tests/tmp/ra'
    libretroConfigurations.recalboxFiles.retroarchCustom = 'tests/tmp/ra-custom.cfg'
    libretroConfigurations.recalboxFiles.retroarchCoreCustom = 'tests/tmp/ra-core-custom.cfg'
    libretroGenerator.recalboxFiles.BIOS = 'tests/tmp/bios/'
    """fake_process.register_subprocess(
        "udevadm info -e | grep /dev/input/mouse", stdout=["E: DEVNAME=/dev/input/mouse0"]
    )
    fake_process.register_subprocess(
        "cat /proc/bus/input/devices | grep -ni mouse0", stdout=["171:H: Handlers=event10 mouse0"]
    )
    fake_process.register_subprocess(
        "udevadm info /dev/input/mouse0 | grep -ni S:", stdout=["Mayflash_Wiimote_PC_Adapter"]
    )
    fake_process.register_subprocess(
        "udevadm info /dev/input/mouse0 | grep -ni ID_INPUT_MOUSE", stdout=["13:E: ID_INPUT_MOUSE=1"]
    )"""

    return LibretroGenerator()


@pytest.fixture
def system_nes():
    return Emulator(name='nes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='fceumm')


@pytest.fixture
def system_snes():
    return Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x2002')


@pytest.fixture
def system_px68k():
    return Emulator(name='x68000', videoMode='1920x1080', ratio='auto', emulator='libretro', core='px68k')


@pytest.fixture
def system_quasi88():
    return Emulator(name='pc88', videoMode='1920x1080', ratio='auto', emulator='libretro', core='quasi88')


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


def test_simple_generate_snes(emulator, system_snes, controller_configuration, mocker):
    command = emulator.generate(system_snes, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/snes9x2002_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_nes_mayflash(emulator_mayflash, system_nes, controller_configuration, mocker):
    mocker.patch('os.path.exists', return_value=True)
    command = emulator_mayflash.generate(system_nes, controller_configuration, keyValueSettings("", False), Arguments('path/to/duckhunt'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/fceumm_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/duckhunt']


def test_simple_generate_px68k_hdd(emulator, system_px68k, controller_configuration, mocker):
    mocker.patch('os.path.getsize', return_value=2097152)
    command = emulator.generate(system_px68k, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/px68k_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_px68k_floppy(emulator, system_px68k, controller_configuration, mocker):
    mocker.patch('os.path.getsize', return_value=720000)
    command = emulator.generate(system_px68k, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/px68k_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_quasi88(emulator, system_quasi88, controller_configuration, mocker):
    command = emulator.generate(system_quasi88, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/quasi88_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_quasi88_multi_disk_1(emulator, system_quasi88, controller_configuration, mocker):
    mocker.patch("glob.glob", return_value=['path/to/test(Disk 1 of 5).zip',
                                            'path/to/test(Disk 2 of 5).zip',
                                            'path/to/test(Disk 3 of 5).zip',
                                            'path/to/test(Disk 4 of 5).zip',
                                            'path/to/test(Disk 5 of 5).zip'])
    command = emulator.generate(system_quasi88, controller_configuration, keyValueSettings("", False), Arguments('path/to/test(Disk 1 of 5).zip'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/quasi88_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             '--subsystem', 'pc88_5_disk',
                             'path/to/test(Disk 1 of 5).zip',
                             'path/to/test(Disk 2 of 5).zip',
                             'path/to/test(Disk 3 of 5).zip',
                             'path/to/test(Disk 4 of 5).zip',
                             'path/to/test(Disk 5 of 5).zip']


def test_simple_generate_quasi88_multi_disk_2(emulator, system_quasi88, controller_configuration, mocker):
    mocker.patch("glob.glob", return_value=['path/to/test(Disk 1 of 5)(Disk A).zip',
                                            'path/to/test(Disk 2 of 5)(Disk B).zip',
                                            'path/to/test(Disk 3 of 5)(Disk C).zip',
                                            'path/to/test(Disk 4 of 5)(Disk D).zip',
                                            'path/to/test(Disk 5 of 5)(Disk E).zip'])
    command = emulator.generate(system_quasi88, controller_configuration, keyValueSettings("", False), Arguments('path/to/test(Disk 1 of 5)(Disk A).zip'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/quasi88_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             '--subsystem', 'pc88_5_disk',
                             'path/to/test(Disk 1 of 5)(Disk A).zip',
                             'path/to/test(Disk 2 of 5)(Disk B).zip',
                             'path/to/test(Disk 3 of 5)(Disk C).zip',
                             'path/to/test(Disk 4 of 5)(Disk D).zip',
                             'path/to/test(Disk 5 of 5)(Disk E).zip']
