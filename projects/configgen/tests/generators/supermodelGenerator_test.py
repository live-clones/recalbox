#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.supermodel.supermodelGenerator as supermodelGenerator
import configgen.generators.supermodel.supermodelControllers as supermodelControllers
from configgen.generators.supermodel.supermodelGenerator import SupermodelGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllers.controller as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    supermodelGenerator.recalboxFiles.supermodelConfigFile = 'tests/resources/ConfigModel3.ini'
    supermodelControllers.recalboxFiles.supermodelControlsIni = 'tests/tmp/supermodel.ini'
    return SupermodelGenerator()


@pytest.fixture
def system():
    return Emulator(name='supermodel', videoMode='1920x1080', ratio='auto', emulator='supermodel', core='supermodel')


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


def test_simple_generate_supermodel(emulator, system, mocker, controller_configuration, fake_process):
    fake_process.register_subprocess(
        ["tvservice -s"],
        stdout=['[{ "code":1, "width":1920, "height":1080 }]']
    )
    mocker.patch('platform.machine', return_value='x86_64')
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/supermodel', 'path/to/test',
                             '-new-scsp',
                             '-fullscreen', '-res=1920,1080',
                             '-no-multi-texture', '-legacy3d',
                             '-crosshairs=1', '-gpu-multi-threaded',
                             '-ppc-frequency=100',
                             '-load-state=/recalbox/share/saves/model3/',
                             '-log-output=/recalbox/share/system/configs/model3/Supermodel.log',
                             '-log-level=info']
