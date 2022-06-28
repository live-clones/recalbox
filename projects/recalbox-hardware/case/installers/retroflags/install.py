import os
import logger
from installers.base.install import InstallBase


class Install(InstallBase):

    def __init__(self):
        InstallBase.__init__(self)


    def InstallHardware(self, case):
        logger.hardlog("Installing Retroflag Case hardware")
        try:
            os.system("mount -o remount,rw /boot")
            if os.system('echo -e "\\ndtoverlay=retroflag-case" >> /boot/recalbox-user-config.txt') != 0:
                logger.hardlog("Retroflag: error adding dts to recalbox-user-config.txt")
                return False
        except Exception as e:
            logger.hardlog("Retroflag: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")

        logger.hardlog("Retroflag Case hardware installed successfully!")
        return True


    def InstallSoftware(self, case):
        logger.hardlog("Installing Retroflag Case software")
        return case


    def UninstallHardware(self, case):
        logger.hardlog("Uninstalling Retroflag Case hardware")
        try:
            os.system("mount -o remount,rw /boot")
            # Uninstall /boot/recalbox-user-config.txt
            if os.system('sed -i "/dtoverlay=retroflag-case/d" /boot/recalbox-user-config.txt') != 0:
                logger.hardlog("Retroflag: Error removing kms driver")
                return False
            logger.hardlog("Retroflag: kms driver removed")

        except Exception as e:
            logger.hardlog("Retroflag: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")

        return True


    def UninstallSoftware(self, case):
        logger.hardlog("Uninstalling Retroflag Case software")
        return ""


    def GetInstallScript(self, case):

        return None
