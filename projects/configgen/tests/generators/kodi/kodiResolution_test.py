import pytest

from configgen.Emulator import Emulator
from configgen.generators.kodi.kodiResolution import KodiResolution



def test_given_1920x240_return_standard_mode(mocker):
    kodiResolution = KodiResolution()
    system = Emulator("kodi","","","1920x240","")
    assert kodiResolution.transform(system) == "0192000240060.00000pstd"


def test_given_640x480_return_standard_mode(mocker):
    kodiResolution = KodiResolution()
    system = Emulator("kodi","","","640x480","")
    assert kodiResolution.transform(system) == "0064000480060.00000pstd"

def test_given_default_return_no_mode(mocker):
    kodiResolution = KodiResolution()
    system = Emulator("kodi","","","default","")
    assert kodiResolution.transform(system) == ""

def test_given_640x480_interlaced_return_interlaced_mode(mocker):
    kodiResolution = KodiResolution()
    system = Emulator("kodi","","","640x480i","")
    assert kodiResolution.transform(system) == "0064000480060.00000istd"
