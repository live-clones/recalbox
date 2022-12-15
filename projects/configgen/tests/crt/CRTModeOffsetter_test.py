from configgen.Emulator import Emulator, ExtraArguments
from configgen.crt.CRTConfigParser import CRTScreenType
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.crt.Mode import Mode
from configgen.settings.keyValueSettings import keyValueSettings


def createSystem(crt_verticaloffset_p1920x240at120: int = 0,
                 crt_horizontaloffset_p1920x240at120: int = 0,
                 crt_viewportwidth_p1920x240at120: int = 0,
                 crt_verticaloffset_p640x480: int = 0,
                 crt_horizontaloffset_p640x480: int = 0,
                 crt_viewportwidth_p640x480: int = 0,
                 crt_verticaloffset_i768x576: int = 0,
                 crt_horizontaloffset_i768x576: int = 0,
                 crt_viewportwidth_i768x576: int = 0,
                 crt_verticaloffset_i640x480: int = 0,
                 crt_horizontaloffset_i640x480: int = 0,
                 crt_viewportwidth_i640x480: int = 0,
                 crt_verticaloffset_p1920x288: int = 0,
                 crt_horizontaloffset_p1920x288: int = 0,
                 crt_viewportwidth_p1920x288: int = 0,
                 crt_verticaloffset_p1920x240: int = 0,
                 crt_horizontaloffset_p1920x240: int = 0,
                 crt_viewportwidth_p1920x240: int = 0,
                 crt_verticaloffset_p1920x224: int = 0,
                 crt_horizontaloffset_p1920x224: int = 0,
                 crt_viewportwidth_p1920x224: int = 0,
                 crtscreentype: CRTScreenType = CRTScreenType.k15):
    system = Emulator("fbneo", "retroarch", "fbneo")
    system.configure(keyValueSettings(""),
                     ExtraArguments("", "", "", "", "", "", "", "", "ntsc", "progressive", crtscreentype, "enabled",
                                    "auto",
                                    False,
                                    crt_verticaloffset_p1920x240at120,
                                    crt_horizontaloffset_p1920x240at120,
                                    crt_viewportwidth_p1920x240at120,
                                    crt_verticaloffset_p640x480,
                                    crt_horizontaloffset_p640x480,
                                    crt_viewportwidth_p640x480,
                                    crt_verticaloffset_i768x576,
                                    crt_horizontaloffset_i768x576,
                                    crt_viewportwidth_i768x576,
                                    crt_verticaloffset_i640x480,
                                    crt_horizontaloffset_i640x480,
                                    crt_viewportwidth_i640x480,
                                    crt_verticaloffset_p1920x288,
                                    crt_horizontaloffset_p1920x288,
                                    crt_viewportwidth_p1920x288,
                                    crt_verticaloffset_p1920x240,
                                    crt_horizontaloffset_p1920x240,
                                    crt_viewportwidth_p1920x240,
                                    crt_verticaloffset_p1920x224,
                                    crt_horizontaloffset_p1920x224,
                                    crt_viewportwidth_p1920x224))
    return system


def test_given_mode_list_and_offset_zero_nothing_move(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem()
    assert CRTModeOffsetter().processMode(mode, system) is mode


def test_given_240pmode_and_hoffset_minus2_will_move_horizontally(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-2)
    assert CRTModeOffsetter().processMode(mode, system) == Mode("1920 1 92 184 300 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_240pmode_and_voffset_minus1_will_move_vertically(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_verticaloffset_p1920x240=-1)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 80 184 312 240 1 3 3 15 0 0 0 60 0 39087360 1")


def test_given_vertical_offset_too_high_will_not_make_porch_under_minimal_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_verticaloffset_p1920x240=20)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 80 184 312 240 1 1 3 17 0 0 0 60 0 39087360 1")


def test_given_vertical_offset_too_low_will_make_maximum_porch_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_verticaloffset_p1920x240=-10)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 80 184 312 240 1 9 3 9 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_too_high_will_not_make_porch_under_minimal_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=33)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 1 184 391 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_too_low_will_make_maximum_porch_value(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-33)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 196 184 196 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_will_adapt_to_1920_horizontal_resolution_by_x6(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-5)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 110 184 282 240 1 2 3 16 0 0 0 60 0 39087360 1")


def test_given_horizontal_offset_will_adapt_to_320_horizontal_resolution(mocker):
    mode = Mode("320 1 4 30 46 240 1 4 5 14 0 0 0 60 0 6400000 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-2, crt_verticaloffset_p1920x240=-2)
    assert CRTModeOffsetter().processMode(mode, system) == Mode("320 1 6 30 44 240 1 6 5 12 0 0 0 60 0 6400000 1")


def test_given_horizontal_offset_will_adapt_to_640_horizontal_resolution_by_x2(mocker):
    mode = Mode("640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1")
    system = createSystem(crt_horizontaloffset_i640x480=-2, crt_verticaloffset_i640x480=-2)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "640 1 28 64 100 480 1 5 6 32 0 0 0 60 1 13054080 1")


def test_given_p1920x240_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-1, crt_verticaloffset_p1920x240=-1)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 86 184 306 240 1 3 3 15 0 0 0 60 0 39087360 1")


def test_given_p1920x224_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("1920 1 80 184 312 224 1 10 3 24 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x224=-1, crt_verticaloffset_p1920x224=-1)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 86 184 306 224 1 11 3 23 0 0 0 60 0 39087360 1")


def test_given_p1920x288_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1")
    system = createSystem(crt_horizontaloffset_p1920x288=-1, crt_verticaloffset_p1920x288=-1)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 86 184 306 288 1 5 3 17 0 0 0 50 0 39062400 1")


def test_given_i640x480_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1")
    system = createSystem(crt_horizontaloffset_i640x480=-2, crt_verticaloffset_i640x480=-2)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "640 1 28 64 100 480 1 5 6 32 0 0 0 60 1 13054080 1")


def test_given_i768x576_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("768 1 24 72 88 576 1 6 5 38 0 0 0 50 1 14875000 1")
    system = createSystem(crt_horizontaloffset_i768x576=-2, crt_verticaloffset_i768x576=-2)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "768 1 28 72 84 576 1 8 5 36 0 0 0 50 1 14875000 1")


def test_given_p640x480_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1")
    system = createSystem(crt_horizontaloffset_p640x480=-3, crt_verticaloffset_p640x480=-3)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "640 1 30 96 42 480 1 14 2 29 0 0 0 60 0 25452000 1")


def test_given_p1920x240at120_mode_and_offsets_then_modify_mode(mocker):
    mode = Mode("1920 1 48 208 256 240 1 4 3 15 0 0 0 120 0 76462080 1")
    system = createSystem(crtscreentype=CRTScreenType.k31, crt_horizontaloffset_p1920x240at120=-3,
                          crt_verticaloffset_p1920x240at120=-3)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 66 208 238 240 1 7 3 12 0 0 0 120 0 76462080 1")


def test_given_mode_lower_than_224p_then_use224p_offsets(mocker):
    mode = Mode("1920 1 80 184 312 220 1 10 3 24 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x224=-1, crt_verticaloffset_p1920x224=-1)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 86 184 306 220 1 11 3 23 0 0 0 60 0 39087360 1")


def test_given_mode_lower_than_240p_then_use240p_offsets(mocker):
    mode = Mode("1920 1 80 184 312 230 1 10 3 24 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-1, crt_verticaloffset_p1920x240=-1,
                          crt_horizontaloffset_p1920x224=0, crt_verticaloffset_p1920x224=0)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 86 184 306 230 1 11 3 23 0 0 0 60 0 39087360 1")


def test_given_mode_lower_than_288p_then_use240p_offsets(mocker):
    mode = Mode("1920 1 80 184 312 287 1 10 3 24 0 0 0 60 0 39087360 1")
    system = createSystem(crt_horizontaloffset_p1920x240=-1, crt_verticaloffset_p1920x240=-1,
                          crt_horizontaloffset_p1920x288=0, crt_verticaloffset_p1920x288=0)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "1920 1 86 184 306 287 1 11 3 23 0 0 0 60 0 39087360 1")



def test_given_interlaced_mode_and_offsets_then_do_not_get_under_2_for_vfrontporch(mocker):
    mode = Mode("768 1 24 72 88 576 1 6 5 38 0 0 0 50 1 14875000 1")
    system = createSystem(crt_verticaloffset_i768x576=+5)
    assert CRTModeOffsetter().processMode(mode, system) == Mode(
        "768 1 24 72 88 576 1 2 5 42 0 0 0 50 1 14875000 1")