from installers.base.install import InstallBase


class Install(InstallBase):

    def __init__(self):
        InstallBase.__init__(self)

    def InstallHardware(self, case):

        return False

    def InstallSoftware(self, case):

        return case

    def UninstallHardware(self, case):

        return False

    def UninstallSoftware(self, case):

        return ""

    def GetInstallScript(self, case):

        return None
