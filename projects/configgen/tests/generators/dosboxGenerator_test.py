#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.dosbox.dosboxGenerator as dosboxGenerator
from configgen.generators.dosbox.dosboxGenerator import DosBoxGenerator
from configgen.settings.keyValueSettings import keyValueSettings

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    dosboxGenerator.recalboxFiles.dosboxConfig = 'tests/tmp/dosbox.conf'
    return DosBoxGenerator()


@pytest.fixture
def system():
    return Emulator(name='dos', videoMode='1920x1080', ratio='auto', emulator='dosbox', core='dosbox')


def test_simple_generate(emulator, system, mocker):
    command = emulator.generate(system, dict(), keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == 'default'
    assert command.array == ['/usr/bin/dosbox', '-userconf', '-fullscreen',
                             '-exit', 'path/to/test/dosbox.bat', '-c',
                             'set ROOT=path/to/test', '-vkeybd', '/usr/share/dosbox',
                             '-conf', 'tests/tmp/dosbox.conf.extra',
                             '-conf', 'tests/tmp/dosbox.conf']
