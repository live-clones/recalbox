import os
import logger
from installers.base.install import InstallBase


class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "nespi4/"

    def __init__(self):
        InstallBase.__init__(self)


    def InstallHardware(self, case):

        logger.hardlog("Installing NesPi4 Case hardware")

        try:
            os.system("mount -o remount,rw /boot")
            # Install /boot/config.txt - most important change first
            destinationConfig = "/boot/config.txt"
            os.system("sed '/^dtoverlay=RetroFlag.*\.dtbo/d' {}".format(destinationConfig))
            os.system("echo '' >> {}".format(destinationConfig))
            os.system("echo 'dtoverlay=RetroFlag_pw_io.dtbo' >> {}".format(destinationConfig))
            os.system("sed '/^unable_uart=.*/d' {}".format(destinationConfig))
            os.system("echo 'unable_uart=1' >> {}".format(destinationConfig))
            logger.hardlog("NesPi14: config.txt installed")

            # Install Overlay
            sourceOverlay = self.BASE_SOURCE_FOLDER + "assets/overlays/*.dtbo"
            if os.system("cp -r {} /boot/overlays".format(sourceOverlay)) != 0:
                logger.hardlog("NesPi4: Error installing overlays")
                return False
            logger.hardlog("NesPi4: overlay installed")

        except Exception as e:
            logger.hardlog("NesPi4: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /")
            os.system("mount -o remount,ro /boot")

        logger.hardlog("NesPi4 Case hardware installed successfully!")
        return True


    def InstallSoftware(self, case):

        logger.hardlog("Installing NesPi4 Case software")

        try:
            os.system("mount -o remount,rw /")

            script = self.BASE_SOURCE_FOLDER + "assets/shutdown.py"
            try:
                with open("/etc/init.d/S99RetroFlag", 'w+') as sf:
                    sf.write("python {} &".format(script))
                os.chmod("/etc/init.d/S99RetroFlag", 0o755)
                os.system("/etc/init.d/S99RetroFlag")
                logger.hardlog("NesPi4: safe shutdown installed")
            except Exception as e:
                logger.hardlog("NesPi4: error installing safe shutdown ({})".format(e.message))

        except Exception as e:
            logger.hardlog("NesPi4: Exception = {}".format(e))
            return ""

        finally:
            os.system("mount -o remount,ro /")

        logger.hardlog("NesPi4 software installed successfully!")
        return case


    def UninstallHardware(self, case):

        try:
            os.system("mount -o remount,rw /")

            destinationConfig = "/boot/config.txt"
            os.system("sed '/^dtoverlay=RetroFlag.*\.dtbo/d' {}".format(destinationConfig))
            os.system("sed '/^unable_uart=.*/d' {}".format(destinationConfig))

        except Exception as e:
            logger.hardlog("NesPi4: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /")

        return True


    def UninstallSoftware(self, case):

        try:
            os.system("mount -o remount,rw /")

            # Uninstall shutdown scripty
            if os.system("rm -f /etc/init.d/S99RetroFlag") != 0:
                logger.hardlog("NesPi4: Error uninstalling config.txt")
                return False
            logger.hardlog("NesPi4: config.txt uninstalled")

        except Exception as e:
            logger.hardlog("GPi: Exception = {}".format(e))
            return case

        finally:
            os.system("mount -o remount,ro /")

        return ""


    def GetInstallScript(self, case):

        return ""
