import logger
import cases

# Module per case(s)
MODULES = \
{
    "installers.gpi.install": (cases.GPI_V1, cases.GPI_V2, cases.GPI_V3),
    "installers.nuxii.install": (cases.NUXII,),
}


def processHardware(install, case, previousCase):

    uninstallReboot = False
    installReboot   = False

    # Check if the previous case need to be uninstalled
    if install and previousCase not in (case, "", "none"):
        for moduleName, caseNames in MODULES.iteritems():
            if previousCase in caseNames:
                logger.hardlog("Uninstall previous case: " + previousCase)
                module = __import__(moduleName, fromlist=["Install"])
                installClass = getattr(module, "Install")
                installer = installClass()
                installer.UninstallSoftware()
                uninstallReboot = installer.UninstallHardware(previousCase)

    # Call target installer/uninstaller
    for moduleName, caseNames in MODULES.iteritems():
        if case in caseNames:
            logger.hardlog("Install new case: " + case)
            module = __import__(moduleName, fromlist=["Install"])
            installClass = getattr(module, "Install")
            installer = installClass()
            if install:
                installReboot = installer.InstallHardware(case)
            else:
                installReboot = installer.UninstallHardware(case)

    return uninstallReboot | installReboot


def processSoftware(install, case):

    # Call target installer/uninstaller
    for moduleName, caseNames in MODULES.iteritems():
        if case in caseNames:
            logger.hardlog("Install new case: " + case)
            module = __import__(moduleName, fromlist=["Install"])
            installClass = getattr(module, "Install")
            installer = installClass()
            if install:
                case = installer.InstallSoftware(case)
            else:
                case = installer.UninstallSoftware(case)

    return case


def getInstallPicture(case):

    for moduleName, caseNames in MODULES.iteritems():
        if case in caseNames:
            logger.hardlog("Get install picture: " + case)
            module = __import__(moduleName, fromlist=["Install"])
            installClass = getattr(module, "Install")
            installer = installClass()
            return installer.GetInstallPicture(case)

    return None

