import logger
import os
import subprocess
import RPi.GPIO as GPIO
import sys
import time
from settings import keyValueSettings
import cases


# --------- GPI

def read(path, name):
    usbPrefix = "/sys/bus/usb/devices"
    try:
        f = open("{}/{}/{}".format(usbPrefix, path, name))
        return f.readline().rstrip(" \n")
    except IOError:
        pass
    return ""


# GPi case
def DetectGPiCase():
    gpiUsbPath = "1-1"
    if read(gpiUsbPath, "manufacturer") == "RetroFlag":
        if read(gpiUsbPath, "product") == "GPi Case":
            serial = read(gpiUsbPath, "serial")
            if serial == "RetroFlag 01":
                return cases.GPI_V1
            elif serial == "RetroFlag 02":
                return cases.GPI_V2
            elif serial == "RetroFlag 03":
                return cases.GPI_V3

    return cases.NONE

# GPi case 2
def DetectGPiCase2():
    logger.hardlog("trying GPi2")
    hdmiPin = 18
    card = "card1-HDMI-A-1"

    def init():
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(hdmiPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    def cleanup():
        GPIO.setup(hdmiPin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

    def detect(card):
        try:
            with open(f"/sys/class/drm/{card}/edid", 'rb') as f:
                edid = f.read(128)
                f.close()
                if edid[8] == 0x64 and edid[9] == 0x8b:
                    logger.hardlog('gpi2: success reading edid!')
                    return True
        except Exception as e:
            logger.hardlog(f"gpi2: error reading edid: {e}")
        return False

    def is_hdmi_connected(card):
        try:
            with open(f"/sys/class/drm/{card}/status") as f:
                line = f.readline().strip()
                if line == "connected":
                    logger.hardlog(f"gpi2: {card} status connected")
                    return True
        except Exception as e:
            logger.hardlog(f"gpi2: error reading {card} status: {e}")
        return False

    detected = cases.NONE
    import re
    pattern = re.compile("Compute Module 4")

    for i, line in enumerate(open('/proc/cpuinfo')):
        if any(re.finditer(pattern, line)):
            logger.hardlog("gpi2: compute module 4 found!")
            init()
            time.sleep(0.1)
            if is_hdmi_connected(card) and detect(card):
                detected = cases.GPI2

            cleanup()

    return detected

# --------- NesPi 4

def DetectNesPi4Case():
    gpiUsbPath = "2-1:1.0"
    if read(gpiUsbPath, "modalias") in ("usb:v152Dp0562d0214dc00dsc00dp00ic08isc06ip62in00", "usb:v152Dp0578d0209dc00dsc00dp00ic08isc06ip50in00"):
        return cases.NESPI4

    return cases.NONE

# --------- Nuxii

# --------- PiBoy


def DetectPiBoyCase():
    case = cases.NONE
    try:
        logger.hardlog("trying piboy")
        if os.path.isfile('/sys/kernel/xpi_gamecon/status'):
            with open('/sys/kernel/xpi_gamecon/status', 'r') as f:
                status = int(f.readline())
        else:
            status = int(subprocess.run(['/usr/bin/piboy-tester'], stdout=subprocess.PIPE).stdout.decode('utf-8'))
        logger.hardlog(f"status = {status}")
        if status in (70, 198):
            case = cases.PIBOY
            # load module now, or piboy may shutdown
            subprocess.run(['modprobe', 'xpi_gamecon'])
    except:
        pass
    return case

# --------- Main

# Main identification routine
def Identify():
    case = cases.NONE

    with open("/recalbox/recalbox.arch", "r") as sf:
        board = sf.readline()

    if board in ("rpi0", "rpi1", "rpi3", "rpizero2legacy"):
        case = DetectGPiCase()

    if board == "rpi4":
        case = DetectNesPi4Case()

    if board == "rpi4" and case == cases.NONE:
        case = DetectGPiCase2()

    if board in ["rpi3", "rpi4"] and case == cases.NONE:
        case = DetectPiBoyCase()

    return case


# Usage
def printUsageAndExit():
    print('Usage: manage.py "casename" install machine')
    print('       - casename : case to install. empty string for auto detection')
    print('       - install : 1 to install, 0 to uninstall')
    print('       - machine : 1 if the script runs from boot. 0 is this is called after a human decision')
    print('       - phase   : 0 hardware install/uninstall. 1 software install/uninstall')
    sys.exit(0)


# Main
def mainInstall():
    if len(sys.argv) < 5:
        printUsageAndExit()

    case    = sys.argv[1]
    install = (sys.argv[2] == '1')
    machine = (sys.argv[3] == '1')
    phase   = int(sys.argv[4])
    if phase < 0: phase = 0
    if phase > 1: phase = 1

    settings = keyValueSettings("/boot/recalbox-boot.conf")
    settings.loadFile()

    # Install/Uninstall new case
    needReboot = False
    requestedCase = case
    caseKey = "case"
    previousPhase = -1

    previousCase = settings.getOption(caseKey, "")
    if ':' in previousCase:
        previousCase, previousPhase = previousCase.split(':')
        previousPhase = int(previousPhase)

    # Decide whether we install/uninstall or not
    if previousCase == "" or previousPhase < phase or not machine:

        # Auto-identifying
        logger.hardlog("Current case: " + case)
        if len(case) == 0:
            case = Identify()
            logger.hardlog("Case detected: " + case)

        import installer
        if machine:
            # Machine initiated - process install phases
            if phase == 0:
                needReboot = installer.processHardware(install, case, previousCase)
            else:
                case = installer.processSoftware(install, case)
        else:
            # Human action - process both hardware & software
            needReboot = installer.processHardware(install, case, previousCase)
            case = installer.processSoftware(install, case)

        # Set new case value
        settings.setOption(caseKey, "{}:{}".format(case, phase))

        # Save settings
        subprocess.call(["mount", "-o", "remount,rw", "/boot"])
        settings.saveFile()

        if needReboot and machine:
            logger.hardlog("Automatic reboot required")
            subprocess.call(["reboot", "-f"])
            sys.exit(0)

    # No reboot after phase 0, make install image available
    if previousPhase == 0:

        import installer
        picture = installer.getInstallScript(previousCase)
        if picture is not None:

            # import logger
            logger.hardlog("Request install script")

            # Make temporary link to install script
            import os
            pathToSymLink = "/tmp/.install.sh"
            if os.path.exists(pathToSymLink):
                os.remove(pathToSymLink)
            os.symlink(picture, pathToSymLink)


    # Need reboot?
    if needReboot:
        return 2
    # Human request failed?
    if not machine and requestedCase != case:
        return 1
    # Everything is fine
    return 0


# Entry point
if __name__ == "__main__":
    exit(mainInstall())
