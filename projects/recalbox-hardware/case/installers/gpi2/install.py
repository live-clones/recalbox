import os
import logger
import shutil
from filemanipulation import sed, stripline
from installers.base.install import InstallBase
from settings import keyValueSettings

class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "gpi2/"
    RECALBOX_CONF = "/recalbox/share/system/recalbox.conf"

    def __init__(self):
        InstallBase.__init__(self)

    def InstallHardware(self, case):

        logger.hardlog("Installing RetroFlag GPi CASE 2 hardware")

        try:
            os.system("mount -o remount,rw /boot")
            os.system("mount -o remount,rw /")
            files = {
                '/boot/recalbox-user-config.txt': '/boot/recalbox-user-config.txt.backup',
                self.BASE_SOURCE_FOLDER + 'assets/recalbox-user-config.txt': '/boot/recalbox-user-config.txt',
                self.BASE_SOURCE_FOLDER + 'assets/gpi2.ppm': '/boot/boot.ppm',
            }
            for source_file, dest_file in files.items():
                installed_file = shutil.copy(source_file, dest_file)
                logger.hardlog(f"GPi2: {installed_file} installed")

            sed('\s*video=[^ ]+', '', '/boot/cmdline.txt')
            sed('noswap', 'noswap video=HDMI-A-2:d', '/boot/cmdline.txt')
            logger.hardlog("GPi2: set video parameter in cmdline.txt")

        except Exception as e:
            logger.hardlog("GPi2: Exception = {}".format(e))
            return False

        logger.hardlog("RetroFlag GPi CASE 2 hardware installed successfully!")
        return True

    def UninstallHardware(self, case):

        try:
            os.system("mount -o remount,rw /boot")
            os.system("mount -o remount,rw /")
            # Uninstall /boot/recalbox-user-config.txt
            if os.system("cp /boot/recalbox-user-config.txt.backup /boot/recalbox-user-config.txt") != 0:
                logger.hardlog("GPi2: Error uninstalling recalbox-user-config.txt")
                return False
            logger.hardlog("GPi2: recalbox-user-config.txt uninstalled")
            os.remove("/boot/boot.ppm")
            logger.hardlog("GPi2: /boot/boot.ppm uninstalled")
            sed(' video=HDMI-A-2:d', '', '/boot/cmdline.txt')
            logger.hardlog("GPi2: removed video setting in cmdline.txt")

        except Exception as e:
            logger.hardlog("GPi2: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")
            os.system("mount -o remount,ro /")

        return True

    def InstallSoftware(self, case):

        if case == "GPi2":

            logger.hardlog("Installing RetroFlag GPi CASE 2 software")

            try:
                os.system("mount -o remount,rw /")
                # Load recalbox.conf
                recalboxConf = keyValueSettings(self.RECALBOX_CONF, False)
                recalboxConf.loadFile()

                recalboxConf.setOption("emulationstation.theme.folder", "recalbox-goa2")
                logger.hardlog("GPi2: theme set to recalbox-goa2")
                recalboxConf.saveFile()
                # Force default videomode
                sed(
                    "([a-zA-Z0-9.].videomode)\\s*=.*",
                    "\\1=default",
                    self.RECALBOX_CONF,
                )
                # Install /recalbox/share/.retroarch.cfg
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/gpi2-retroarch.cfg"
                if os.system("cp {} /recalbox/share/.retroarch.cfg".format(sourceConfig)) != 0:
                    logger.hardlog("GPi2: Error installing .retroarch.cfg")
                    return ""
                logger.hardlog("GPi2: .retroarch.cfg installed")
                # Install modified es_input.cfg
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/es_input.cfg"
                if os.system(f"cp {sourceConfig} /recalbox/share/system/.emulationstation/es_input.cfg") != 0:
                    logger.hardlog("GPi2: Error installing es_input.cfg")
                    return ""
                logger.hardlog("GPi2: es_input.cfg installed")
                # Install /recalbox/system/resources/splash/logo-version.png
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/gpi2.png"
                if os.system(f"cp {sourceConfig} /recalbox/system/resources/splash/logo-version.png") != 0:
                    logger.hardlog("GPi2: Error installing gpi2.png")
                    return ""
                logger.hardlog("GPi2: gpi2.png installed")

            except Exception as e:
                logger.hardlog("GPi2: Exception = {}".format(e))
                return ""

            finally:
                os.system("mount -o remount,ro /")

            logger.hardlog("RetroFlag GPi CASE 2 software installed successfully!")
            return case

        return ""

    def UninstallSoftware(self, case):

        try:
            os.system("mount -o remount,rw /")
            os.remove("/recalbox/share/.retroarch.cfg")
            logger.hardlog("GPi2: /recalbox/share/.retroarch.cfg uninstalled")

        except Exception as e:
            logger.hardlog("GPi2: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /")

        return True

    def GetInstallScript(self, case):

        return None
