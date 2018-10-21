#!/usr/bin/env python

import sys
import os.path
import unittest
import shutil
from configgen.Emulator import Emulator


sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), '../../..')))

import configgen.generators.libretro.libretroConfig as libretroConfig
import configgen.settings.unixSettings as unixSettings
import configgen.settings.recalboxSettings as recalSettings

retroarchcustomFile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'tmp/retroarchcustom.cfg'))
corecustomFile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'tmp/retroarchcores.cfg'))
recalboxConfFile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'tmp/recalbox.conf'))

# Cloning config files
shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../resources/retroarchcustom.cfg.origin')), \
                retroarchcustomFile)
shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../resources/retroarchcores.cfg.origin')), \
                corecustomFile)
shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../resources/recalbox.conf.origin')), \
                recalboxConfFile)



libretroSettings = unixSettings.UnixSettings(retroarchcustomFile, ' ' )
coreSettings = unixSettings.UnixSettings(corecustomFile, ' ' )
# test Systems
snes = Emulator(name='snes', videomode='4', core='pocketsnes', shaders='', ratio='auto', smooth='2', rewind='false', emulator='libretro')
snes9x = Emulator(name='snes', videomode='4', core='snes9x_next', shaders='', ratio='auto', smooth='2', rewind='false', emulator='libretro')
nes = Emulator(name='nes', videomode='4', core='pocketsnes', shaders='', ratio='16/9', smooth='1', rewind='false', emulator='libretro')
nes43 = Emulator(name='nes', videomode='4', core='pocketsnes', shaders='myshaders.gpslp', ratio='4/3', smooth='1', rewind='false', emulator='libretro')
nesauto = Emulator(name='nes', videomode='4', core='pocketsnes', shaders='myshaders.gpslp', ratio='auto', smooth='1', rewind='true', emulator='libretro')
nes43 = Emulator(name='nes', videomode='4', core='pocketsnes', shaders='myshaders.gpslp', ratio='4/3', smooth='1', rewind='false', emulator='libretro')
wswan = Emulator(name='wswan', emulator='libretro', core='mednafen_wswan', ratio='16/10')
nescustomratio = Emulator(name='nes', videomode='4', core='pocketsnes', shaders='myshaders.gpslp', ratio='custom', smooth='1', rewind='true', emulator='libretro')
onePlayerController = {"player1":"p1"}
threePlayersControllers = {"player1":"p1","player2":"p2","player3":"p3"}


class TestLibretroConfig(unittest.TestCase):
    def setUp(self):
        # Injecting test file
        recalSettings.settingsFile = recalboxConfFile
        libretroConfig.libretroSettings = libretroSettings
        libretroConfig.coreSettings = coreSettings

    def test_smooth_override_defaut_and_global(self):
        settings = snes.config
        settings.update(recalSettings.loadAll('snes'))
        retroconf = libretroConfig.createLibretroConfig(snes,onePlayerController)
        self.assertEquals(retroconf['video_smooth'], 'false')

    def test_create_with_shader_true(self):
        settings = snes.config
        settings.update(recalSettings.loadAll('snes'))
        retroconf = libretroConfig.createLibretroConfig(snes, onePlayerController)
        self.assertEquals(retroconf['video_shader'], 'myshaderfile.gplsp')
        self.assertEquals(retroconf['video_shader_enable'], 'true')
        self.assertEquals(retroconf['video_smooth'], 'false')

    def test_create_with_shader_true_and_smooth_true(self):
        settings = snes.config
        settings.update(recalSettings.loadAll('snes'))
        retroconf = libretroConfig.createLibretroConfig(snes, onePlayerController)
        self.assertEquals(retroconf['video_shader_enable'], 'true')
        self.assertEquals(retroconf['video_smooth'], 'false')


    def test_create_with_ratio_169(self):
        retroconf = libretroConfig.createLibretroConfig(nes, onePlayerController)
        self.assertEquals(retroconf['aspect_ratio_index'], 1)
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'false')


    def test_create_with_ratio_43(self):
        retroconf = libretroConfig.createLibretroConfig(nes43, onePlayerController)
        self.assertEquals(retroconf['aspect_ratio_index'], 0)
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'false')

    def test_create_with_ratio_auto(self):
        retroconf = libretroConfig.createLibretroConfig(nesauto, onePlayerController)
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'true')

    def test_create_rewind_true(self):
        retroconf = libretroConfig.createLibretroConfig(nesauto, onePlayerController)
        self.assertEquals(retroconf['rewind_enable'], 'true')

    def test_create_rewind_false(self):
        retroconf = libretroConfig.createLibretroConfig(nes, onePlayerController)
        self.assertEquals(retroconf['rewind_enable'], 'false')


    def test_write_config_to_file(self):
        retroconf = libretroConfig.createLibretroConfig(nesauto, onePlayerController)
        libretroConfig.writeLibretroConfigToFile(retroconf)
        self.assertEquals(libretroSettings.load('rewind_enable'), 'true')

    def test_write_config_to_file_shaders(self):
        retroconf = libretroConfig.createLibretroConfig(nes43, onePlayerController)
        libretroConfig.writeLibretroConfigToFile(retroconf)
        self.assertEquals(libretroSettings.load('rewind_enable'), 'false')
        self.assertEquals(libretroSettings.load('video_shader'), 'myshaders.gpslp')
        self.assertEquals(libretroSettings.load('video_shader_enable'), 'true')
        self.assertEquals(libretroSettings.load('aspect_ratio_index'), '0')
        self.assertEquals(libretroSettings.load('video_aspect_ratio_auto'), 'false')


    def test_write_config_to_file_new1610(self):
        retroconf = libretroConfig.createLibretroConfig(wswan, onePlayerController)
        libretroConfig.writeLibretroConfigToFile(retroconf)
        self.assertEquals(libretroSettings.load('aspect_ratio_index'), '2')
        self.assertEquals(libretroSettings.load('video_aspect_ratio_auto'), 'false')

    def test_driver_udev_default(self):
        nes.config['inputdriver'] = None
        retroconf = libretroConfig.createLibretroConfig(nes, onePlayerController)
        self.assertEquals(retroconf['input_joypad_driver'], 'udev')

    def test_driver_forced_sdl(self):
        nes.config['inputdriver'] = 'sdl2'
        retroconf = libretroConfig.createLibretroConfig(nes, onePlayerController)
        self.assertEquals(retroconf['input_joypad_driver'], 'sdl2')



    def test_create_with_ratio_custom(self):
        retroconf = libretroConfig.createLibretroConfig(nescustomratio, onePlayerController)
        self.assertEquals(retroconf['video_aspect_ratio_auto'], 'false')

    def test_snes9x_and_at_least_three_controllers_then_set_device_p2_to_257(self):
        retroconf = libretroConfig.createLibretroConfig(snes9x, threePlayersControllers)
        self.assertTrue(retroconf['input_libretro_device_p2'] is '257')


if __name__ == '__main__':
    unittest.main()