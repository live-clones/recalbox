import os
import logger
import shutil
from filemanipulation import sed, stripline
from installers.base.install import InstallBase
from settings import keyValueSettings


class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "gpi2w/"
    RECALBOX_CONF = "/recalbox/share/system/recalbox.conf"

    def __init__(self):
        InstallBase.__init__(self)

    def InstallHardware(self, case):

        logger.hardlog("Installing GPi Case 2W hardware")

        try:
            os.system("mount -o remount,rw /boot")
            files = {
                '/boot/recalbox-user-config.txt': '/boot/recalbox-user-config.txt.backup',
                self.BASE_SOURCE_FOLDER + 'assets/recalbox-user-config.txt': '/boot/recalbox-user-config.txt',
                self.BASE_SOURCE_FOLDER + 'assets/gpi2w.ppm': '/boot/boot.ppm',
            }
            for source_file, dest_file in files.items():
                installed_file = shutil.copy(source_file, dest_file)
                logger.hardlog(f"GPi2W: {installed_file} installed")

            sed(' *snd_bcm2835.enable_compat_alsa=1.*', '', '/boot/cmdline.txt')
            sed('noswap', 'noswap snd_bcm2835.enable_compat_alsa=1 snd_bcm2835.enable_hdmi=0', '/boot/cmdline.txt')
            logger.hardlog("GPi2W: set audio parameters in cmdline.txt")

        except Exception as e:
            logger.hardlog("GPi2W: Exception = {}".format(e))
            return False

        logger.hardlog("RetroFlag GPi CASE 2W hardware installed successfully!")
        return True

    def InstallSoftware(self, case):

        logger.hardlog("Installing GPi Case 2W software")

        try:
            os.system("mount -o remount,rw /")

            # Load recalbox.conf
            recalboxConf = keyValueSettings(self.RECALBOX_CONF, False)
            recalboxConf.loadFile()

            # Switch default resolution
            recalboxConf.setOptionOnMatching("[a-z\.]*\.videomode", "default")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: set default resolution")

            # Set new hostname
            recalboxConf.setOption("system.hostname", "RECALBOXGPI2W")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: set RECALBOXGPI2W hostname")

            # Disable kodi
            recalboxConf.setOption("kodi.enabled", "0")
            recalboxConf.setOption("kodi.xbutton", "0")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: KODI disabled")

            # Disable virtual gamepad
            recalboxConf.setOption("system.virtual-gamepads.enabled", "0")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: Virtual Gamepad disabled")

            # Disable other controllers
            recalboxConf.setOption("controllers.xarcade.enabled", "0")
            recalboxConf.setOption("controllers.bluetooth.enabled", "0")
            recalboxConf.setOption("controllers.ps3.enabled", "0")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: Other controllers disabled")

            # Install GPi XBOX360 config
            files = {
                '/recalbox/share/system/.emulationstation/es_input.cfg': '/recalbox/share/system/.emulationstation/es_input.cfg.org',
                self.BASE_SOURCE_FOLDER + 'assets/es_input.fragment.xml': '/recalbox/share/system/.emulationstation/es_input.cfg',
            }
            for source_file, dest_file in files.items():
                installed_file = shutil.copy(source_file, dest_file)
                logger.hardlog(f"GPi2W: {installed_file} installed")

            # Switch Theme
            recalboxConf.setOption("emulationstation.theme.folder", "recalbox-240p")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: GPi-case 240p theme installed")

            # Fine tune retroarch
            retroarchOriginOverrides = {
                "menu_driver": "rgui",
                "menu_rgui_full_width_layout": "true",
                "menu_rgui_shadows": "true",
                "rgui_aspect_ratio": "0",
                "rgui_aspect_ratio_lock": "1",
                "rgui_background_filler_thickness_enable": "false",
                "rgui_border_filler_enable": "true",
                "rgui_border_filler_thickness_enable": "false",
                "rgui_browser_directory": "default",
                "rgui_config_directory": "~/.config/retroarch/config",
                "rgui_extended_ascii": "true",
                "rgui_inline_thumbnails": "true",
                "rgui_internal_upscale_level": "1",
                "rgui_menu_color_theme": "25",
                "rgui_menu_theme_preset": "",
                "rgui_particle_effect": "4",
                "rgui_particle_effect_speed": "1.000000",
                "rgui_show_start_screen": "false",
                "rgui_swap_thumbnails": "false",
                "rgui_thumbnail_delay": "0",
                "rgui_thumbnail_downscaler": "0",
                "video_font_size": "18",
                "menu_widget_scale_auto": "false",
                "menu_widget_scale_factor": "2.800000",
                "menu_widget_scale_factor_windowed": "2.800000",
            }
            for fileName in ("/recalbox/share/system/configs/retroarch/retroarchcustom.cfg.origin",
                             "/recalbox/share/system/configs/retroarch/retroarchcustom.cfg"):
                settings = keyValueSettings(fileName, True)
                settings.loadFile(True)
                for k, v in retroarchOriginOverrides.items():
                    settings.setOption(k, v)
                settings.saveFile()
            logger.hardlog("GPi2W: Retroarch gui set to RGUI")

        except Exception as e:
            logger.hardlog("GPi2W: Exception = {}".format(e))
            return ""

        finally:
            os.system("mount -o remount,ro /")

        logger.hardlog("GPi2W software installed successfully!")
        return case

    def UninstallHardware(self, case):

        try:
            os.system("mount -o remount,rw /boot")
            # Uninstall /boot/recalbox-user-config.txt
            if os.system("cp /boot/recalbox-user-config.txt.backup /boot/recalbox-user-config.txt") != 0:
                logger.hardlog("GPi2W: Error uninstalling recalbox-user-config.txt")
                return False
            logger.hardlog("GPi2W: recalbox-user-config.txt uninstalled")

            # Uninstall /boot/cmdline.txt
            if os.system("cp /boot/cmdline.txt.backup /boot/cmdline.txt") != 0:
                logger.hardlog("GPi2W: Error uninstalling cmdline.txt")
                return False
            logger.hardlog("GPi2W: cmdline.txt uninstalled")

        except Exception as e:
            logger.hardlog("GPi2W: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")

        return True

    def UninstallSoftware(self, case):

        try:
            # Load recalbox.conf
            recalboxConf = keyValueSettings(self.RECALBOX_CONF, False)
            recalboxConf.loadFile()

            # Switch back to default theme
            recalboxConf.setOption("emulationstation.theme.folder", "recalbox-next")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: GPi-case theme uninstalled")

            # Remove boot image
            os.system("rm -f /boot/boot.ppm")
            logger.hardlog("GPi2W: GPi-case boot image uninstalled")

            # Uninstall GPi pad
            files = {
                '/recalbox/share/system/.emulationstation/es_input.cfg.org': '/recalbox/share/system/.emulationstation/es_input.cfg',
            }
            for source_file, dest_file in files.items():
                installed_file = shutil.copy(source_file, dest_file)
                logger.hardlog(f"GPi2W: {installed_file} installed")
            logger.hardlog("GPi2W: Controller uninstalled")

            # Re-enable kodi
            recalboxConf.setOption("kodi.enabled", "1")
            recalboxConf.setOption("kodi.xbutton", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: KODI enabled")

            # Re-enable virtual gamepad
            recalboxConf.setOption("system.virtual-gamepads.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: Virtual Gamepad enabled")

            # Re-enable other controller
            recalboxConf.setOption("controllers.xarcade.enabled", "1")
            recalboxConf.setOption("controllers.bluetooth.enabled", "1")
            recalboxConf.setOption("controllers.ps3.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: Other controllers enabled")

            # Reset hostname
            recalboxConf.setOption("system.hostname", "RECALBOX")
            recalboxConf.saveFile()
            logger.hardlog("GPi2W: Reset RECALBOX hostname")

        except Exception as e:
            logger.hardlog("GPi2W: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /")

        return True

    def GetInstallScript(self, case):

        return None
