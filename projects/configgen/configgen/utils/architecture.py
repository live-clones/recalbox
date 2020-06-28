

class Architecture:

    __Arch = "Unknown"  # type: str

    def __init__(self):
        self.__getArch()

    def __getArch(self):
        try:
            with open("/recalbox/recalbox.arch") as f:
                self.__Arch = f.read()
        except Exception as ex:
            print("Error getting architecture: {}".format(ex))

    @property
    def isPi0or1(self):
        return self.__Arch == "rpi1"

    @property
    def isPi2(self):
        return self.__Arch == "rpi2"

    @property
    def isPi3(self):
        return self.__Arch == "rpi3"

    @property
    def isPi4(self):
        return self.__Arch == "rpi4"

    @property
    def isXu4(self):
        return self.__Arch == "odroidxu4"

    @property
    def isX86(self):
        return self.__Arch == "x86"

    @property
    def isX64(self):
        return self.__Arch == "x86_64"

    @property
    def isSupportingTvService(self):
        return self.isPi0or1 or self.isPi2 or self.isPi3
