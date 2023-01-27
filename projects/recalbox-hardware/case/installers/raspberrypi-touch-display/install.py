import os
import logger
from installers.base.install import InstallBase
from filemanipulation import sed


class Install(InstallBase):

    def __init__(self):
        InstallBase.__init__(self)

    def InstallHardware(self, case):
        logger.hardlog("Installing Raspberry Pi Touch Display Case hardware")
        try:
            os.system("mount -o remount,rw /boot")
            if os.system('echo -e "\\ndtoverlay=vc4-kms-dsi-7inch" >> /boot/recalbox-user-config.txt') != 0:
                raise "error adding dtoverlay to recalbox-user-config.txt"
            sed(' *video=[^ ]+', '', '/boot/cmdline.txt')
            sed('noswap', 'noswap video=DSI-1:800x480@60,rotate=180', '/boot/cmdline.txt')
        except Exception as e:
            logger.hardlog("Raspberry Pi Touch Display: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")

        logger.hardlog("Raspberry Pi Touch Display Case hardware installed successfully!")
        return True

    def InstallSoftware(self, case):
        logger.hardlog("Installing Raspberry Pi Touch Display Case software")
        return case

    def UninstallHardware(self, case):
        logger.hardlog("Uninstalling Raspberry Pi Touch Display Case hardware")
        try:
            os.system("mount -o remount,rw /boot")
            # Uninstall /boot/recalbox-user-config.txt
            if os.system('sed -i "/dtoverlay=vc4-kms-dsi-7inch/d" /boot/recalbox-user-config.txt') != 0:
                raise "Raspberry Pi Touch Display: Error removing kms driver"
            logger.hardlog("Raspberry Pi Touch Display: kms driver removed")
            sed(' *video=[^ ]+', '', '/boot/cmdline.txt')
        except Exception as e:
            logger.hardlog("Raspberry Pi Touch Display: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")

        return True

    def UninstallSoftware(self, case):
        logger.hardlog("Uninstalling Raspberry Pi Touch Display Case software")
        return ""

    def GetInstallScript(self, case):

        return None
