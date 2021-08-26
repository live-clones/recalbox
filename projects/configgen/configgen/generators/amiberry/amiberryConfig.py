#!/usr/bin/env python
import os
from typing import List

import configgen.recalboxFiles as recalboxFiles
from configgen.generators.amiberry.amiberryKickstarts import KickstartManager
from configgen.generators.amiberry.amiberryRetroarchConfig import AmiberryRetroarchConfig
from configgen.generators.amiberry.amiberrySubSystems import SubSystems
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.controllersConfig import ControllerDictionary
from configgen.generators.amiberry.amiberryRomType import RomType


class ConfigGenerator:

    def __init__(self, targetFileName: str):
        self.settingsFileName: str = targetFileName
        self.settings = keyValueSettings("")

    def loadConfigFile(self, filename: str):
        self.settings.changeSettingsFile(filename)
        self.settings.loadFile(False)

    def saveConfigFile(self):
        self.settings.changeSettingsFile(self.settingsFileName)
        self.settings.saveFile()

    def SetDefaultPath(self, subsystem: SubSystems):
        romsPath = os.path.join(recalboxFiles.ROMS, str(subsystem))
        self.settings.setOption("config_description", "Recalbox auto-generated configuration for Amiga " + str(subsystem))
        self.settings.setOption("config_hardware", "true")
        self.settings.setOption("config_host", "true")
        self.settings.setOption("config_version", "4.4.0")
        self.settings.setOption("amiberry.rom_path", romsPath)
        self.settings.setOption("amiberry.floppy_path", romsPath)
        self.settings.setOption("amiberry.hardfile_path", romsPath)
        self.settings.setOption("amiberry.cd_path", romsPath)
        if subsystem == SubSystems.CD32:
            self.settings.setOption("flash_file", os.path.join(recalboxFiles.SAVES, "{}/cd32.nvr".format(subsystem)))
        if subsystem == SubSystems.CDTV:
            self.settings.setOption("flash_file", os.path.join(recalboxFiles.SAVES, "{}/cdtv.nvr".format(subsystem)))
        if subsystem in SubSystems.COMPUTERS:
            self.settings.setOption("pcmcia_mb_rom_file", ":ENABLED")
            self.settings.setOption("ide_mb_rom_file", ":ENABLED")

    def SetUI(self, keyboardLayout: str, showfps: bool):
        self.settings.setOption("use_gui", "no")
        self.settings.setOption("show_leds", "yes" if showfps else "no")
        # Valid for amiberry?
        if keyboardLayout not in ["us", "fr", "de", "dk", "es", "se", "it"]:
            keyboardLayout = "us"
        self.settings.setOption("kbd_lang", keyboardLayout)

    def SetInput(self, subsystem: SubSystems):
        if subsystem in SubSystems.HAVEMOUSE:
            self.settings.setOption("input.joymouse_speed_analog", "2")
            self.settings.setOption("input.joymouse_speed_digital", "10")
            self.settings.setOption("input.joymouse_deadzone", "33")
            self.settings.setOption("input.mouse_speed", "100")
        self.settings.setOption("input.joystick_deadzone", "33")
        self.settings.setOption("input.analog_joystick_multiplier", "15")
        self.settings.setOption("input.analog_joystick_offset", "-1")
        self.settings.setOption("input.autofire_speed", "0")
        
    def SetJoystick(self, subsystem: SubSystems, controllers: ControllerDictionary):
        self.settings.setOption("amiberry.use_analogue_remap", "false")
        self.settings.setOption("amiberry.use_retroarch_quit", "true")
        self.settings.setOption("amiberry.use_retroarch_menu", "true")
        self.settings.setOption("amiberry.use_retroarch_reset", "true")
        if subsystem in SubSystems.HAVEMOUSE:
            # Set mouse
            self.settings.setOption("joyport0", "mouse")
            self.settings.setOption("joyport0_autofire", "none")
            self.settings.setOption("joyport0_mode", "mousenowheel")
            self.settings.setOption("joyport0_mousemap", "right")
            self.settings.setOption("joyport0_friendlyname", "Mouse")
            self.settings.setOption("joyport0_name", "MOUSE0")
            # Set joysticks
            for key in controllers:
                controller = controllers[key]
                player = int(controller.player)
                indexPadSDL = int(controller.index) + 1
                if 1 <= player <= 3:
                    self.settings.setOption("joyport{}".format(key), "joy{}".format(indexPadSDL))
                    self.settings.setOption("joyport{}_autofire".format(key), "none")
                    self.settings.setOption("joyport{}_mode".format(key), "cd32joy" if subsystem == SubSystems.CD32 else "djoy")
                    self.settings.setOption("joyport{}_mousemap".format(key), "right")
                    self.settings.setOption("joyport{}_friendlyname".format(key), controller.realName)
                    self.settings.setOption("joyport{}_name".format(key), "JOY{}".format(indexPadSDL))
                    retroarchConfig = AmiberryRetroarchConfig(controller)
                    retroarchConfig.generateConfiguration()
                    retroarchConfig.saveConfigurationFile()
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetCPU(self, subsystem: SubSystems, needSlowCPU: bool):
        if subsystem == SubSystems.A600:
            self.settings.setOption("cpu_speed", "real")
            self.settings.setOption("cpu_type", "68000")
            self.settings.setOption("cpu_model", "68000")
            self.settings.setOption("cpu_compatible", "true")
            self.settings.setOption("cpu_24bit_addressing", "true")
            self.settings.setOption("fpu_no_unimplemented", "true")
            self.settings.setOption("fpu_strict", "false")
            self.settings.setOption("compfpu", "false")
            self.settings.setOption("cachesize", "0")
        elif subsystem == SubSystems.A1200:
            if needSlowCPU:
                # Tweak the CPU to workaround Amiberry's loader bug on 68000/68010 CPUs
                # We run a 1200 slowed down to a normal 68000
                # https://github.com/midwan/amiberry/issues/417
                self.settings.setOption("cpu_speed", "real")
            else:
                self.settings.setOption("finegrain_cpu_speed", "1024")
            self.settings.setOption("cpu_type", "68ec020")
            self.settings.setOption("cpu_model", "68020")
            self.settings.setOption("cpu_compatible", "false")
            self.settings.setOption("cpu_24bit_addressing", "true")
            self.settings.setOption("fpu_no_unimplemented", "true")
            self.settings.setOption("fpu_strict", "false")
            self.settings.setOption("compfpu", "false")
            self.settings.setOption("cachesize", "0")
        elif subsystem == SubSystems.CD32:
            self.settings.setOption("finegrain_cpu_speed", "1024")
            self.settings.setOption("cpu_type", "68ec020")
            self.settings.setOption("cpu_model", "68020")
            self.settings.setOption("cpu_compatible", "false")
            self.settings.setOption("cpu_24bit_addressing", "true")
            self.settings.setOption("fpu_no_unimplemented", "true")
            self.settings.setOption("fpu_strict", "false")
            self.settings.setOption("compfpu", "false")
            self.settings.setOption("cachesize", "0")
        elif subsystem == SubSystems.CDTV:
            self.settings.setOption("cpu_speed", "real")
            self.settings.setOption("cpu_type", "68000")
            self.settings.setOption("cpu_model", "68000")
            self.settings.setOption("cpu_compatible", "true")
            self.settings.setOption("cpu_24bit_addressing", "true")
            self.settings.setOption("fpu_no_unimplemented", "true")
            self.settings.setOption("fpu_strict", "false")
            self.settings.setOption("compfpu", "false")
            self.settings.setOption("cachesize", "0")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetChipset(self, subsystem: SubSystems):
        if subsystem == SubSystems.A600:
            self.settings.setOption("chipset", "ecs")
            self.settings.setOption("chipset_refreshrate", "50.000000")
            self.settings.setOption("collision_level", "playfields")
            self.settings.setOption("chipset_compatible", "A600")
            self.settings.setOption("rtc", "none")
            self.settings.setOption("ksmirror_a8", "true")
            self.settings.setOption("pcmcia", "true")
            self.settings.setOption("cia_todbug", "true")
            self.settings.setOption("immediate_blits", "false")
            self.settings.setOption("waiting_blits", "automatic")
            self.settings.setOption("fast_copper", "false")
        elif subsystem == SubSystems.A1200:
            self.settings.setOption("chipset", "aga")
            self.settings.setOption("chipset_refreshrate", "50.000000")
            self.settings.setOption("collision_level", "playfields")
            self.settings.setOption("chipset_compatible", "A1200")
            self.settings.setOption("rtc", "MSM6242B")
            self.settings.setOption("ksmirror_a8", "true")
            self.settings.setOption("pcmcia", "true")
            self.settings.setOption("immediate_blits", "false")
            self.settings.setOption("waiting_blits", "automatic")
            self.settings.setOption("fast_copper", "false")
        elif subsystem == SubSystems.CD32:
            self.settings.setOption("chipset", "aga")
            self.settings.setOption("chipset_refreshrate", "50.000000")
            self.settings.setOption("collision_level", "playfields")
            self.settings.setOption("chipset_compatible", "CD32")
            self.settings.setOption("rtc", "none")
            self.settings.setOption("ksmirror_e0", "false")
            self.settings.setOption("ksmirror_a8", "true")
            self.settings.setOption("cd32cd", "true")
            self.settings.setOption("cd32c2p", "true")
            self.settings.setOption("cd32nvram", "true")
            self.settings.setOption("immediate_blits", "false")
            self.settings.setOption("waiting_blits", "automatic")
            self.settings.setOption("fast_copper", "false")
        elif subsystem == SubSystems.CDTV:
            self.settings.setOption("chipset", "ecs_agnus")
            self.settings.setOption("chipset_refreshrate", "50.000000")
            self.settings.setOption("collision_level", "playfields")
            self.settings.setOption("chipset_compatible", "CDTV")
            self.settings.setOption("rtc", "MSM6242B")
            self.settings.setOption("ksmirror_e0", "false")
            self.settings.setOption("immediate_blits", "false")
            self.settings.setOption("waiting_blits", "automatic")
            self.settings.setOption("fast_copper", "false")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetMemory(self, subsystem: SubSystems):
        if subsystem == SubSystems.A600:
            self.settings.setOption("chipmem_size", "2")
            self.settings.setOption("z3mapping", "uae")
            self.settings.setOption("fastmem_size", "0")
            self.settings.setOption("a3000mem_size", "0")
            self.settings.setOption("mbresmem_size", "0")
            self.settings.setOption("z3mem_size", "0")
            self.settings.setOption("z3mem_start", "0x0")
            self.settings.setOption("bogomem_size", "0")
            self.settings.setOption("gfxcard_hardware_vblank", "false")
            self.settings.setOption("gfxcard_hardware_sprite", "false")
            self.settings.setOption("gfxcard_multithread", "false")
            self.settings.setOption("rtg_modes", "0x112")
        elif subsystem == SubSystems.A1200:
            self.settings.setOption("chipmem_size", "4")
            self.settings.setOption("z3mapping", "real")
            self.settings.setOption("fastmem_size", "4")
            self.settings.setOption("a3000mem_size", "0")
            self.settings.setOption("mbresmem_size", "0")
            self.settings.setOption("z3mem_size", "0")
            self.settings.setOption("z3mem_start", "0x40000000")
            self.settings.setOption("bogomem_size", "0")
            self.settings.setOption("gfxcard_hardware_vblank", "false")
            self.settings.setOption("gfxcard_hardware_sprite", "false")
            self.settings.setOption("gfxcard_multithread", "false")
            self.settings.setOption("rtg_modes", "0x112")
        elif subsystem == SubSystems.CD32:
            self.settings.setOption("chipmem_size", "8")
            self.settings.setOption("z3mapping", "real")
            self.settings.setOption("fastmem_size", "0")
            self.settings.setOption("a3000mem_size", "0")
            self.settings.setOption("mbresmem_size", "0")
            self.settings.setOption("z3mem_size", "0")
            self.settings.setOption("z3mem_start", "0x40000000")
            self.settings.setOption("bogomem_size", "0")
            self.settings.setOption("gfxcard_hardware_vblank", "false")
            self.settings.setOption("gfxcard_hardware_sprite", "false")
            self.settings.setOption("gfxcard_multithread", "false")
            self.settings.setOption("rtg_modes", "0x112")
        elif subsystem == SubSystems.CDTV:
            self.settings.setOption("chipmem_size", "2")
            self.settings.setOption("z3mapping", "real")
            self.settings.setOption("fastmem_size", "0")
            self.settings.setOption("a3000mem_size", "0")
            self.settings.setOption("mbresmem_size", "0")
            self.settings.setOption("z3mem_size", "0")
            self.settings.setOption("z3mem_start", "0x40000000")
            self.settings.setOption("bogomem_size", "0")
            self.settings.setOption("gfxcard_hardware_vblank", "false")
            self.settings.setOption("gfxcard_hardware_sprite", "false")
            self.settings.setOption("gfxcard_multithread", "false")
            self.settings.setOption("rtg_modes", "0x112")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetSound(self, subsystem: SubSystems):
        if subsystem == SubSystems.A600:
            self.settings.setOption("sound_output", "exact")
            self.settings.setOption("sound_channels", "stereo")
            self.settings.setOption("sound_stereo_separation", "7")
            self.settings.setOption("sound_stereo_mixing_delay", "0")
            self.settings.setOption("sound_frequency", "44100")
            self.settings.setOption("sound_interpol", "none")
            self.settings.setOption("sound_filter", "off")
            self.settings.setOption("sound_filter_type", "standard")
            self.settings.setOption("sound_volume_cd", "0")
        elif subsystem == SubSystems.A1200:
            self.settings.setOption("sound_output", "exact")
            self.settings.setOption("sound_channels", "stereo")
            self.settings.setOption("sound_stereo_separation", "7")
            self.settings.setOption("sound_stereo_mixing_delay", "0")
            self.settings.setOption("sound_frequency", "44100")
            self.settings.setOption("sound_interpol", "none")
            self.settings.setOption("sound_filter", "off")
            self.settings.setOption("sound_filter_type", "enhanced")
            self.settings.setOption("sound_volume_cd", "0")
        elif subsystem == SubSystems.CD32:
            self.settings.setOption("sound_output", "normal")
            self.settings.setOption("sound_channels", "stereo")
            self.settings.setOption("sound_stereo_separation", "7")
            self.settings.setOption("sound_stereo_mixing_delay", "0")
            self.settings.setOption("sound_frequency", "44100")
            self.settings.setOption("sound_interpol", "none")
            self.settings.setOption("sound_filter", "off")
            self.settings.setOption("sound_filter_type", "enhanced")
            self.settings.setOption("sound_volume_cd", "0")
        elif subsystem == SubSystems.CDTV:
            self.settings.setOption("sound_output", "exact")
            self.settings.setOption("sound_channels", "stereo")
            self.settings.setOption("sound_stereo_separation", "7")
            self.settings.setOption("sound_stereo_mixing_delay", "0")
            self.settings.setOption("sound_frequency", "44100")
            self.settings.setOption("sound_interpol", "none")
            self.settings.setOption("sound_filter", "off")
            self.settings.setOption("sound_filter_type", "standard")
            self.settings.setOption("sound_volume_cd", "0")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetGraphics(self, subsystem: SubSystems):
        if subsystem == SubSystems.A600:
            self.settings.setOption("gfx_framerate", "0")
            self.settings.setOption("gfx_width", "640")
            self.settings.setOption("gfx_height", "256")
            self.settings.setOption("gfx_refreshrate", "50")
            self.settings.setOption("gfx_refreshrate_rtg", "50")
            self.settings.setOption("gfx_lores", "false")
            self.settings.setOption("gfx_resolution", "hires")
            self.settings.setOption("gfx_lores_mode", "normal")
            self.settings.setOption("gfx_linemode", "none")
            self.settings.setOption("gfx_fullscreen_amiga", "false")
            self.settings.setOption("gfx_fullscreen_picasso", "false")
            self.settings.setOption("ntsc", "false")
        elif subsystem == SubSystems.A1200:
            self.settings.setOption("gfx_framerate", "0")
            self.settings.setOption("gfx_width", "640")
            self.settings.setOption("gfx_height", "256")
            self.settings.setOption("gfx_refreshrate", "50")
            self.settings.setOption("gfx_refreshrate_rtg", "50")
            self.settings.setOption("gfx_lores", "false")
            self.settings.setOption("gfx_resolution", "hires")
            self.settings.setOption("gfx_lores_mode", "normal")
            self.settings.setOption("gfx_linemode", "none")
            self.settings.setOption("gfx_fullscreen_amiga", "false")
            self.settings.setOption("gfx_fullscreen_picasso", "false")
            self.settings.setOption("ntsc", "false")
        elif subsystem == SubSystems.CD32:
            self.settings.setOption("gfx_framerate", "0")
            self.settings.setOption("gfx_width", "768")
            self.settings.setOption("gfx_height", "270")
            self.settings.setOption("gfx_refreshrate", "50")
            self.settings.setOption("gfx_refreshrate_rtg", "50")
            self.settings.setOption("gfx_lores", "false")
            self.settings.setOption("gfx_resolution", "hires")
            self.settings.setOption("gfx_lores_mode", "normal")
            self.settings.setOption("gfx_linemode", "none")
            self.settings.setOption("gfx_fullscreen_amiga", "false")
            self.settings.setOption("gfx_fullscreen_picasso", "false")
            self.settings.setOption("ntsc", "false")
        elif subsystem == SubSystems.CDTV:
            self.settings.setOption("gfx_framerate", "0")
            self.settings.setOption("gfx_width", "768")
            self.settings.setOption("gfx_height", "270")
            self.settings.setOption("gfx_refreshrate", "50")
            self.settings.setOption("gfx_refreshrate_rtg", "50")
            self.settings.setOption("gfx_lores", "false")
            self.settings.setOption("gfx_resolution", "hires")
            self.settings.setOption("gfx_lores_mode", "normal")
            self.settings.setOption("gfx_linemode", "none")
            self.settings.setOption("gfx_fullscreen_amiga", "false")
            self.settings.setOption("gfx_fullscreen_picasso", "false")
            self.settings.setOption("ntsc", "false")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetNetwork(self, network: bool):
        self.settings.setOption("bsdsocket_emu", "true" if network else "false")

    def SetFloppies(self, subsystem: SubSystems, floppyPathList: List[str]):
        if subsystem in SubSystems.COMPUTERS:
            self.settings.setOption("floppy_speed", "100")
            count = min(len(floppyPathList), 4)
            self.settings.setOption("nr_floppies", str(count))
            if count > 0:
                self.settings.setOption("floppy0", floppyPathList[0])
                if count > 1:
                    self.settings.setOption("floppy1", floppyPathList[1])
                    if count > 2:
                        self.settings.setOption("floppy2", floppyPathList[2])
                        self.settings.setOption("floppy2type", "0")
                        if count > 3:
                            self.settings.setOption("floppy3", floppyPathList[3])
                            self.settings.setOption("floppy3type", "0")
        elif subsystem in SubSystems.CONSOLES:
            # No drives on CD32/CDTV
            return
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetCD(self, subsystem: SubSystems, cdpath: str):
        self.settings.setOption("cd_speed", "100")
        if subsystem in SubSystems.COMPUTERS:
            # No CD
            return
        elif subsystem == SubSystems.CD32:
            self.settings.setOption("cd32cd", "true")
            self.settings.setOption("cd32c2p", "true")
            self.settings.setOption("cd32nvram", "true")
            if cdpath:
                self.settings.setOption("cdimage0", cdpath + ",image")
        elif subsystem == SubSystems.CDTV:
            if cdpath:
                self.settings.setOption("cdimage0", cdpath + ",image")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetHDDFS(self, subsystem: SubSystems, hdd0mointpoint: str):
        if subsystem in SubSystems.COMPUTERS:
            volume, _ = os.path.splitext(os.path.basename(hdd0mointpoint))
            self.settings.setOption("filesystem2", "rw,DH0:{}:{},0".format(volume, hdd0mointpoint))
            self.settings.setOption("uaehf0", "dir,rw,DH0:{}:{},0".format(volume, hdd0mointpoint))
        elif subsystem in SubSystems.CONSOLES:
            # No HDDFS on CD32/CDTV
            return
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetHDF(self, subsystem: SubSystems, hdf: str):
        if subsystem in SubSystems.COMPUTERS:
            self.settings.setOption("filesystem2", "rw,DH0:{},32,1,2,512,0,,ide0_mainboard".format(hdf))
            self.settings.setOption("uaehf0", "hdf,rw,DH0:{},32,1,2,512,0,,ide0_mainboard".format(hdf))
        elif subsystem in SubSystems.CONSOLES:
            # No HDF on CD32/CDTV
            return
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetKickstarts(self, subsystem: SubSystems, romtype: RomType):
        manager = KickstartManager()
        manager.GetKickstartsFor(subsystem, romtype)
        kickstart = manager.GetBIOS
        self.settings.setOption("kickstart_rom_file", kickstart)
        if manager.NeedExtendedBIOS:
            extended = manager.GetExtendedBIOS
            self.settings.setOption("kickstart_ext_rom_file", extended)



