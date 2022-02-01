

class Architecture:

    __Arch = "Unknown"  # type: str

    def __init__(self):
        self.__getArch()

    def __getArch(self):
        try:
            with open("/recalbox/recalbox.arch") as f:
                self.__Arch = f.read()
            if self.isGoa2:
                with open("/sys/class/drm/card0/card0-DSI-1/mode") as f:
                    reso = f.readline().strip("\r\n")
                    if reso == "480x854p60":
                        self.__Arch = "odroidgo3"
        except Exception as ex:
            print("Error getting architecture: {}".format(ex))

    @property
    def Architecture(self):
        return self.__Arch

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
    def isGoa2(self):
        return self.__Arch == "odroidgo2"

    @property
    def isGoa3(self):
        return self.__Arch == "odroidgo3"

    @property
    def isX86(self):
        return self.__Arch == "x86"

    @property
    def isX64(self):
        return self.__Arch == "x86_64"

    @property
    def isSupportingTvService(self):
        return self.isPi0or1 or self.isPi2
