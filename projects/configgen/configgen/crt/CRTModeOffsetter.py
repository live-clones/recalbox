from os.path import exists

from configgen.crt.Mode import Mode
from configgen.utils.recallog import recallog


class CRTModeOffsetter:

    switchFile = "/sys/devices/platform/recalboxrgbdual/dipswitch-50hz/value"

    def processMode(self, mode: Mode, horizontal_offset: int, vertical_offset: int) -> Mode:
        # Todo remove when we will have a specific screen for pal calibration
        shouldOffsetPal = True
        if exists(self.switchFile):
            with open(self.switchFile) as f:
                content = f.read()
                if content == "0\n":
                    shouldOffsetPal = False
        if shouldOffsetPal and abs(50 - mode.framerate) < 2:
            horizontal_offset += 5
        horizontal_offset *= mode.width // 320
        if mode.h_front_porch - horizontal_offset < 1:
            horizontal_offset = mode.h_front_porch - 1
        if mode.h_front_porch - horizontal_offset > mode.h_back_porch + horizontal_offset:
            horizontal_offset = -(mode.h_back_porch - mode.h_front_porch) // 2
        if mode.v_front_porch - vertical_offset < 1:
            vertical_offset = mode.v_front_porch - 1
        if mode.v_front_porch - vertical_offset > mode.v_back_porch + vertical_offset:
            vertical_offset = -(mode.v_back_porch - mode.v_front_porch) // 2
        mode.h_front_porch -= horizontal_offset
        mode.h_back_porch += horizontal_offset
        mode.v_front_porch -= vertical_offset
        mode.v_back_porch += vertical_offset

        recallog("Offseting mode: {} {}".format(horizontal_offset, vertical_offset), log_type="CRT")
        return mode
