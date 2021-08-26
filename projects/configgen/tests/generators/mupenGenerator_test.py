#!/usr/bin/env python
import pytest
import shutil
from configgen.Emulator import Emulator
import configgen.generators.mupen.mupenControllers as mupenControllers
import configgen.generators.mupen.mupenConfig as mupenConfig
from configgen.generators.mupen import mupenGenerator
from configgen.generators.mupen.mupenGenerator import MupenGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    mupenGenerator.recalboxFiles.mupenCustom = 'tests/tmp/mupen64plus.cfg'
    mupenConfig.recalboxFiles.mupenCustom = 'tests/tmp/mupen64plus.cfg'
    mupenControllers.recalboxFiles.mupenMappingSystem = 'tests/tmp/mupen64plus-input.xml'
    mupenControllers.recalboxFiles.mupenInput = 'tests/tmp/mupen64plus-InputAutoCfg.ini'
    shutil.copyfile('tests/resources/input.xml', 'tests/tmp/mupen64plus-input.xml')
    return MupenGenerator()


@pytest.fixture
def system_gliden64():
    return Emulator(name='n64', videoMode='1920x1080', ratio='auto', emulator='mupen64plus', core='gliden64')


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


def test_simple_generate_mupen_gliden64(emulator, system_gliden64, controller_configuration):
    command = emulator.generate(system_gliden64, controller_configuration, keyValueSettings("", False), Arguments('path/to/test.n64'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/mupen64plus', '--corelib', '/usr/lib/libmupen64plus.so.2.0.0',
                             '--gfx', '/usr/lib/mupen64plus/mupen64plus-video-gliden64.so',
                             '--configdir', '/recalbox/share/system/configs/mupen64/',
                             '--datadir', '/recalbox/share/system/configs/mupen64/',
                             '--sshotdir', '/recalbox/share/screenshots',
                             '--fullscreen', '--resolution', '1920x1080',
                             'path/to/test.n64',
                             ]
