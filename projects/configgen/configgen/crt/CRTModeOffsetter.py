from os.path import exists

from configgen.crt.Mode import Mode
from configgen.utils.recallog import recallog


class CRTModeOffsetter:

    switchFile = "/sys/devices/platform/recalboxrgbdual/dipswitch-50hz/value"

    def processMode(self, mode: Mode, ntsc_horizontal_offset: int, ntsc_vertical_offset: int, pal_horizontal_offset: int, pal_vertical_offset: int) -> Mode:
        # Todo remove when we will have a specific screen for pal calibration
        horizontal = ntsc_horizontal_offset
        vertical = ntsc_vertical_offset
        if abs(50 - mode.framerate) < 2:
            if pal_horizontal_offset != 0 or pal_vertical_offset != 0:
                # Pal offset set (may be forced 50Hz), so we use values
                horizontal = pal_horizontal_offset
                vertical = pal_vertical_offset
            else:
                # Here we have no overload of pal offset, so we adapt as we can
                horizontal += 3
        horizontal *= mode.width // 320
        if mode.h_front_porch - horizontal < 1:
            horizontal = mode.h_front_porch - 1
        if mode.h_front_porch - horizontal > mode.h_back_porch + horizontal:
            horizontal = -(mode.h_back_porch - mode.h_front_porch) // 2
        if mode.v_front_porch - vertical < 1:
            vertical = mode.v_front_porch - 1
        if mode.v_front_porch - vertical > mode.v_back_porch + vertical:
            vertical = -(mode.v_back_porch - mode.v_front_porch) // 2
        mode.h_front_porch -= horizontal
        mode.h_back_porch += horizontal
        mode.v_front_porch -= vertical
        mode.v_back_porch += vertical

        recallog("Offseting mode: {} {}".format(horizontal, vertical), log_type="CRT")
        return mode
