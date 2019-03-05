#!/usr/bin/env python

import sys
import os.path
import unittest
import shutil

from configgen import controllersConfig
from configgen.Emulator import Emulator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.generators.libretro.libretroConfigurations as libretroConfigurations
import configgen.settings.recalboxSettings as recalSettings

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../..')))


class TestLibretroConfig(unittest.TestCase):

    def setUp(self):
        # Reset test files
        self.retroarchcustomFile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'tmp/retroarchcustom.cfg'))
        self.corecustomFile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'tmp/retroarchcores.cfg'))
        self.recalboxConfFile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'tmp/recalbox.conf'))

        # Cloning config files
        shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../resources/retroarchcustom.cfg.origin')), self.retroarchcustomFile)
        shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../resources/retroarchcores.cfg.origin')), self.corecustomFile)
        shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../resources/recalbox.conf.origin')), self.recalboxConfFile)

        # Injecting test file
        recalSettings.settingsFile = self.recalboxConfFile

        # Create configurations
        self.snes = Emulator(name='snes', videomode='4', core='snes9x2002', shaders='', ratio='auto', smooth='2', rewind='false', emulator='libretro')
        self.snes9x = Emulator(name='snes', videomode='4', core='snes9x_next', shaders='', ratio='auto', smooth='2', rewind='false', emulator='libretro')
        self.nes = Emulator(name='nes', videomode='4', core='snes9x2002', shaders='', ratio='16/9', smooth='1', rewind='false', emulator='libretro')
        self.nes43 = Emulator(name='nes', videomode='4', core='snes9x2002', shaders='myshaders.gpslp', ratio='4/3', smooth='1', rewind='false', emulator='libretro')
        self.nesauto = Emulator(name='nes', videomode='4', core='snes9x2002', shaders='myshaders.gpslp', ratio='auto', smooth='1', rewind='true', emulator='libretro')
        self.wswan = Emulator(name='wswan', emulator='libretro', core='mednafen_wswan', ratio='16/10')
        self.nescustomratio = Emulator(name='nes', videomode='4', core='snes9x2002', shaders='myshaders.gpslp', ratio='custom', smooth='1', rewind='true', emulator='libretro')

        # test inputs
        self.basicInputs1 = { 'hotkey': controllersConfig.Input("hotkey", "button", "10", "1","") }
        self.basicController1 = controllersConfig.Controller("contr1", "joypad", "GUID1", 1, 0, "Joypad1RealName", self.basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)
        self.basicController2 = controllersConfig.Controller("contr2", "joypad", "GUID2", 1, 0, "Joypad2RealName", self.basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)
        self.basicController3 = controllersConfig.Controller("contr3", "joypad", "GUID3", 1, 0, "Joypad3RealName", self.basicInputs1, nbaxes=6, nbhats=1, nbbuttons=10)

        self.onePlayerController = { "1": self.basicController1 }
        self.threePlayersControllers = { "1": self.basicController1, "2": self.basicController2, "3": self.basicController3 }

    def test_smooth_override_defaut_and_global(self):
        settings = self.snes.config
        settings.update(recalSettings.loadAll('snes'))
        builder = libretroConfigurations.LibretroConfiguration(self.snes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['video_smooth'], 'false')

    def test_create_with_shader_true(self):
        settings = self.snes.config
        settings.update(recalSettings.loadAll('snes'))
        builder = libretroConfigurations.LibretroConfiguration(self.snes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['video_shader'], 'myshaderfile.gplsp')
        self.assertEquals(retroconf['video_shader_enable'], 'true')
        self.assertEquals(retroconf['video_smooth'], 'false')

    def test_create_with_shader_true_and_smooth_true(self):
        settings = self.snes.config
        settings.update(recalSettings.loadAll('snes'))
        builder = libretroConfigurations.LibretroConfiguration(self.snes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['video_shader_enable'], 'true')
        self.assertEquals(retroconf['video_smooth'], 'false')


    def test_create_with_ratio_169(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['aspect_ratio_index'], 1)
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'false')


    def test_create_with_ratio_43(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nes43, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['aspect_ratio_index'], 0)
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'false')

    def test_create_with_ratio_auto(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nesauto, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'true')

    def test_create_rewind_true(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nesauto, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['rewind_enable'], 'true')

    def test_create_rewind_false(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['rewind_enable'], 'false')


    def test_write_config_to_file(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nesauto, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        builder.createRetroarchConfiguration()
        reloaded = keyValueSettings(self.retroarchcustomFile, True)
        reloaded.loadFile(True)
        self.assertEquals(reloaded['rewind_enable'], 'true')

    def test_write_config_to_file_shaders(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nes43, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        builder.createRetroarchConfiguration()
        reloaded = keyValueSettings(self.retroarchcustomFile, True)
        reloaded.loadFile(True)
        self.assertEquals(reloaded['rewind_enable'], 'false')
        self.assertEquals(reloaded['video_shader'], 'myshaders.gpslp')
        self.assertEquals(reloaded['video_shader_enable'], 'true')
        self.assertEquals(reloaded['aspect_ratio_index'], '0')
        self.assertEquals(reloaded['video_aspect_ratio_auto'], 'false')


    def test_write_config_to_file_new1610(self):
        builder = libretroConfigurations.LibretroConfiguration(self.wswan, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        builder.createRetroarchConfiguration()
        reloaded = keyValueSettings(self.retroarchcustomFile, True)
        reloaded.loadFile(True)
        self.assertEquals(reloaded['aspect_ratio_index'], '2')
        self.assertEquals(reloaded['video_aspect_ratio_auto'], 'false')

    def test_driver_udev_default(self):
        self.nes.config['inputdriver'] = None
        builder = libretroConfigurations.LibretroConfiguration(self.nes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['input_joypad_driver'], 'udev')

    def test_driver_forced_sdl(self):
        self.nes.config['inputdriver'] = 'sdl2'
        builder = libretroConfigurations.LibretroConfiguration(self.nes, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['input_joypad_driver'], 'sdl2')



    def test_create_with_ratio_custom(self):
        builder = libretroConfigurations.LibretroConfiguration(self.nescustomratio, self.onePlayerController, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'false')

    def test_snes9x_and_at_least_three_controllers_then_set_device_p2_to_257(self):
        builder = libretroConfigurations.LibretroConfiguration(self.snes9x, self.threePlayersControllers, "/rom.rom")
        builder.overrideLibretroConfigurationFiles(None, self.retroarchcustomFile)
        retroconf = builder.createRetroarchConfiguration()
        self.assertTrue(retroconf['input_libretro_device_p2'] is '257')


if __name__ == '__main__':
    unittest.main()