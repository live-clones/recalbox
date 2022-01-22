#!/usr/bin/env python
import shutil
from pathlib import Path

import pytest
import configgen.generators.libretro.libretroConfigurations as libretroConfigurations
import configgen.generators.libretro.libretroGenerator as libretroGenerator
import configgen.generators.libretro.libretroLightGuns as libretroLightGuns
from configgen.Command import Command
from configgen.Emulator import Emulator, ExtraArguments
from configgen.crt.Mode import Mode
from configgen.generators.libretro.libretroGenerator import LibretroGenerator
from configgen.settings.keyValueSettings import keyValueSettings
import configgen.controllers.controller as controllersConfig
from tests.Givens import givenThoseFiles
from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    libretroConfigurations.recalboxFiles.retroarchRoot = 'tests/tmp/ra'
    libretroConfigurations.recalboxFiles.retroarchCustom = 'tests/tmp/ra-custom.cfg'
    libretroConfigurations.recalboxFiles.retroarchCoreCustom = 'tests/tmp/ra-core-custom.cfg'
    libretroGenerator.recalboxFiles.BIOS = 'tests/tmp/bios/'
    libretroLightGuns.esLightGun = 'tests/resources/lightgun.cfg'
    libretroLightGuns.GAME_INFO_PATH = 'tests/resources/es_state.inf'
    return LibretroGenerator()


@pytest.fixture
def emulator_mayflash():
    libretroLightGuns.recalboxFiles.esLightGun = 'tests/tmp/lightgun.cfg'
    libretroConfigurations.recalboxFiles.retroarchRoot = 'tests/tmp/ra'
    libretroConfigurations.recalboxFiles.retroarchCustom = 'tests/tmp/ra-custom.cfg'
    libretroConfigurations.recalboxFiles.retroarchCoreCustom = 'tests/tmp/ra-core-custom.cfg'
    libretroGenerator.recalboxFiles.BIOS = 'tests/tmp/bios/'
    shutil.copyfile('tests/resources/lightgun.cfg', 'tests/tmp/lightgun.cfg')

    return LibretroGenerator()


@pytest.fixture
def system_nes():
    return Emulator(name='nes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='fceumm')


@pytest.fixture
def system_gb_with_overlays():
    gb = Emulator(name='gb', videoMode='1920x1080', ratio='auto', emulator='libretro', core='gb')
    recalbox_conf = keyValueSettings("", True)
    recalbox_conf.setString("global.recalboxoverlays", "1")
    gb.configure(recalbox_conf, ExtraArguments("", "", "", "", "", "", "", "", "", "", "", 0, 0, 0, "auto"))
    return gb

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


def test_simple_generate_snes(emulator, system_snes, controller_configuration):
    command = emulator.generate(system_snes, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/snes9x2002_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_nes_mayflash(emulator_mayflash, system_nes, controller_configuration, mocker):
    mocker.patch('os.path.exists', return_value=True)
    command = emulator_mayflash.generate(system_nes, controller_configuration, keyValueSettings("", False),
                                         Arguments('path/to/duckhunt'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/fceumm_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/duckhunt']


def test_simple_generate_px68k_hdd(emulator, system_px68k, controller_configuration, mocker):
    mocker.patch('os.path.getsize', return_value=2097152)
    command = emulator.generate(system_px68k, controller_configuration, keyValueSettings("", False),
                                Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/px68k_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_px68k_floppy(emulator, system_px68k, controller_configuration, mocker):
    mocker.patch('os.path.getsize', return_value=720000)
    command = emulator.generate(system_px68k, controller_configuration, keyValueSettings("", False),
                                Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/retroarch', '-L', '/usr/lib/libretro/px68k_libretro.so',
                             '--config', 'tests/tmp/ra-custom.cfg',
                             '--appendconfig', 'tests/tmp/ra-custom.cfg.overrides.cfg',
                             'path/to/test']


def test_simple_generate_quasi88(emulator, system_quasi88, controller_configuration):
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
    command = emulator.generate(system_quasi88, controller_configuration, keyValueSettings("", False),
                                Arguments('path/to/test(Disk 1 of 5).zip'))
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
    command = emulator.generate(system_quasi88, controller_configuration, keyValueSettings("", False),
                                Arguments('path/to/test(Disk 1 of 5)(Disk A).zip'))
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


def test_crt_enabled_create_mode_configuration(mocker, emulator, system_snes, controller_configuration):
    recalbox_conf = keyValueSettings("", True)
    givenThoseFiles(mocker, {
        "/recalbox/share/system/configs/crt/systems.txt": "snes,pal,15kHz,progressive,snes:224@50p,1740,224\nsnes,ntsc,15kHz,progressive,snes:224@60p,0,0",
        "/recalbox/share/system/configs/crt/modes.txt": "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60.1\nsnes:224@50p,1920 1 78 192 210 224 1 3 3 16 0 0 0 50 0 37730000 1,50.1"})

    system_snes.configure(recalbox_conf,
                          ExtraArguments("", "", "", "", "", "", "", "auto", "progressive", "15kHz", "recalboxrgbdual", -2, -2, 1820, "auto"))

    emulator.generate(system_snes, controller_configuration, recalbox_conf, Arguments('path/to/rom'))
    generated_config = Path(libretroConfigurations.recalboxFiles.retroarchCustom).read_text()
    assert 'crt_switch_timings_ntsc = "1920 1 90 192 198 224 1 5 3 14 0 0 0 60 0 37730000 1"' in generated_config
    assert 'crt_switch_timings_pal = "1920 1 90 192 198 224 1 5 3 14 0 0 0 50 0 37730000 1"' in generated_config
    assert 'custom_viewport_width_ntsc = 1820' in generated_config
    assert 'custom_viewport_width_pal = 1740' in generated_config
    assert 'video_refresh_rate_ntsc = "60.1"' in generated_config
    assert 'video_refresh_rate_pal = "50.1"' in generated_config

def test_crt_enabled_create_overlay_configuration(mocker, emulator, system_gb_with_overlays, controller_configuration):
    recalbox_conf = keyValueSettings("", True)
    recalbox_conf.setString("global.recalboxoverlays", "1")

    system_gb_with_overlays.configure(recalbox_conf,
                                      ExtraArguments("", "", "", "", "", "", "", "auto", "progressive", "15kHz",
                                                     "recalboxrgbdual", 0, 0, 0, "auto"))

    givenThoseFiles(mocker, {
        "/recalbox/share/system/configs/crt/systems.txt": "gb,pal,15kHz,progressive,gb:pal:240@50p,1740,224\ngb,ntsc,15kHz,progressive,gb:ntsc:224@60p,0,0",
        "/recalbox/share/system/configs/crt/modes.txt": "gb:pal:240@50p,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,501\ngb:ntsc:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60.1"})

    # Easy way to fake overlay file on file system
    mocker.patch('os.path.isfile', return_value=True)

    commandLine: Command = emulator.generate(system_gb_with_overlays, controller_configuration, recalbox_conf, Arguments('path/to/rom'))
    assert "/recalbox/share_init/240poverlays/gb/gb.cfg" in commandLine.array[6]
