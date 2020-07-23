from abc import abstractmethod

# Abstract installer
class InstallBase:

    BASE_SOURCE_FOLDER = "/recalbox/system/hardware/case/installers/"

    @abstractmethod
    def __init__(self):
        pass

    @abstractmethod
    def InstallHardware(self, case):
        pass

    @abstractmethod
    def InstallSoftware(self, case):
        pass

    @abstractmethod
    def UninstallHardware(self, case):
        pass

    @abstractmethod
    def UninstallSoftware(self, case):
        pass

    @abstractmethod
    def GetInstallScript(self, case):
        pass