#!/usr/bin/env python
from typing import List

from configgen.generators.amiberry.amiberrySubSystems import SubSystems
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.amiberry.amiberryRomType import RomType


class ConfigGenerator:

    def __init__(self, targetFileName: str):
        self.settingsFileName: str = targetFileName
        from configgen.settings.keyValueSettings import keyValueSettings
        self.settings = keyValueSettings("").defineBool('true', 'false')

    def loadConfigFile(self, filename: str):
        self.settings.changeSettingsFile(filename)
        self.settings.loadFile(False)

    def saveConfigFile(self):
        self.settings.changeSettingsFile(self.settingsFileName)
        self.settings.saveFile()

    def SetDefaults(self, subsystem):
        self.settings.setBool("whdload_showsplash",False)
        self.settings.setInt("whdload_configdelay", 0)
        self.settings.setBool("synchronize_clock", True)

    def SetDefaultPath(self, subsystem: SubSystems):
        import configgen.recalboxFiles as recalboxFiles
        import os
        romsPath = os.path.join(recalboxFiles.ROMS, str(subsystem))
        self.settings.setString("config_description", "Recalbox auto-generated configuration for Amiga {}".format(subsystem))
        self.settings.setBool("config_hardware", True)
        self.settings.setBool("config_host", True)
        self.settings.setString("config_version", "4.1.5")
        self.settings.setString("amiberry.rom_path", romsPath)
        self.settings.setString("amiberry.floppy_path", romsPath)
        self.settings.setString("amiberry.hardfile_path", romsPath)
        self.settings.setString("amiberry.cd_path", romsPath)
        if subsystem == SubSystems.CD32:
            self.settings.setString("flash_file", os.path.join(recalboxFiles.SAVES, "{}/cd32.nvr".format(subsystem)))
        if subsystem == SubSystems.CDTV:
            self.settings.setString("flash_file", os.path.join(recalboxFiles.SAVES, "{}/cdtv.nvr".format(subsystem)))
        if subsystem in SubSystems.COMPUTERS:
            self.settings.setString("pcmcia_mb_rom_file", ":ENABLED")
            self.settings.setString("ide_mb_rom_file", ":ENABLED")

    def SetUI(self, keyboardLayout: str, showfps: bool):
        self.settings.setBool("use_gui", False)
        self.settings.setBool("show_leds", showfps)
        # Valid for amiberry?
        if keyboardLayout not in ["us", "fr", "de", "dk", "es", "se", "it"]:
            keyboardLayout = "us"
        self.settings.setString("kbd_lang", keyboardLayout)

    def SetInput(self, subsystem: SubSystems):
        if subsystem in SubSystems.HAVEMOUSE:
            self.settings.setInt("input.joymouse_speed_analog", 2)
            self.settings.setInt("input.joymouse_speed_digital", 10)
            self.settings.setInt("input.joymouse_deadzone", 33)
            self.settings.setInt("input.mouse_speed", 100)
        self.settings.setInt("input.joystick_deadzone", 33)
        self.settings.setInt("input.analog_joystick_multiplier", 15)
        self.settings.setInt("input.analog_joystick_offset", -1)
        self.settings.setInt("input.autofire_speed", 0)
        
    def SetJoystick(self, subsystem: SubSystems, controllers: ControllerPerPlayer):
        self.settings.setBool("amiberry.use_analogue_remap", False)
        self.settings.setBool("amiberry.use_retroarch_quit", True)
        self.settings.setBool("amiberry.use_retroarch_menu", True)
        self.settings.setBool("amiberry.use_retroarch_reset", True)
        port: int = 0
        if subsystem in SubSystems.HAVEMOUSE:
            # Set mouse
            self.settings.setString("joyport0", "mouse")
            self.settings.setString("joyport0autofire", "none")
            self.settings.setString("joyport0mode", "mousenowheel")
            self.settings.setString("joyport0mousemap", "right")
            self.settings.setString("joyportfriendlyname0", "Mouse")
            self.settings.setString("joyportname0", "MOUSE0")
            port += 1
        # Set joysticks
        for _, controller in controllers.items():
            indexPadSDL = controller.SdlIndex
            if 1 <= controller.PlayerIndex <= 3:
                self.settings.setString("joyport{}".format(port), "joy{}".format(indexPadSDL))
                self.settings.setString("joyport{}autofire".format(port), "none")
                self.settings.setString("joyport{}mode".format(port), "cd32joy" if subsystem == SubSystems.CD32 else "djoy")
                self.settings.setString("joyport{}mousemap".format(port), "right")
                self.settings.setString("joyportfriendlyname{}".format(port), controller.DeviceName)
                self.settings.setString("joyportname{}".format(port), "JOY{}".format(indexPadSDL))
                from configgen.generators.amiberry.amiberryRetroarchConfig import AmiberryRetroarchConfig
                retroarchConfig = AmiberryRetroarchConfig(controller)
                retroarchConfig.generateConfiguration()
                retroarchConfig.saveConfigurationFile()
                port += 1

    def SetCPU(self, subsystem: SubSystems, needSlowCPU: bool):
        self.settings.setBool("cpu_24bit_addressing", True)
        self.settings.setBool("fpu_no_unimplemented", True)
        self.settings.setInt("cachesize", 0)
        self.settings.setBool("fpu_strict", False)
        self.settings.setBool("compfpu", False)
        if subsystem == SubSystems.A600:
            self.settings.setString("cpu_speed", "real")
            self.settings.setInt("cpu_type", 68000)
            self.settings.setInt("cpu_model", 68000)
            self.settings.setBool("cpu_compatible", True)
        elif subsystem == SubSystems.A1200:
            # Tweak the CPU to workaround Amiberry's loader bug on 68000/68010 CPUs
            # We run a 1200 slowed down to a normal 68000
            # https://github.com/midwan/amiberry/issues/417
            if needSlowCPU: self.settings.setString("cpu_speed", "real")
            else:           self.settings.setString("finegrain_cpu_speed", "1024")
            self.settings.setString("cpu_type", "68ec020")
            self.settings.setInt("cpu_model", 68020)
            self.settings.setBool("cpu_compatible", False)
        elif subsystem == SubSystems.CD32:
            self.settings.setString("finegrain_cpu_speed", "1024")
            self.settings.setString("cpu_type", "68ec020")
            self.settings.setInt("cpu_model", 68020)
            self.settings.setBool("cpu_compatible", False)
        elif subsystem == SubSystems.CDTV:
            self.settings.setString("cpu_speed", "real")
            self.settings.setInt("cpu_type", 68000)
            self.settings.setInt("cpu_model", 68000)
            self.settings.setBool("cpu_compatible", True)
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetChipset(self, subsystem: SubSystems):
        self.settings.setString("chipset_refreshrate", "50.000000")
        self.settings.setString("collision_level", "playfields")
        self.settings.setBool("immediate_blits", False)
        self.settings.setString("waiting_blits", "automatic")
        self.settings.setBool("fast_copper", False)
        if subsystem == SubSystems.A600:
            self.settings.setString("chipset", "ecs")
            self.settings.setString("chipset_compatible", "A600")
            self.settings.setString("rtc", "none")
            self.settings.setBool("ksmirror_a8", True)
            self.settings.setBool("pcmcia", True)
            self.settings.setBool("cia_todbug", True)
        elif subsystem == SubSystems.A1200:
            self.settings.setString("chipset", "aga")
            self.settings.setString("chipset_compatible", "A1200")
            self.settings.setString("rtc", "MSM6242B")
            self.settings.setBool("ksmirror_a8", True)
            self.settings.setBool("pcmcia", True)
        elif subsystem == SubSystems.CD32:
            self.settings.setString("chipset", "aga")
            self.settings.setString("chipset_compatible", "CD32")
            self.settings.setString("rtc", "none")
            self.settings.setBool("ksmirror_e0", False)
            self.settings.setBool("ksmirror_a8", True)
            self.settings.setBool("cd32cd", True)
            self.settings.setBool("cd32c2p", True)
            self.settings.setBool("cd32nvram", True)
        elif subsystem == SubSystems.CDTV:
            self.settings.setString("chipset", "ecs_agnus")
            self.settings.setString("chipset_compatible", "CDTV")
            self.settings.setString("rtc", "MSM6242B")
            self.settings.setBool("ksmirror_e0", False)
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetMemory(self, subsystem: SubSystems):
        self.settings.setInt("a3000mem_size", 0)
        self.settings.setInt("mbresmem_size", 0)
        self.settings.setInt("z3mem_size", 0)
        self.settings.setInt("bogomem_size", 0)
        self.settings.setBool("gfxcard_hardware_vblank", False)
        self.settings.setBool("gfxcard_hardware_sprite", False)
        self.settings.setBool("gfxcard_multithread", False)
        self.settings.setString("rtg_modes", "0x112")
        if subsystem == SubSystems.A600:
            self.settings.setInt("chipmem_size", 2)
            self.settings.setString("z3mapping", "uae")
            self.settings.setInt("fastmem_size", 0)
            self.settings.setString("z3mem_start", "0x0")
        elif subsystem == SubSystems.A1200:
            self.settings.setInt("chipmem_size", 4)
            self.settings.setString("z3mapping", "real")
            self.settings.setInt("fastmem_size", 4)
            self.settings.setString("z3mem_start", "0x40000000")
        elif subsystem == SubSystems.CD32:
            self.settings.setInt("chipmem_size", 8)
            self.settings.setString("z3mapping", "real")
            self.settings.setInt("fastmem_size", 0)
            self.settings.setString("z3mem_start", "0x40000000")
        elif subsystem == SubSystems.CDTV:
            self.settings.setInt("chipmem_size", 2)
            self.settings.setString("z3mapping", "real")
            self.settings.setInt("fastmem_size", 0)
            self.settings.setString("z3mem_start", "0x40000000")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetSound(self, subsystem: SubSystems):
        self.settings.setString("sound_channels", "stereo")
        self.settings.setInt("sound_stereo_separation", 7)
        self.settings.setInt("sound_stereo_mixing_delay", 0)
        self.settings.setInt("sound_frequency", 44100)
        self.settings.setString("sound_interpol", "none")
        self.settings.setString("sound_filter", "off")
        self.settings.setInt("sound_volume_cd", 0)
        if subsystem == SubSystems.A600:
            self.settings.setString("sound_output", "exact")
            self.settings.setString("sound_filter_type", "standard")
        elif subsystem == SubSystems.A1200:
            self.settings.setString("sound_output", "exact")
            self.settings.setString("sound_filter_type", "enhanced")
        elif subsystem == SubSystems.CD32:
            self.settings.setString("sound_output", "normal")
            self.settings.setString("sound_filter_type", "enhanced")
        elif subsystem == SubSystems.CDTV:
            self.settings.setString("sound_output", "exact")
            self.settings.setString("sound_filter_type", "standard")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetGraphics(self, subsystem: SubSystems):
        self.settings.setBool("amiberry.gfx_auto_height", False)
        self.settings.setBool("gfx_fullscreen_amiga", True)
        self.settings.setBool("gfx_fullscreen_picasso", True)
        self.settings.setInt("gfx_width_fullscreen", 1280)
        self.settings.setInt("gfx_height_fullscreen", 720)
        self.settings.setInt("gfx_framerate", 0)
        self.settings.setInt("gfx_refreshrate", 50)
        self.settings.setInt("gfx_refreshrate_rtg", 50)
        self.settings.setBool("gfx_lores", False)
        self.settings.setString("gfx_resolution", "hires")
        self.settings.setString("gfx_lores_mode", "normal")
        self.settings.setString("gfx_linemode", "none")
        self.settings.setBool("ntsc", False)
        if subsystem == SubSystems.A600:
            self.settings.setInt("gfx_width", 640)
            self.settings.setInt("gfx_height", 512)
        elif subsystem == SubSystems.A1200:
            self.settings.setInt("gfx_width", 640)
            self.settings.setInt("gfx_height", 512)
        elif subsystem == SubSystems.CD32:
            self.settings.setInt("gfx_width", 768)
            self.settings.setInt("gfx_height", 540)
        elif subsystem == SubSystems.CDTV:
            self.settings.setInt("gfx_width", 768)
            self.settings.setInt("gfx_height", 540)
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetNetwork(self, network: bool):
        self.settings.setBool("bsdsocket_emu", network)

    def SetFloppies(self, subsystem: SubSystems, floppyPathList: List[str]):
        if subsystem in SubSystems.COMPUTERS:
            self.settings.setInt("floppy_speed", 100)
            count = min(len(floppyPathList), 4)
            self.settings.setString("nr_floppies", str(count))
            if count > 0:
                self.settings.setString("floppy0", floppyPathList[0])
                if count > 1:
                    self.settings.setString("floppy1", floppyPathList[1])
                    if count > 2:
                        self.settings.setString("floppy2", floppyPathList[2])
                        self.settings.setInt("floppy2type", 0)
                        if count > 3:
                            self.settings.setString("floppy3", floppyPathList[3])
                            self.settings.setInt("floppy3type", 0)
        elif subsystem in SubSystems.CONSOLES:
            # No drives on CD32/CDTV
            return
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetCD(self, subsystem: SubSystems, cdpath: str):
        self.settings.setInt("cd_speed", 100)
        if subsystem in SubSystems.COMPUTERS:
            # No CD
            return
        elif subsystem == SubSystems.CD32:
            self.settings.setBool("cd32cd", True)
            self.settings.setBool("cd32c2p", True)
            self.settings.setBool("cd32nvram", True)
            if cdpath:
                self.settings.setString("cdimage0", cdpath + ",image")
        elif subsystem == SubSystems.CDTV:
            if cdpath:
                self.settings.setString("cdimage0", cdpath + ",image")
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetHDDFS(self, subsystem: SubSystems, hdd0mointpoint: str):
        if subsystem in SubSystems.COMPUTERS:
            import os
            volume, _ = os.path.splitext(os.path.basename(hdd0mointpoint))
            self.settings.setString("filesystem2", "rw,DH0:{}:{},0".format(volume, hdd0mointpoint))
            self.settings.setString("uaehf0", "dir,rw,DH0:{}:{},0".format(volume, hdd0mointpoint))
        elif subsystem in SubSystems.CONSOLES:
            # No HDDFS on CD32/CDTV
            return
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetHDF(self, subsystem: SubSystems, hdf: str):
        if subsystem in SubSystems.COMPUTERS:
            self.settings.setString("filesystem2", "rw,DH0:{},32,1,2,512,0,,ide0_mainboard".format(hdf))
            self.settings.setString("uaehf0", "hdf,rw,DH0:{},32,1,2,512,0,,ide0_mainboard".format(hdf))
        elif subsystem in SubSystems.CONSOLES:
            # No HDF on CD32/CDTV
            return
        else:
            raise Exception("Unknown subsystem " + str(subsystem))

    def SetKickstarts(self, subsystem: SubSystems, romtype: RomType):
        from configgen.generators.amiberry.amiberryKickstarts import KickstartManager
        manager = KickstartManager()
        manager.GetKickstartsFor(subsystem, romtype)
        kickstart = manager.GetBIOS
        self.settings.setString("kickstart_rom_file", kickstart)
        if manager.NeedExtendedBIOS:
            extended = manager.GetExtendedBIOS
            self.settings.setString("kickstart_ext_rom_file", extended)



