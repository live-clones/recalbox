#!/usr/bin/env python

import sys
import os.path
import unittest
import shutil

import configgen.generators.libretro.libretroControllers as libretroControllers
import configgen.settings.unixSettings as unixSettings
import configgen.controllersConfig as controllersConfig
from configgen.Emulator import Emulator
from configgen.settings.keyValueSettings import keyValueSettings

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../..")))

shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../resources/retroarchcustom.cfg.origin")), \
                os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp/retroarchcustom.cfg")))
shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../resources/es_input.cfg.origin")), \
                os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp/es_input.cfg")))

# Injecting test recalbox.conf
libretroControllers.settingsRoot = os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp"))
# Injecting test es_input
controllersConfig.esInputs = os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp/es_input.cfg"))

# Injecting test retroarch.conf
libretroSettingsFile = os.path.abspath(os.path.join(os.path.dirname(__file__), "tmp/retroarchcustom.cfg"))
libretroSettings = unixSettings.UnixSettings(libretroSettingsFile, ' ')
libretroControllers.libretroSettings = libretroSettings


class TestLibretro4ControllerIndex(unittest.TestCase):
    def setUp(self):
        # Test objects
        basicInputs1 = {'a': controllersConfig.Input("a", "button", "10", "1", "0"), 'start': controllersConfig.Input("start", "button", "11", "1", "0")}
        basicController1 = controllersConfig.Controller("contr1", "joypad", "GUID1", '1', 0, "Joypad1RealName", basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)
        basicController2 = controllersConfig.Controller("contr2", "joypad", "GUID2", '2', 1, "Joypad2RealName", basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)
        basicController3 = controllersConfig.Controller("contr3", "joypad", "GUID3", '3', 2, "Joypad3RealName", basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)
        basicController4 = controllersConfig.Controller("contr4", "joypad", "GUID4", '4', 3, "Joypad4RealName", basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)
        self.controllers4 = {"1": basicController1, "2": basicController2, "3": basicController3, "4": basicController4}
        self.controllers2 = {"1": basicController1, "2": basicController2}
        self.controllers2weird = {"3": basicController4, "2": basicController3}
        self.controllers4reversed = {"1": basicController4, "2": basicController3, "3": basicController2, "4": basicController1}

        self.snes = Emulator(name='snes', videomode='4', core='snes9x2002', shaders='', ratio='auto', smooth='2', rewind='false', emulator='libretro')

    def test_4_controller(self):
        controllerConfig = libretroControllers.LibretroControllers(self.snes, keyValueSettings(None, True), self.controllers4)
        config = controllerConfig.fillControllersConfiguration()
        self.assertEquals(config["input_player1_joypad_index"], 0)
        self.assertEquals(config["input_player2_joypad_index"], 1)
        self.assertEquals(config["input_player3_joypad_index"], 2)
        self.assertEquals(config["input_player4_joypad_index"], 3)

    def test_reversed_controller(self):
        controllerConfig = libretroControllers.LibretroControllers(self.snes, keyValueSettings(None, True), self.controllers4reversed)
        config = controllerConfig.fillControllersConfiguration()
        self.assertEquals(config["input_player1_joypad_index"], 3)
        self.assertEquals(config["input_player2_joypad_index"], 2)
        self.assertEquals(config["input_player3_joypad_index"], 1)
        self.assertEquals(config["input_player4_joypad_index"], 0)

    def test_2_last_controllers(self):
        controllerConfig = libretroControllers.LibretroControllers(self.snes, keyValueSettings(None, True), self.controllers2weird)
        config = controllerConfig.fillControllersConfiguration()
        self.assertEquals(config["input_player2_joypad_index"], 2)
        self.assertEquals(config["input_player3_joypad_index"], 3)



if __name__ == '__main__':
    unittest.main()
