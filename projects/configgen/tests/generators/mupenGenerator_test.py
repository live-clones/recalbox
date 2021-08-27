#!/usr/bin/env python
import pytest
import shutil
from configgen.Emulator import Emulator
import configgen.generators.mupen.mupenControllers as mupenControllers
import configgen.generators.mupen.mupenConfig as mupenConfig
from configgen.generators.mupen import mupenGenerator
from configgen.generators.mupen.mupenGenerator import MupenGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllers.controller as controllersConfig

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
