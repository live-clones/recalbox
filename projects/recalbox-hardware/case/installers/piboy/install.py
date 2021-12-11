import os
import logger
from filemanipulation import sed, stripline
from installers.base.install import InstallBase
from settings import keyValueSettings

# status flags
# ex: 1100 0110
#     ||    |\-VSTAT1
#     ||    \-VSTAT2
#     |\-power button (0 -> off position, 1 -> on position)
#     \-VBus


class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "piboy/"
    RECALBOX_CONF = "/recalbox/share/system/recalbox.conf"

    def __init__(self):
        InstallBase.__init__(self)

    def InstallHardware(self, case):

        logger.hardlog("Installing PiBoy DMG hardware")

        try:
            os.system("mount -o remount,rw /boot")
            os.system("mount -o remount,rw /")
            # Install /boot/config.txt - most important change first
            sourceConfig = self.BASE_SOURCE_FOLDER + "assets/config.txt"
            os.system("cp /boot/config.txt /boot/config.txt.backup")
            if os.system("cp {} /boot".format(sourceConfig)) != 0:
                logger.hardlog("PiBoy: Error installing config.txt")
                return False
            logger.hardlog("PiBoy: config.txt installed")
            # Install /boot/recalbox-user-config.txt
            sourceConfig = self.BASE_SOURCE_FOLDER + "assets/recalbox-user-config.txt"
            os.system("cp /boot/recalbox-user-config.txt /boot/recalbox-user-config.txt.backup")
            if os.system("cp {} /boot".format(sourceConfig)) != 0:
                logger.hardlog("PiBoy: Error installing recalbox-user-config.txt")
                return False
            logger.hardlog("PiBoy: recalbox-user-config.txt installed")
            # install boot image
            sourcePpm = self.BASE_SOURCE_FOLDER + "assets/piboy.ppm"
            if os.system("cp -r {} /boot/boot.ppm".format(sourcePpm)) == 0:
                logger.hardlog("PiBoy: boot image installed")
            else:
                logger.hardlog("PiBoy: boot image NOT installed")

            sed('noswap', 'noswap video=HDMI-A-1:d', '/boot/cmdline.txt')
            logger.hardlog("PiBoy: set video parameter in cmdline.txt")

            # Install /etc/init.d/S01piboy
            sourceConfig = self.BASE_SOURCE_FOLDER + "assets/S01piboy"
            if os.system("cp {} /etc/init.d/".format(sourceConfig)) != 0:
                logger.hardlog("PiBoy: Error installing S01piboy")
                return ""
            logger.hardlog("PiBoy: S01piboy installed")
            sourceConfig = self.BASE_SOURCE_FOLDER + "assets/piboy-battery-indicator"
            if os.system("cp {} /usr/bin/".format(sourceConfig)) != 0:
                logger.hardlog("PiBoy: Error installing piboy-battery-indicator")
                return ""
            logger.hardlog("PiBoy: piboy-battery-indicator installed")

        except Exception as e:
            logger.hardlog("PiBoy: Exception = {}".format(e))
            return False

        finally:
            # write 129 to flags in order to reboot properly (instead of shutdown)
            with open("/sys/kernel/xpi_gamecon/flags", "w") as xpiflags:
                xpiflags.write("129\n")

        logger.hardlog("PiBoy DMG hardware installed successfully!")
        return True

    def UninstallHardware(self, case):

        try:
            os.system("mount -o remount,rw /boot")
            # Uninstall /boot/config.txt
            if os.system("cp /boot/config.txt.backup /boot/config.txt") != 0:
                logger.hardlog("PiBoy: Error uninstalling config.txt")
                return False
            logger.hardlog("PiBoy: config.txt uninstalled")
            # Uninstall /boot/recalbox-user-config.txt
            if os.system("cp /boot/recalbox-user-config.txt.backup /boot/recalbox-user-config.txt") != 0:
                logger.hardlog("PiBoy: Error uninstalling recalbox-user-config.txt")
                return False
            logger.hardlog("PiBoy: recalbox-user-config.txt uninstalled")
            os.remove("/boot/boot.ppm")
            logger.hardlog("PiBoy: /boot/boot.ppm uninstalled")
            sed(' video=HDMI-A-1:d', '', '/boot/cmdline.txt')
            logger.hardlog("PiBoy: removed video setting in cmdline.txt")
            os.remove("/etc/init.d/S01piboy")
            logger.hardlog("PiBoy: /etc/init.d/S01piboy uninstalled")
            os.remove("/usr/bin/piboy-battery-indicator")
            logger.hardlog("PiBoy: piboy-battery-indicator uninstalled")

        except Exception as e:
            logger.hardlog("PiBoy: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")
            os.system("mount -o remount,ro /")

        return True

    def InstallSoftware(self, case):

        if case == "PiBoy":

            logger.hardlog("Installing PiBoy DMG software")

            try:
                os.system("mount -o remount,rw /")
                # Load recalbox.conf
                recalboxConf = keyValueSettings(self.RECALBOX_CONF, False)
                recalboxConf.loadFile()

                # Set powerswitch.sh config
                recalboxConf.setOption("system.power.switch", "PIBOY")
                logger.hardlog("PiBoy: powerswitch configured")
                # Set wpaf config
                recalboxConf.setOption("hat.wpaf.enabled", "1")
                recalboxConf.setOption("hat.wpaf.board", "piboy")
                logger.hardlog("PiBoy: wpaf configured")
                recalboxConf.setOption("emulationstation.theme.folder", "recalbox-goa2")
                logger.hardlog("PiBoy: theme set to recalbox-goa2")
                recalboxConf.setOption("audio.device", "alsa_card.platform-bcm2835_audio.2:analog-output-headphones")
                logger.hardlog("PiBoy: audio.device set to alsa_card.platform-bcm2835_audio.2:analog-output-headphones")
                recalboxConf.saveFile()
                # Force default videomode
                sed(
                    "([a-zA-Z0-9.].videomode)\\s*=.*",
                    "\\1=default",
                    self.RECALBOX_CONF,
                )
                # Install /etc/init.d/S06volumed
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/S06volumed"
                if os.system("cp {} /etc/init.d/".format(sourceConfig)) != 0:
                    logger.hardlog("PiBoy: Error installing S06volumed")
                    return ""
                logger.hardlog("PiBoy: S06volumed installed")

                # Install /etc/init.d/S15piboydisplay
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/S15piboydisplay"
                if os.system("cp {} /etc/init.d/".format(sourceConfig)) != 0:
                    logger.hardlog("PiBoy: Error installing S15piboydisplay")
                    return ""
                logger.hardlog("PiBoy: S15piboydisplay installed")

                # Install /recalbox/share/.retroarch.cfg
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/piboy-retroarch.cfg"
                if os.system("cp {} /recalbox/share/.retroarch.cfg".format(sourceConfig)) != 0:
                    logger.hardlog("PiBoy: Error installing .retroarch.cfg")
                    return ""
                logger.hardlog("PiBoy: .retroarch.cfg installed")

                # start volumed service for volume wheel to work properly
                os.system("/etc/init.d/S06volumed start")
                # start piboydisplay to manage lcd display and theme
                os.system("/etc/init.d/S15piboydisplay start")

            except Exception as e:
                logger.hardlog("PiBoy: Exception = {}".format(e))
                return ""

            finally:
                os.system("mount -o remount,ro /")

            logger.hardlog("PiBoy DMG software installed successfully!")
            return case

        return ""

    def UninstallSoftware(self, case):

        try:
            os.system("mount -o remount,rw /")
            os.remove("/etc/init.d/S06volumed")
            logger.hardlog("PiBoy: /etc/init.d/S06volumed uninstalled")
            os.remove("/etc/init.d/S15piboydisplay")
            logger.hardlog("PiBoy: /etc/init.d/S15piboydisplay uninstalled")
            os.remove("/recalbox/share/.retroarch.cfg")
            logger.hardlog("PiBoy: /recalbox/share/.retroarch.cfg uninstalled")
            # Load recalbox.conf
            recalboxConf = keyValueSettings(self.RECALBOX_CONF, False)
            recalboxConf.loadFile()

            # Remove powerswitch.sh config
            recalboxConf.removeOption("system.power.switch")
            # Remove wpaf config
            recalboxConf.setOption("hat.wpaf.enabled", "0")
            recalboxConf.removeOption("hat.wpaf.board")
            recalboxConf.saveFile()
            logger.hardlog("PiBoy: powerswitch unconfigured")

        except Exception as e:
            logger.hardlog("PiBoy: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /")

        return True

    def GetInstallScript(self, case):

        return None
