import typing
from dataclasses import dataclass
from enum import Enum, StrEnum


class CRTResolution(StrEnum):
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


CRTConfigurationByResolution = dict[str, dict[str, int]]

CRTSystemMode = typing.Tuple[str, str, str, str, str, int, int]
CRTArcadeMode = typing.Tuple[str, str, int, int, int]


class CRTVideoStandard(StrEnum):
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


class CRTRegion(StrEnum):
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


class CRTScreenType(StrEnum):
    k15 = "15kHz"
    k31 = "31kHz"

    @staticmethod
    def fromString(value: str):
        if value == "31kHz":
            return CRTScreenType.k31
        return CRTScreenType.k15


class CRTResolutionType(StrEnum):
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