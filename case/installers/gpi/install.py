import os
import logger
import xml.etree.ElementTree as XmlTree
from installers.base.install import InstallBase

class Install(InstallBase):

    BASE_SOURCE_FOLDER = InstallBase.BASE_SOURCE_FOLDER + "gpi/"
    #BASE_SOURCE_FOLDER = "/home/bkg2k/Development/Recalbox/recalbox-hardware/case/installers/" + "gpi/"

    def __init__(self):
        pass

    def InstallHardware(self, case):

        if case == "GPiV1":

            logger.hardlog("Installing GPi Case V1 hardware")

            try:
                os.system("mount -o remount,rw /boot")
                # Install /boot/config.txt - most important change first
                sourceConfig = self.BASE_SOURCE_FOLDER + "assets/config.txt"
                os.system("cp /boot/config.txt /boot/config.txt.backup")
                if os.system("cp {} /boot/overlays".format(sourceConfig)) != 0:
                    logger.hardlog("GPi: Error installing config.txt")
                    return False
                logger.hardlog("GPi: config.txt installed")

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

            logger.hardlog("Installing GPi Case V1 software")

            try:
                # Install safe shutdown
                os.system("mount -o remount,rw /")
                script = self.BASE_SOURCE_FOLDER + "assets/recalbox_SafeShutdown_gpi.py"
                try:
                    with open("/etc/init.d/S99RetroFlag", 'w+') as sf:
                        sf.write("python {} &".format(script))
                    os.chmod("/etc/init.d/S99RetroFlag", 0o755)
                    logger.hardlog("GPi: safe shutdown installed")
                except Exception as e:
                    logger.hardlog("GPi: error installing safe shutdown ({})".format(e.message))

                # Switch default resolution
                os.system("sed -i -E 's/([a-z\.]*)videomode=.*/\1videomode=default/g' /recalbox/share/system/recalbox.conf")
                logger.hardlog("GPi: set default resolution")

                # Install GPi XBOX360 config
                srcTree = XmlTree.parse(self.BASE_SOURCE_FOLDER + "assets/es_input.fragment.xml")
                srcRoot = srcTree.getroot()
                dstTree = XmlTree.parse("/recalbox/share/system/.emulationstation/es_input.cfg")
                dstRoot = dstTree.getroot()
                for sourceController in srcRoot.findall(".//inputConfig"):
                    for controller in dstRoot.findall(".//inputConfig"):
                        if  controller.get("deviceName") == sourceController.get("deviceName") and \
                            controller.get("deviceGUID") == sourceController.get("deviceGUID"):
                            dstRoot.remove(controller)
                            dstRoot.append(sourceController)
                dstTree.write("/recalbox/share/system/.emulationstation/es_input.xml", "UTF-8", True, None, "xml")
                logger.hardlog("GPi: controller updated")

                # Install Theme
                sourceTheme = self.BASE_SOURCE_FOLDER + "assets/theme/recalbox-gpicase"
                if os.system("cp -r {} /recalbox/share/themes/".format(sourceTheme)) == 0:
                    # Switch Theme
                    os.system("sed -i -E 's/name=|ThemeSet| value=|.*|/name=|ThemeSet| value=|recalbox-gpicase|/g' /recalbox/share/system/.emulationstation/es_settings.cfg".replace('|', '"'))
                    logger.hardlog("GPi: GPi-case theme installed")

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
            # Uninstall /boot/config.txt
            if os.system("cp /boot/config.txt.backup /boot/config.txt") != 0:
                logger.hardlog("GPi: Error uninstalling config.txt")
                return False
            logger.hardlog("GPi: config.txt uninstalled")

        except Exception as e:
            logger.hardlog("GPi: Exception = {}".format(e))
            return False

        finally:
            os.system("mount -o remount,ro /boot")

        return True


    def UninstallSoftware(self, case):

        try:
            # Switch back to default theme
            os.system("sed -E 's/name=|ThemeSet| value=|.*|/name=|ThemeSet| value=|recalbox-next|/g' /recalbox/share/system/.emulationstation/es_settings.cfg".replace('|', '"'))
            logger.hardlog("GPi: GPi-case theme uninstalled")

        except Exception as e:
            logger.hardlog("GPi: Exception = {}".format(e))
            return case

        return ""


    def GetInstallPicture(self, case):

        return self.BASE_SOURCE_FOLDER + "assets/install.png"
