#!/usr/bin/env python

import shutil
import unittest
from configgen.Emulator import Emulator
import configgen.generators.advancemame.advMameGenerator as advMameGenerator
from configgen.generators.advancemame.advMameGenerator import AdvMameGenerator
from configgen.generators.advancemame import advMameControllers
from configgen.settings.keyValueSettings import keyValueSettings

from tests.generators.FakeArguments import Arguments

ADVMAME_CUSTOM_CFG_FILE = 'tests/tmp/advmame.rc'
ADVMAME_ORIGIN_CFG_FILE = 'tests/resources/advmame.rc.origin'


class TestAdvancemameGenerator(unittest.TestCase):
    def setUp(self):
        # Injecting test file
        advMameGenerator.recalboxFiles.advancemameConfig = ADVMAME_CUSTOM_CFG_FILE
        advMameGenerator.recalboxFiles.advancemameConfigmOrigin = ADVMAME_ORIGIN_CFG_FILE
        advMameControllers.recalboxFiles.advancemameConfigOrigin = ADVMAME_ORIGIN_CFG_FILE
        self.emulator = AdvMameGenerator()
        self.system = Emulator(name='advancemame', videoMode='16', ratio='auto', emulator='advancemame', core='advancemame')
        # Cloning config files
        shutil.copyfile(ADVMAME_ORIGIN_CFG_FILE, ADVMAME_CUSTOM_CFG_FILE)

    def test_simpleGenerate(self):
        # fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        command = self.emulator.generate(self.system, dict(), keyValueSettings("", False), Arguments('path/to/test.zip'))
        self.assertEqual(command.videomode, 'default')
        self.assertEqual(command.array, ['/usr/bin/advmame', '-cfg', ADVMAME_CUSTOM_CFG_FILE, 'test'])

    def test_verboseGenerate(self):
        # fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        command = self.emulator.generate(self.system, dict(), keyValueSettings("", False), Arguments('path/to/test.zip', verbose=True))
        self.assertEqual(command.videomode, 'default')
        self.assertEqual(command.array, ['/usr/bin/advmame', '-log', '-cfg', ADVMAME_CUSTOM_CFG_FILE, 'test'])


if __name__ == '__main__':
    unittest.main()
