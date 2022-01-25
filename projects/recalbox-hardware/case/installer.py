import logger
import cases

# Module per case(s)
MODULES = {
    "installers.gpi.install": (cases.GPI_V1, cases.GPI_V2, cases.GPI_V3),
    "installers.nespi4.install": (cases.NESPI4,),
    "installers.nuxii.install": (cases.NUXII,),
    "installers.piboy.install": (cases.PIBOY,),
    "installers.retroflags.install": (cases.PISTATION, cases.NESPICASEPLUS, cases.SUPERPICASE, cases.MEGAPICASE),
    "installers.gpi2.install": (cases.GPI2,),
}

def processHardware(install, case, previousCase):

    uninstallReboot = False
    installReboot = False

    # Check if the previous case need to be uninstalled
    if install and previousCase not in (case, "", "none"):
        for moduleName, caseNames in MODULES.items():
            if previousCase in caseNames:
                logger.hardlog("Uninstall previous case: " + previousCase)
                module = __import__(moduleName, fromlist=["Install"])
                installClass = getattr(module, "Install")
                installer = installClass()
                installer.UninstallSoftware()
                uninstallReboot = installer.UninstallHardware(previousCase)

    # Call target installer/uninstaller
    for moduleName, caseNames in MODULES.items():
        if case in caseNames:
            module = __import__(moduleName, fromlist=["Install"])
            installClass = getattr(module, "Install")
            installer = installClass()
            if install:
                logger.hardlog("Installing case hardware: " + case)
                installReboot = installer.InstallHardware(case)
            else:
                logger.hardlog("Uninstalling case hardware: " + case)
                installReboot = installer.UninstallHardware(case)

    return uninstallReboot | installReboot


def processSoftware(install, case):

    # Call target installer/uninstaller
    for moduleName, caseNames in MODULES.items():
        if case in caseNames:
            module = __import__(moduleName, fromlist=["Install"])
            installClass = getattr(module, "Install")
            installer = installClass()
            if install:
                logger.hardlog("Installing case software: " + case)
                case = installer.InstallSoftware(case)
            else:
                logger.hardlog("Uninstalling case software: " + case)
                case = installer.UninstallSoftware(case)

    return case


def getInstallScript(case):

    for moduleName, caseNames in MODULES.items():
        if case in caseNames:
            logger.hardlog("Get install script: " + case)
            module = __import__(moduleName, fromlist=["Install"])
            installClass = getattr(module, "Install")
            installer = installClass()
            return installer.GetInstallScript(case)

    return None
