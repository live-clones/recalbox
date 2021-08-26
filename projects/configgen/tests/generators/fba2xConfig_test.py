#!/usr/bin/env python

import shutil
import unittest
from configgen.Emulator import Emulator
from configgen.generators.fba2x import fba2xConfig
from configgen.mutableEmulator import MutableEmulator

fba2xCustom = 'tests/tmp/fba2x.cfg'

# Cloning config files
shutil.copyfile('tests/resources/fba2x.cfg.origin', fba2xCustom)

# test Systems
fba2xNormal = Emulator(name='fba', videoMode='4', ratio='auto', emulator='fba2x', core='fba2x')
fba2x43 = MutableEmulator(name='fba', videoMode='4', ratio='4/3', emulator='fba2x', core='fba2x')
fba2x43.SetShaderSet("scanlines").SetSmooth(False)
fba2x169 = Emulator(name='fba', videoMode='4', ratio='16/9', emulator='fba2x', core='fba2x')


class TestFBAConfig(unittest.TestCase):
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
