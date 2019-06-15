import sys
from settings import Settings

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
def DetectGPiCase(cases):
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

# --------- Nuxii

# --------- Main

# Main identification routine
def Identify():
    cases = __import__("cases")
    case = cases.NONE

    with open("/recalbox/recalbox.arch", "r") as sf:
        board = sf.readline()

    if board == "rpi0" or board == "rpi1":
        case = DetectGPiCase(cases)

    return case


# Usage
def printUsageAndExit():
    print('Usage: manage.py "casename" install machine')
    print('       - casename : case to install. empty string for auto detection')
    print('       - install : 1 to install, 0 to uninstall')
    print('       - machine : 1 if the script runs from boot. 0 is this is called after a human decision')
    print('       - phase   : 0 hardware install/uninstall. 1 software install/uninstall')
    exit(0)


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

    settings = Settings("/boot/recalbox-boot.conf")
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

        # import logger
        logger = __import__("logger")

        # Auto-identifying
        logger.hardlog("Current case: " + case)
        if len(case) == 0:
            case = Identify()
            logger.hardlog("Case detected: " + case)

        installers = __import__("installer")
        if machine:
            # Machine initiated - process install phases
            if phase == 0:
                needReboot = installers.processHardware(install, case, previousCase)
            else:
                case = installers.processSoftware(install, case)
        else:
            # Human action - process both hardware & software
            needReboot = installers.processHardware(install, case, previousCase)
            case = installers.processSoftware(install, case)

        # Set new case value
        settings.setOption(caseKey, "{}:{}".format(case, phase))

        # Save settings
        subprocess = __import__("subprocess")
        subprocess.call(["mount", "-o", "remount,rw", "/boot"])
        settings.saveFile()

        if needReboot and machine:
            logger.hardlog("Automatic reboot required")
            subprocess.call(["reboot", "-f"])
            exit(0)

    # No reboot after phase 0, make install image available
    if previousPhase == 0:

        installers = __import__("installer")
        picture = installers.getInstallPicture(previousCase)
        if picture is not None:

            # import logger
            logger = __import__("logger")
            logger.hardlog("Request install picture")

            # Make temporary link to image
            os = __import__("os")
            pathToSymLink = "/tmp/.install.png"
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
