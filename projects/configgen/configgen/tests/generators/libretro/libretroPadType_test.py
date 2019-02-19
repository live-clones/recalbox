#!/usr/bin/env python

import os
import sys
import os.path
import unittest
import shutil

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), "../../..")))

import configgen.generators.libretro.libretroControllers as libretroControllers
import configgen.controllersConfig as controllersConfig
from configgen.Emulator import Emulator
# Cloning config files
shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../resources/retroarchcustom.cfg.origin")), \
                os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp/retroarchcustom.cfg")))

# Injecting test retroarchroot
libretroControllers.settingsRoot = os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp"))

onlyBtnsInputs = {'up': controllersConfig.Input("up", "button", "1", "1", "0")}
onlyHatsInputs = {'up': controllersConfig.Input("up", "hat", "1", "1", "0")}
onlyAxisInputs = {'up': controllersConfig.Input("up", "axis", "1", "1", "0")}

axisAndBtnsInputs = {'up': controllersConfig.Input("up", "button", "1", "1", "0"),
                     'joystickup': controllersConfig.Input("up", "axis", "1", "1", "0")}
snes = Emulator('snes', 'snes', 'libretro')
psx = Emulator('psx', 'psx', 'libretro')

# Test the padtype settings
class TestLibretroAnalogMode(unittest.TestCase):
    def test_padtype_only_btn(self):
        basicControllerBTN = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', "0", "Joypad1RealName",
                                                          onlyBtnsInputs, nbaxes=6, nbhats=1, nbbuttons=10)
        padtype = libretroControllers.LibretroControllers.getAnalogMode(basicControllerBTN)
        self.assertEqual("1", padtype)

    def test_padtype_only_joystick(self):
        basicController = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', "0", "Joypad1RealName",
                                                       onlyAxisInputs, nbaxes=6, nbhats=1, nbbuttons=10)
        padtype = libretroControllers.LibretroControllers.getAnalogMode(basicController)
        self.assertEqual("0", padtype)

    def test_padtype_only_hats(self):
        basicController = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', "0", "Joypad1RealName",
                                                       onlyHatsInputs, nbaxes=6, nbhats=1, nbbuttons=10)
        padtype = libretroControllers.LibretroControllers.getAnalogMode(basicController)
        self.assertEqual("1", padtype)

    def test_padtype_dual1(self):
        basicController = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', "0", "Joypad1RealName",
                                                       axisAndBtnsInputs, nbaxes=6, nbhats=1, nbbuttons=10)
        padtype = libretroControllers.LibretroControllers.getAnalogMode(basicController)
        self.assertEqual("1", padtype)


    def test_padtype_dual2(self):
        basicController = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', "0", "Joypad1RealName",
                                                       axisAndBtnsInputs, nbaxes=6, nbhats=1, nbbuttons=10)
        padtype = libretroControllers.LibretroControllers.getAnalogMode(basicController)
        self.assertEqual("1", padtype)

    def test_padtype_psx(self):
        basicController = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', "0", "Joypad1RealName",
                                                       axisAndBtnsInputs, nbaxes=6, nbhats=1, nbbuttons=10)
        padtype = libretroControllers.LibretroControllers.getAnalogMode(basicController)
        self.assertEqual("1", padtype)