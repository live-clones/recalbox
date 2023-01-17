import os
import logger
from installers.base.install import InstallBase


class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "argonone/"
    S99 = "/etc/init.d/S99argononed"
    CONF_FILE = "/recalbox/share/system/argononed.conf"

    def __init__(self):
        InstallBase.__init__(self)


    def InstallHardware(self, case):
        logger.hardlog("Installing ArgonOne Case hardware")
        try:
            os.system("mount -o remount,rw /")
            os.system("mount -o remount,rw /boot")
            
            if os.system('echo -e "\ni2c-bcm2708\ni2c-dev\n" >> /etc/modules.conf') != 0:
                logger.hardlog("ArgonOne: error enabling modules")
                return False
            if os.system('echo -e "\ndtparam=i2c_arm=on\nenable_uart=1\n" >> /boot/recalbox-user-config.txt') != 0:
                logger.hardlog("ArgonOne: error editing /boot/recalbox-user-config.txt")
                return False
            if os.system('cp {}assets/S99argononed {}'.format(self.BASE_SOURCE_FOLDER, self.S99)) != 0:
                logger.hardlog("ArgonOne: error copying S99argononed")
                return False
        except Exception as e:
            logger.hardlog("ArgonOne: Exception = {}".format(e))
            return False

        logger.hardlog("ArgonOne Case hardware installed successfully!")
        return True


    def InstallSoftware(self, case):
        logger.hardlog("Installing ArgonOne Case software")
        try:
            if os.system('cp {}assets/argononed.conf {}'.format(self.BASE_SOURCE_FOLDER, self.CONF_FILE)) != 0:
                logger.hardlog("ArgonOne: error installing config file")
        except Exception as e:
            logger.hardlog("ArgonOne: Exception = {}".format(e))
        return ""


    def UninstallHardware(self, case):
        logger.hardlog("Uninstalling ArgonOne Case hardware")
        try:
            os.system("mount -o remount,rw /")
            os.system("mount -o remount,rw /boot")
            if os.system('sed -i "/i2c-bcm2708/d" /etc/modules.conf') != 0:
                logger.hardlog("ArgonOne: Error removing i2c-bcm2708 module")
            if os.system('sed -i "/i2c-dev/d" /etc/modules.conf') != 0:
                logger.hardlog("ArgonOne: Error removing i2c-dev module")
            if os.system('sed -i "/dtparam=i2c_arm=on/d" /boot/recalbox-user-config.txt') != 0:
                logger.hardlog("ArgonOne: Error removing dtparam")
            if os.system('sed -i "/enable_uart=1/d" /boot/recalbox-user-config.txt') != 0:
                logger.hardlog("ArgonOne: Error removing enable_uart")
            if os.system("rm -f {}".format(self.S99)) != 0:
                logger.hardlog("ArgonOne: Error removing S99argononed")
        except Exception as e:
            logger.hardlog("ArgonOne: Exception = {}".format(e))
        return case


    def UninstallSoftware(self, case):
        logger.hardlog("Uninstalling Retroflag Case software")
        os.system("rm -f {}".format(self.CONF_FILE))
        return ""


    def GetInstallScript(self, case):

        return None
