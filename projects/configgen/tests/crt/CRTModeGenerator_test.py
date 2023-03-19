import dataclasses

import pytest

from configgen.crt.CRTModeGenerator import CRTModeGenerator
from configgen.crt.Mode import Mode
from configgen.crt.MonitorRange import MonitorRange

standard15KhzMonitor = MonitorRange(15625, 15750, 49.50, 65.00, 2.000, 4.700, 8.000, 0.064, 0.192, 1.024, 0, 0, 192,
                                    288, 448, 576)
extended15KhzMonitor = MonitorRange(15500, 15750, 49.50, 65.00, 2.000, 4.700, 8.000, 0.064, 0.192, 1.024, 0, 0, 192,
                                    288, 448, 576)
standard31KhzMonitor = MonitorRange(31400, 31600, 100, 130, 0.671, 2.683, 3.353, 0.034, 0.101, 0.436, 0, 0, 200, 256, 0,
                                    0)


def test_given_a_out_of_range_sync_then_throws():
    with pytest.raises(Exception):
        CRTModeGenerator().generate(320, 240, 120, standard15KhzMonitor)


def test_given_a_out_of_range_progressive_vres_then_throws():
    with pytest.raises(Exception):
        CRTModeGenerator().generate(320, 320, 60, standard15KhzMonitor)


def test_given_a_out_of_range_interlaced_vres_then_throws():
    with pytest.raises(Exception):
        CRTModeGenerator().generate(320, 720, 60, standard15KhzMonitor, interlaced=True)


def test_given_a_320x240p_mode_then_generate():
    mode = CRTModeGenerator().generate(320, 240, 60, standard15KhzMonitor)
    assert mode == Mode("320 1 16 32 56 240 1 2 3 16 0 0 0 60 0 6639840 1", "60")


def test_given_a_1920x240pat60_on_extended_mode_then_generate():
    mode = CRTModeGenerator().generate(1920, 240, 60, extended15KhzMonitor)
    assert mode == Mode("1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1", "60")


def test_given_already_existing_standard_modes_before_migration_then_create_same_modes():
    modes = [
        [1920, 288, 50, "1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"],
        [1920, 224, 60, "1920 1 80 184 312 224 1 10 3 24 0 0 0 60 0 39087360 1"],
        [1920, 288, 50, "1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"],
        [1920, 240, 50, "1920 1 80 184 312 240 1 28 3 42 0 0 0 50 0 39062400 1"],
        [1920, 240, 60.0988, "1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 39001717 1"],
        [1920, 288, 50.01, "1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39070212 1"],
        [1920, 224, 59.185606, "1920 1 80 184 312 224 1 12 3 26 0 0 0 59 0 39147727 1"],
        [1920, 288, 49.701459, "1920 1 80 184 312 288 1 5 3 19 0 0 0 49 0 39077275 1"],
        [1920, 288, 49.701459, "1920 1 80 184 312 288 1 5 3 19 0 0 0 49 0 39077275 1"],
        [1920, 240, 60, "1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1"],
        [1920, 288, 50, "1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"],
        [1920, 240, 60, "1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1"],
        [1920, 288, 49.860759671615, "1920 1 80 184 312 288 1 4 3 19 0 0 0 49 0 39078071 1"],
        [1920, 288, 50.320232, "1920 1 80 184 312 288 1 3 3 17 0 0 0 50 0 39061382 1"],
        [1920, 240, 59.727500569606, "1920 1 80 184 312 240 1 2 3 17 0 0 0 59 0 39058918 1"],
        [1920, 224, 59.92274, "1920 1 80 184 312 224 1 10 3 24 0 0 0 59 0 39037028 1"],
        [1920, 224, 59.94, "1920 1 80 184 312 224 1 10 3 24 0 0 0 59 0 39048272 1"],
        [1920, 240, 59.82, "1920 1 80 184 312 240 1 2 3 17 0 0 0 59 0 39119408 1"],
        [1920, 240, 59.826, "1920 1 80 184 312 240 1 2 3 17 0 0 0 59 0 39123332 1"],
        [1920, 288, 49.761, "1920 1 80 184 312 288 1 5 3 19 0 0 0 49 0 39124088 1"],
        [1920, 224, 59.92, "1920 1 80 184 312 224 1 10 3 24 0 0 0 59 0 39035243 1"]
    ]

    for mode in modes:
        assert CRTModeGenerator().generate(mode[0], mode[1], mode[2], standard15KhzMonitor).timings() == mode[3]


def test_given_sheet_progressives_extended_modes_then_create_same_modes():
    modes = [
        [1920, 240, 60, "1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1"],
        [1920, 240, 60, "1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1"],
        [1920, 240, 59.94, "1920 1 80 184 312 240 1 1 3 16 0 0 0 59 0 38898662 1"],
        [1920, 240, 59.922751013551, "1920 1 80 184 312 240 1 1 3 16 0 0 0 59 0 38887468 1"],
        [1920, 240, 59.92274, "1920 1 80 184 312 240 1 1 3 16 0 0 0 59 0 38887461 1"],
        [1920, 240, 59.922743, "1920 1 80 184 312 240 1 1 3 16 0 0 0 59 0 38887463 1"],
    ]

    for mode in modes:
        assert CRTModeGenerator().generate(mode[0], mode[1], mode[2], extended15KhzMonitor).timings() == mode[3]


def test_given_a_640x480_interlaced_mode_then_generate():
    mode = CRTModeGenerator().generate(640, 480, 60, standard15KhzMonitor, interlaced=True)
    assert mode == Mode("640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1", "60")


def test_given_a_1920x240pat120Hz_then_generate_mode():
    mode = CRTModeGenerator().generate(1920, 240, 120, standard31KhzMonitor)
    assert mode == Mode("1920 1 48 208 256 240 1 4 3 15 0 0 0 120 0 76462080 1", "60")

# def test_generate():
#     mode = CRTModeGenerator().generate(400, 224, 60, extended15KhzMonitor)
#     assert mode == ""
