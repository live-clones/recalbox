import typing
from dataclasses import dataclass
from enum import Enum


class CRTResolution(str, Enum):
    # 60Hz
    p1920x240 = "p1920x240"
    p320x240 = "p320x240"
    p1920x224 = "p1920x224"
    i640x480 = "i640x480"
    # 50Hz
    p1920x288 = "p1920x288"
    p384x288 = "p384x288"
    i768x576 = "i768x576"
    # 31kHz
    p640x480 = "p640x480"
    p1920x240at120 = "p1920x240at120"


class CRTSuperRez(str, Enum):
    original = "original"
    x2 = "x2"
    x6 = "x6"
    x8 = "x8"

    @staticmethod
    def fromString(value: str):
        if value == "original":
            return CRTSuperRez.original
        if value == "x2":
            return CRTSuperRez.x2
        if value == "x6":
            return CRTSuperRez.x6
        if value == "x8":
            return CRTSuperRez.x8
        return CRTSuperRez.original

    def multiplier(self):
        if self == "original":
            return 1
        if self == "x2":
            return 2
        if self == "x6":
            return 6
        if self == "x8":
            return 8
        return 1


CRTConfigurationByResolution = dict[str, dict[str, int]]

CRTSystemMode = typing.Tuple[str, str, str, str, str, int, int]
CRTArcadeMode = typing.Tuple[str, str, int, int, int]


@dataclass
class CRTArcadeGameV2:
    name: str
    core: str
    width: int
    height: int
    vfreq: float
    vertical: bool


class CRTVideoStandard(str, Enum):
    PAL = "pal"
    NTSC = "ntsc"
    AUTO = "auto"
    ALL = "all"

    @staticmethod
    def fromString(value: str):
        if value == "pal":
            return CRTVideoStandard.PAL
        if value == "ntsc":
            return CRTVideoStandard.NTSC
        return CRTVideoStandard.AUTO


class CRTRegion(str, Enum):
    AUTO = "auto"
    EU = "eu"
    US = "us"
    JP = "jp"

    @staticmethod
    def fromString(value: str):
        if value == "eu":
            return CRTRegion.EU
        if value == "us":
            return CRTRegion.US
        if value == "jp":
            return CRTRegion.JP
        return CRTRegion.AUTO


class CRTScreenType(str, Enum):
    k15 = "15kHz"
    k15Ext = "15kHzExt"
    k31 = "31kHz"

    @staticmethod
    def fromString(value: str):
        if value == "31kHz":
            return CRTScreenType.k31
        if value == "15kHzExt":
            return CRTScreenType.k15Ext
        return CRTScreenType.k15


class CRTResolutionType(str, Enum):
    Progressive = "progressive"
    Interlaced = "interlaced"
    DoubleFreq = "doublefreq"

    @classmethod
    def fromString(cls, value: str):
        for k, v in cls.__members__.items():
            if v == value:
                return v
        return CRTResolutionType.Progressive


@dataclass
class CRTSystem:
    core: str
    region: str
    display: str
    reztype: str
    mode_id: str
    viewport_width: int
    viewport_height: int
