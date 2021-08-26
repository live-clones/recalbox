#!/usr/bin/env python

import shutil
import unittest
from configgen.Emulator import Emulator
import configgen.settings.unixSettings as unixSettings
import configgen.generators.fba2x.fba2xGenerator as fba2xGenerator
from configgen.generators.fba2x.fba2xGenerator import Fba2xGenerator
from configgen.settings.keyValueSettings import keyValueSettings

from tests.generators.FakeArguments import Arguments

FBA2X_CUSTOM_CFG_FILE = 'tests/tmp/fba2x.cfg'
FBA2X_ORIGIN_CFG_FILE = 'tests/resources/fba2x.cfg.origin'


class TestFBAGenerator(unittest.TestCase):
    def setUp(self):
        # Injecting test file
        fba2xGenerator.recalboxFiles.fbaCustom = FBA2X_CUSTOM_CFG_FILE
        fba2xGenerator.recalboxFiles.fbaCustomOrigin = FBA2X_ORIGIN_CFG_FILE
        fba2xGenerator.fba2xConfig.fbaSettings = unixSettings.UnixSettings(FBA2X_CUSTOM_CFG_FILE)
        fba2xGenerator.fba2xControllers.fbaSettings = unixSettings.UnixSettings(FBA2X_CUSTOM_CFG_FILE)
        self.fba2xGen = Fba2xGenerator()
        self.system = Emulator(name='fba', videoMode='4', ratio='auto', emulator='fba2x', core='fba2x')
        # Cloning config files
        shutil.copyfile(FBA2X_ORIGIN_CFG_FILE, FBA2X_CUSTOM_CFG_FILE)

    def test_ratioAutoReturnsMaintainAspect1(self):
        # fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        command = self.fba2xGen.generate(self.system, dict(), keyValueSettings("", False), Arguments('test.zip'))
        self.assertEqual(command.videomode, '4')
        self.assertEqual(command.array, ['/usr/bin/fba2x', '--configfile', FBA2X_CUSTOM_CFG_FILE, '--logfile', '/recalbox/share/system/logs//fba2x.log', 'test.zip'])


if __name__ == '__main__':
    unittest.main()
