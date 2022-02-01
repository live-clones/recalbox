import os
import logger
from installers.base.install import InstallBase
from settings import keyValueSettings


class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "gpi/"

    RECALBOX_CONF = "/recalbox/share/system/recalbox.conf"

    def __init__(self):
        InstallBase.__init__(self)


    def InstallHardware(self, case):

        if case == "GPiV1":

            logger.hardlog("Installing GPi Case V1 hardware")

            try:
                os.system("mount -o remount,rw /boot")
                # Install /boot/recalbox-user-config.txt - most important change first
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/recalbox-user-config.txt"
                os.system("cp /boot/recalbox-user-config.txt /boot/recalbox-user-config.txt.backup")
                if os.system("cp {} /boot".format(sourceConfig)) != 0:
                    logger.hardlog("GPi: Error installing recalbox-user-config.txt")
                    return False
                logger.hardlog("GPi: recalbox-user-config.txt installed")

                # Remove kms
                if os.system('sed -i "s|dtoverlay=vc4-kms-v3d||g" /boot/config.txt') != 0:
                    logger.hardlog("GPi: Error removing kms driver")
                    return False
                logger.hardlog("GPi: kms driver removed")

                # Install /boot/cmdline.txt, correct sound issue
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/cmdline.txt"
                os.system("cp /boot/cmdline.txt /boot/cmdline.txt.backup")
                if os.system("cp {} /boot".format(sourceConfig)) != 0:
                    logger.hardlog("GPi: Error installing cmdline.txt")
                    return False
                logger.hardlog("GPi: cmdline.txt installed")

                # Install Overlay
                sourceOverlay = self.BASE_SOURCE_FOLDER + "assets/overlays/*.dtbo"
                if os.system("cp -r {} /boot/overlays".format(sourceOverlay)) != 0:
                    logger.hardlog("GPi: Error installing overlays")
                    return False
                logger.hardlog("GPi: overlays installed")

            except Exception as e:
                logger.hardlog("GPi: Exception = {}".format(e))
                return False

            finally:
                os.system("mount -o remount,ro /")
                os.system("mount -o remount,ro /boot")

            logger.hardlog("GPiV1 hardware installed successfully!")
            return True

        # Unidentified case
        return False


    def InstallSoftware(self, case):

        if case == "GPiV1":

            board: str = "unknown"
            with open("/recalbox/recalbox.arch", "r") as sf:
                board = sf.readline()
            if board not in ("rpi0", "rpi1", "rpizero2legacy"):
                os.system("fbv2 -i -k /recalbox/system/resources/splash/gpierror.png")
                while True: pass  # Infinite loop

            logger.hardlog("Installing GPi Case V1 software")

            try:
                os.system("mount -o remount,rw /")

                # Load recalbox.conf
                recalboxConf = keyValueSettings(self.RECALBOX_CONF, False)
                recalboxConf.loadFile()

                # Switch default resolution
                recalboxConf.setOptionOnMatching("[a-z\.]*\.videomode", "default")
                recalboxConf.saveFile()
                logger.hardlog("GPi: set default resolution")

                # Set new hostname
                recalboxConf.setOption("system.hostname", "RECALBOXGPI")
                recalboxConf.saveFile()
                logger.hardlog("GPi: set RECALBOXGPI hostname")

                # Disable kodi
                recalboxConf.setOption("kodi.enabled", "0")
                recalboxConf.setOption("kodi.xbutton", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: KODI disabled")

                # Disable virtual gamepad
                recalboxConf.setOption("system.virtual-gamepads.enabled", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: Virtual Gamepad disabled")

                # Disable updates
                recalboxConf.setOption("updates.enabled", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: Updates disabled")

                # Disable netplay
                recalboxConf.setOption("global.netplay", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: Netplay disabled")

                # Disable webmanager
                recalboxConf.setOption("system.manager.enabled", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: Webmanager disabled")

                # Disable other controllers
                recalboxConf.setOption("controllers.xarcade.enabled", "0")
                recalboxConf.setOption("controllers.bluetooth.enabled", "0")
                recalboxConf.setOption("controllers.ps3.enabled", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: Other controllers disabled")

                # Disable music popups
                recalboxConf.setOption("emulationstation.popup.music", "0")
                recalboxConf.saveFile()
                logger.hardlog("GPi: Music popup disabled")

                # Install GPi XBOX360 config
                os.system("rm -f /recalbox/share/system/.emulationstation/es_input.cfg.org")
                os.system("mv /recalbox/share/system/.emulationstation/es_input.cfg /recalbox/share/system/.emulationstation/es_input.cfg.org")
                os.system("mv " + self.BASE_SOURCE_FOLDER + "assets/es_input.fragment.xml /recalbox/share/system/.emulationstation/es_input.cfg")
                logger.hardlog("GPi: controller updated")

                # Install Theme
                sourceTheme = self.BASE_SOURCE_FOLDER + "assets/theme/recalbox-gpicase"
                if os.system("cp -r {} /recalbox/share/themes/".format(sourceTheme)) == 0:
                    # Switch Theme
                    recalboxConf.setOption("emulationstation.theme.folder", "recalbox-gpicase")
                    recalboxConf.saveFile()
                    logger.hardlog("GPi: GPi-case theme installed")
                else:
                    logger.hardlog("GPi: GPi-case theme NOT installed")

                # install boot image
                sourcePpm = self.BASE_SOURCE_FOLDER + "assets/gpi.ppm"
                if os.system("cp -r {} /boot/boot.ppm".format(sourcePpm)) == 0:
                    logger.hardlog("GPi: GPi-case boot image installed")
                else:
                    logger.hardlog("GPi: GPi-case boot image NOT installed")

                # Copy sound configuration
                recalboxConf.setOption("audio.volume", "90")
                recalboxConf.saveFile()
                soundConfiguration = self.BASE_SOURCE_FOLDER + "assets/asound.conf"
                if os.system("cp {} /etc/".format(soundConfiguration)) == 0:
                    logger.hardlog("GPi: Sound configuration installed")
                else:
                    logger.hardlog("GPi: Sound configuration NOT installed")

                # Fine tune retroarch
                retroarchOriginOverrides =\
                {
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
                logger.hardlog("GPi: Retroarch gui set to RGUI")

            except Exception as e:
                logger.hardlog("GPi: Exception = {}".format(e))
                return ""

            finally:
                os.system("mount -o remount,ro /")
                os.system("mount -o remount,ro /boot")

            logger.hardlog("GPiV1 software installed successfully!")
            return case

        # Unidentified case
        return ""


    def UninstallHardware(self, case):

        try:
            os.system("mount -o remount,rw /boot")
            # Uninstall /boot/recalbox-user-config.txt
            if os.system("cp /boot/recalbox-user-config.txt.backup /boot/recalbox-user-config.txt") != 0:
                logger.hardlog("GPi: Error uninstalling recalbox-user-config.txt")
                return False
            logger.hardlog("GPi: recalbox-user-config.txt uninstalled")

            # Uninstall /boot/cmdline.txt
            if os.system("cp /boot/cmdline.txt.backup /boot/cmdline.txt") != 0:
                logger.hardlog("GPi: Error uninstalling cmdline.txt")
                return False
            logger.hardlog("GPi: cmdline.txt uninstalled")

        except Exception as e:
            logger.hardlog("GPi: Exception = {}".format(e))
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
            logger.hardlog("GPi: GPi-case theme uninstalled")

            # Remove boot image
            os.system("rm -f /boot/boot.ppm")
            logger.hardlog("GPi: GPi-case boot image uninstalled")

            # Uninstall GPi pad
            os.system("rm -f /recalbox/share/system/.emulationstation/es_input.cfg.gpi")
            os.system("mv /recalbox/share/system/.emulationstation/es_input.cfg /recalbox/share/system/.emulationstation/es_input.cfg.gpi")
            os.system("mv /recalbox/share/system/.emulationstation/es_input.cfg.org /recalbox/share/system/.emulationstation/es_input.cfg")
            logger.hardlog("GPi: Controller uninstalled")

            # Re-enable kodi
            recalboxConf.setOption("kodi.enabled", "1")
            recalboxConf.setOption("kodi.xbutton", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi: KODI enabled")

            # Re-enable virtual gamepad
            recalboxConf.setOption("system.virtual-gamepads.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Virtual Gamepad enabled")

            # Re-enable updates
            recalboxConf.setOption("updates.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Updates enabled")

            # Re-enable netplay
            recalboxConf.setOption("netplay.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Netplay enabled")

            # Re-enable webmanager
            recalboxConf.setOption("system.manager.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Webmanager enabled")

            # Re-enable other controller
            recalboxConf.setOption("controllers.xarcade.enabled", "1")
            recalboxConf.setOption("controllers.bluetooth.enabled", "1")
            recalboxConf.setOption("controllers.ps3.enabled", "1")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Other controllers enabled")

            # Reset hostname
            recalboxConf.setOption("system.hostname", "RECALBOX")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Reset RECALBOX hostname")

            # Enable music popups
            recalboxConf.setOption("emulationstation.popup.music", "10")
            recalboxConf.saveFile()
            logger.hardlog("GPi: Music popup enabled")

        except Exception as e:
            logger.hardlog("GPi: Exception = {}".format(e))
            return case

        return ""


    def GetInstallScript(self, case):

        return self.BASE_SOURCE_FOLDER + "assets/phaseInstaller.sh"
