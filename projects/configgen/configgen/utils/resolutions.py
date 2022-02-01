from configgen.utils.architecture import Architecture


class ResolutionParser:

    __CEA = \
    {
        1: (640, 480),
        2: (640, 480),
        3: (854, 480),
        4: (1280, 720),
        5: (1920, 1080),
        6: (640, 480),
        7: (854, 480),
        8: (320, 240),
        9: (426, 240),
        10: (640, 480),
        11: (854, 480),
        12: (320, 240),
        13: (426, 240),
        14: (640, 480),
        15: (854, 480),
        16: (1920, 1080),
        17: (768, 576),
        18: (1024, 576),
        19: (1280, 720),
        20: (1920, 1080),
        21: (768, 576),
        22: (1024, 576),
        23: (384, 288),
        24: (512, 288),
        25: (768, 576),
        26: (1024, 576),
        27: (384, 288),
        28: (512, 288),
        29: (768, 576),
        30: (1024, 576),
        31: (1920, 1080),
        32: (1920, 1080),
        33: (1920, 1080),
        34: (1920, 1080),
        35: (640, 480),
        36: (854, 480),
        37: (768, 576),
        38: (1024, 576),
        39: (1920, 1080),
        40: (1920, 1080),
        41: (1280, 720),
        42: (768, 576),
        43: (1024, 576),
        44: (768, 576),
        45: (1024, 576),
        46: (1920, 1080),
        47: (1280, 720),
        48: (640, 480),
        49: (854, 480),
        50: (640, 480),
        51: (854, 480),
        52: (768, 576),
        53: (1024, 576),
        54: (768, 576),
        55: (1024, 576),
        56: (640, 480),
        57: (854, 480),
        58: (640, 480),
        59: (854, 480),
        60: (1280, 720),
        61: (1280, 720),
        62: (1280, 720),
        63: (1920, 1080),
        64: (1920, 1080),
    }
    
    __DMT = \
    {
        1: (640, 350),
        2: (640, 400),
        3: (720, 400),
        4: (640, 480),
        5: (640, 480),
        6: (640, 480),
        7: (640, 480),
        8: (800, 600),
        9: (800, 600),
        10: (800, 600),
        11: (800, 600),
        12: (800, 600),
        13: (800, 600),
        14: (848, 480),
        15: (1024, 768),
        16: (1024, 768),
        17: (1024, 768),
        18: (1024, 768),
        19: (1024, 768),
        20: (1024, 768),
        21: (1152, 864),
        22: (1280, 768),
        23: (1280, 768),
        24: (1280, 768),
        25: (1280, 768),
        26: (1280, 768),
        27: (1280, 800),
        28: (1280, 800),
        29: (1280, 800),
        30: (1280, 800),
        31: (1280, 800),
        32: (1280, 960),
        33: (1280, 960),
        34: (1280, 960),
        35: (1280, 1024),
        36: (1280, 1024),
        37: (1280, 1024),
        38: (1280, 1024),
        39: (1360, 768),
        40: (1360, 768),
        41: (1400, 1050),
        42: (1400, 1050),
        43: (1400, 1050),
        44: (1400, 1050),
        45: (1400, 1050),
        46: (1440, 900),
        47: (1440, 900),
        48: (1440, 900),
        49: (1440, 900),
        50: (1440, 900),
        51: (1600, 1200),
        52: (1600, 1200),
        53: (1600, 1200),
        54: (1600, 1200),
        55: (1600, 1200),
        56: (1600, 1200),
        57: (1680, 1050),
        58: (1680, 1050),
        59: (1680, 1050),
        60: (1680, 1050),
        61: (1680, 1050),
        62: (1792, 1344),
        63: (1792, 1344),
        64: (1792, 1344),
        65: (1856, 1392),
        66: (1856, 1392),
        67: (1856, 1392),
        68: (1920, 1200),
        69: (1920, 1200),
        70: (1920, 1200),
        71: (1920, 1200),
        72: (1920, 1200),
        73: (1920, 1440),
        74: (1920, 1440),
        75: (1920, 1440),
        76: (2560, 1600),
        77: (2560, 1600),
        78: (2560, 1600),
        79: (2560, 1600),
        80: (2560, 1600),
        81: (1366, 768),
        82: (1920, 1080),
        83: (1600, 900),
        84: (2048, 1152),
        85: (1280, 720),
        86: (1366, 768),
    }

    __NAMED = \
    {
        "nhd": (640, 360),
        "qhd": (960, 540),
        "hd": (1280, 720),
        "hd+": (1600, 900),
        "fhd": (1920, 1080),
        "wqhd": (2560, 1440),
        "qhd+": (3200, 1800),
        "4k uhd": (3840, 2160),

        "qqvga": (160, 120),
        "hqvga": (240, 160),
        "qvga": (320, 240),
        "wqvga": (384, 240),
        "hvga": (480, 320),
        "vga": (640, 480),
        "wvga": (768, 480),
        "fwvga": (854, 480),
        "svga": (800, 600),
        "wsvga": (1024, 576),
        "dvga": (960, 640),

        "xga": (1024, 768),
        "wxga": (1280, 768),
        "fwxga": (1366, 768),
        "xga+": (1152, 864),
        "wxga+": (1440, 900),
        "sxga": (1280, 1024),
        "wsxga": (1440, 960),
        "sxga+": (1400, 1050),
        "wsxga+": (1680, 1050),
        "uxga": (1600, 1200),
        "wuxga": (1920, 1200),

        "qwxga": (2048, 1152),
        "qxga": (2048, 1536),
        "wqxga": (2560, 1600),
        "qsxga": (2560, 2048),
        "wqsxga": (3200, 2048),
        "quxga": (3200, 2400),
        "wquxga": (3840, 2400),
    }

    __Width = 0  # type: int
    __Height = 0  # type: int

    def __init__(self, resolution): # type: (str) -> None
        self.__parse(resolution)
        self.__architecture = Architecture()

    def __parse(self, resolution): # type: (str) -> None
        if len(resolution) > 0 and resolution not in ("auto", "default"):
            if not self.__parseNamedResolutions(resolution):
                if not self.__parseWidthHeight(resolution):
                    if not self.__parseTvServiceResolution(resolution):
                        print("Unknown resolution: {}".format(resolution))

    def __parseNamedResolutions(self, resolution): # type: (str) -> bool
        if resolution in self.__NAMED:
            (w, h) = self.__NAMED[resolution]
            self.__Width = w
            self.__Height = h
            return True
        return False

    def __parseTvServiceResolution(self, resolution): # type: (str) -> bool
        try:
            (mode, sindex, _) = resolution.split(' ')
            index = int(sindex)
            if mode == "CEA":
                (w, h) = self.__CEA[index]
                self.__Width = w
                self.__Height = h
                return True
            elif mode == "DMT":
                (w, h) = self.__DMT[index]
                self.__Width = w
                self.__Height = h
                return True
        except Exception as ex:
            print("{} is not a tvservice resolution: {}".format(resolution, ex))
            pass
        self.__Width = 0
        self.__Height = 0
        return False
    def __parseWidthHeight(self, resolution): # type: (str) -> bool
        try:
            wh = resolution.split('x')
            if len(wh) == 2:
                w = int(wh[0])
                h = int(wh[1])
                if w > 0 and h > 0:
                    self.__Width = w
                    self.__Height = h
                    return True
        except Exception as ex:
            print("{} is not a XxY resolution: {}".format(resolution, ex))
            pass
        self.__Width = 0
        self.__Height = 0
        return False

    @property
    def isSet(self): # type: () -> bool
        return self.__Width != 0 and self.__Height != 0

    @property
    def selfProcess(self): # type: () -> bool
        return not self.__architecture.isSupportingTvService

    @property
    def width(self): # type: () -> int
        return self.__Width

    @property
    def height(self): # type: () -> int
        return self.__Height

    @property
    def string(self): # type: () -> str
        return "{}x{}".format(self.__Width, self.__Height)
