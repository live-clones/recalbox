from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.crt.Mode import Mode


def test_given_mode_list_and_offset_zero_nothing_move(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, 0, 0) is mode


def test_given_mode_and_offset_10_will_move_horizontally(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, -10, 0) == Mode("1920 1 90 184 302 240 1 2 3 16 0 0 0 60 0 39087360 1")


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
    assert CRTModeOffsetter().processMode(mode, 200, 0) == Mode("1920 1 1 184 391 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_too_low_will_make_maximum_porch_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    assert CRTModeOffsetter().processMode(mode, -200, 0) == Mode(
        "1920 1 196 184 196 240 1 2 3 16 0 0 0 60 0 39087360 1")
