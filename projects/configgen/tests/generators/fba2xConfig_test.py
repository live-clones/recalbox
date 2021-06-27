#!/usr/bin/env python

import shutil
import unittest
from configgen.Emulator import Emulator
from configgen.generators.fba2x import fba2xConfig
import configgen.settings.unixSettings as unixSettings

fba2xCustom = 'tests/tmp/fba2x.cfg'

# Cloning config files
shutil.copyfile('tests/resources/fba2x.cfg.origin', fba2xCustom)

fbaSettings = unixSettings.UnixSettings(fba2xCustom)

# test Systems
fba2xNormal = Emulator(name='fba', videomode='4', ratio='auto', smooth='1', emulator='fba2x')
fba2x43 = Emulator(name='fba', videomode='4', ratio='4/3', smooth='0', shaders='scanlines', emulator='fba2x')
fba2x169 = Emulator(name='fba', videomode='4', ratio='16/9', emulator='fba2x')


class TestFBAConfig(unittest.TestCase):
    def setUp(self):
        # Injecting test file
        fba2xConfig.fbaSettings = fbaSettings

    def test_ratioAutoReturnsMaintainAspect1(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        self.assertEqual(fbaConf['MaintainAspectRatio'], '1')

    def test_ratio43ReturnsMaintainAspect1(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2x43)
        self.assertEqual(fbaConf['MaintainAspectRatio'], '1')

    def test_ratio169ReturnsMaintainAspect0(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2x169)
        self.assertEqual(fbaConf['MaintainAspectRatio'], '0')

    def test_SmoothReturnsDisplaySmoothStretch(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        self.assertEqual(fbaConf['DisplaySmoothStretch'], '1')

    def test_SmoothOffReturnsDisplaySmoothStretch0(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2x43)
        self.assertEqual(fbaConf['DisplaySmoothStretch'], '0')

    def test_shadersOffNoScanlines(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        self.assertEqual(fbaConf['DisplayEffect'], '0')

    def test_shadersSetToScanlineReturnsScanlines(self):
        fbaConf = fba2xConfig.createFBAConfig(fba2x43)
        self.assertEqual(fbaConf['DisplayEffect'], '1')


if __name__ == '__main__':
    unittest.main()
