import pytest

from configgen.utils.resolutions import ResolutionParser


def test_given_1920x240_return_progressive_resolution(mocker):
    assert ResolutionParser("1920x240").height == 240
    assert ResolutionParser("1920x240").width == 1920
    assert ResolutionParser("1920x240").progressive is True

def test_given_1920x240p_return_progressive_resolution(mocker):
    assert ResolutionParser("1920x240p").height == 240
    assert ResolutionParser("1920x240p").width == 1920
    assert ResolutionParser("1920x240p").progressive is True

def test_given_640x480i_return_interlaced_resolution(mocker):
    assert ResolutionParser("640x480i").height == 480
    assert ResolutionParser("640x480i").width == 640
    assert ResolutionParser("640x480i").progressive is False
