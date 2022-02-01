from unittest.mock import mock_open

import pytest

from configgen.crt.CRTConfigParser import CRTConfigParser, CRTVideoStandard, CRTScreenType, CRTResolutionType
from configgen.crt.Mode import Mode


def givenThisFileContent(mocker, content):
    mocker.patch('pathlib.Path.exists', return_value=True)
    mocker.patch('builtins.open', mock_open(read_data=content))


def test_given_a_single_line_file_parse_modes_file_should_returns_a_dict_with_the_mode_id_and_mode_string(mocker):
    givenThisFileContent(mocker, "240@60p,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\n")
    config = CRTConfigParser().loadMode("240@60p")
    assert config == Mode("1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1", "60")


def test_given_a_comment_should_be_ignored(mocker):
    givenThisFileContent(mocker, "#240@60p,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60")
    with pytest.raises(Exception):
        CRTConfigParser().loadMode("240@60p")


def test_given_a_multi_line_file_load_mode_should_returns_the_requested_mode(mocker):
    givenThisFileContent(mocker,
                         "240@60p,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\n224@50p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    assert CRTConfigParser().loadMode("224@50p") == Mode("1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1", "60")
    assert CRTConfigParser().loadMode("240@60p") == Mode("1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1", "60")


def test_given_a_malformed_file_load_mode_should_throw(mocker):
    with pytest.raises(Exception):
        givenThisFileContent(mocker, "240@60p,1920, 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60")
        CRTConfigParser().loadMode("240@60p")


def test_given_a_single_line_file_parse_systems_file_should_returns_a_dict_with_the_system_id_the_region_and_mode_id(
        mocker):
    givenThisFileContent(mocker, "snes,pal,15kHz,progressive,snes:224@50p,0,0")
    config = CRTConfigParser().findSystem("snes", CRTVideoStandard.PAL, CRTScreenType.k15, CRTResolutionType.Progressive)
    assert config == ("pal", "15kHz", "progressive", "snes:224@50p", 0, 0)


def test_given_a_multi_line_file_parse_systems_file_should_returns_a_dict_with_the_system_id_the_region_and_mode_id(
        mocker):
    givenThisFileContent(mocker, "snes,pal,15kHz,progressive,snes:224@50p,0,0\nsnes,ntsc,15kHz,progressive,snes:224@60p,0,0")
    config = CRTConfigParser().findSystem("snes", CRTVideoStandard.PAL, CRTScreenType.k15, CRTResolutionType.Progressive)
    assert config == ("pal", "15kHz", "progressive", "snes:224@50p", 0, 0)
    config = CRTConfigParser().findSystem("snes", CRTVideoStandard.NTSC, CRTScreenType.k15, CRTResolutionType.Progressive)
    assert config == ("ntsc", "15kHz", "progressive", "snes:224@60p", 0, 0)


def test_given_a_systems_with_overscans_should_returns_the_overscans(
        mocker):
    givenThisFileContent(mocker, "snes,pal,15kHz,progressive,snes:224@50p,10,20")
    config = CRTConfigParser().findSystem("snes", CRTVideoStandard.PAL, CRTScreenType.k15, CRTResolutionType.Progressive)
    assert config == ("pal", "15kHz", "progressive", "snes:224@50p", 10, 20)


def test_given_an_arcade_game_file_should_load_game_modes(
        mocker):
    givenThisFileContent(mocker, "arkangc,fbneo,arcade:224@59.185606,0,0,0")
    config = CRTConfigParser().findArcadeGame("arkangc","fbneo")
    assert config == ("fbneo","arcade:224@59.185606", 0, 0, 0)


def test_given_an_arcade_with_rotation_then_return_rotation(
        mocker):
    givenThisFileContent(mocker, "arkangc,fbneo,arcade:224@59.185606,0,0,1")
    config = CRTConfigParser().findArcadeGame("arkangc","fbneo")
    assert config == ("fbneo", "arcade:224@59.185606", 0, 0, 1)
