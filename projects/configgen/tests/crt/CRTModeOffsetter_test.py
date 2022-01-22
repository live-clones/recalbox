from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.crt.Mode import Mode


def test_given_mode_list_and_offset_zero_nothing_move(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, 0, 0) is mode


def test_given_mode_and_offset_10_will_move_horizontally(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, -2, 0) == Mode("1920 1 92 184 300 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_mode_and_offset_1_will_move_vertically(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, 0, -1) == Mode("1920 1 80 184 312 240 1 3 3 15 0 0 0 60 0 39087360 1")


def test_given_vertical_offset_too_high_will_not_make_porch_under_minimal_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, 0, 20) == Mode("1920 1 80 184 312 240 1 1 3 17 0 0 0 60 0 39087360 1")


def test_given_vertical_offset_too_low_will_make_maximum_porch_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, 0, -10) == Mode(
        "1920 1 80 184 312 240 1 9 3 9 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_too_high_will_not_make_porch_under_minimal_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, 33, 0) == Mode("1920 1 1 184 391 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_too_low_will_make_maximum_porch_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, -33, 0) == Mode(
        "1920 1 196 184 196 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_will_adapt_to_1920_horizontal_resolution_by_x6(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, -5, 0) == Mode("1920 1 110 184 282 240 1 2 3 16 0 0 0 60 0 39087360 1")

def test_given_horizontal_offset_will_adapt_to_320_horizontal_resolution(mocker):
    mode = Mode("320 1 4 30 46 240 1 4 5 14 0 0 0 60 0 6400000 1")
    assert CRTModeOffsetter().processMode(mode, -2, -2) == Mode("320 1 6 30 44 240 1 6 5 12 0 0 0 60 0 6400000 1")

def test_given_horizontal_offset_will_adapt_to_640_horizontal_resolution_by_x2(mocker):
    mode = Mode("640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1")
    assert CRTModeOffsetter().processMode(mode, -2, -2) == Mode("640 1 28 64 100 480 1 5 6 32 0 0 0 60 1 13054080 1")
