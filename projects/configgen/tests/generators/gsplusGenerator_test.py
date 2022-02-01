#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.gsplus.gsplusGenerator as gsplusGenerator
from configgen.generators.gsplus.gsplusGenerator import GSplusGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllers.controller as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    gsplusGenerator.recalboxFiles.gsplusConfig = 'tests/tmp/gsplus.conf'
    return GSplusGenerator()


@pytest.fixture
def system():
    return Emulator(name='apple2gs', videoMode='1920x1080', ratio='auto', emulator='gsplus', core='gsplus')


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


def test_simple_generate_apple_ii(emulator, system, mocker, controller_configuration):
    mocker.patch("os.path.getsize", return_value=143360)
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/GSplus', '-fullscreen',
                             '-ssdir', '/recalbox/share/screenshots',
                             '-config', 'tests/tmp/gsplus.conf',
                             '-mem', '14680064',
                             '-slot', '6', '-x', '1920',
                             '-y', '1080',
                             '-joy', '-1', '-joy-b0', '13',
                             '-joy-b1', '14', '-joy-b2', '12',
                             '-joy-b3', '15', '-joy-bhk', '16',
                             '-joy-bstart', '3', '-joy-x', '0',
                             '-joy-y', '1', '-joy-x2', '2',
                             '-joy-y2', '3', '-joy-up-button', '4',
                             '-joy-down-button', '6', '-joy-left-button', '7',
                             '-joy-right-button', '5',
                             ]


def test_simple_generate_apple_iigs(emulator, system, mocker):
    mocker.patch("os.path.getsize", return_value=819200)
    command = emulator.generate(system, dict(), keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/GSplus', '-fullscreen',
                             '-ssdir', '/recalbox/share/screenshots',
                             '-config', 'tests/tmp/gsplus.conf',
                             '-mem', '14680064',
                             '-slot', '5', '-x', '1920',
                             '-y', '1080']


def test_simple_generate_apple_iigs_hd(emulator, system, mocker):
    mocker.patch("os.path.getsize", return_value=9000000)
    command = emulator.generate(system, dict(), keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/GSplus', '-fullscreen',
                             '-ssdir', '/recalbox/share/screenshots',
                             '-config', 'tests/tmp/gsplus.conf',
                             '-mem', '14680064',
                             '-slot', '7', '-x', '1920',
                             '-y', '1080']
