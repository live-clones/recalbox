from os.path import exists

from configgen.Emulator import Emulator
from configgen.crt.CRTConfigParser import CRTScreenType
from configgen.crt.CRTTypes import CRTResolution
from configgen.crt.Mode import Mode
from configgen.utils.recallog import recallog


class CRTModeOffsetter:

    switchFile = "/sys/devices/platform/recalboxrgbdual/dipswitch-50hz/value"

    def findOffsetsFromMode(self, mode: Mode, system: Emulator) -> [int, int]:
        resolution = mode.extractCRTResolution()
        if mode:
            return system.CRTHorizontalOffset(resolution), system.CRTVerticalOffset(resolution)
        return [0, 0]

    def processMode(self, mode: Mode, system: Emulator) -> Mode:
        # Todo remove when we will have a specific screen for pal calibration
        horizontal, vertical = self.findOffsetsFromMode(mode, system)
        # if abs(50 - mode.framerate) < 2:
        #     if pal_horizontal_offset != 0 or pal_vertical_offset != 0:
        #         # Pal offset set (may be forced 50Hz), so we use values
        #         horizontal = pal_horizontal_offset
        #         vertical = pal_vertical_offset
        #     else:
        #         # Here we have no overload of pal offset, so we adapt as we can
        #         horizontal += 3
        horizontal *= mode.width // 320
        min_v_porch = 2 if mode.interlaced else 1

        if mode.h_front_porch - horizontal < 1:
            horizontal = mode.h_front_porch - 1
        if mode.h_front_porch - horizontal > mode.h_back_porch + horizontal:
            horizontal = -(mode.h_back_porch - mode.h_front_porch) // 2
        if mode.v_front_porch - vertical < min_v_porch:
            vertical = mode.v_front_porch - min_v_porch
        if mode.v_front_porch - vertical > mode.v_back_porch + vertical:
            vertical = -(mode.v_back_porch - mode.v_front_porch) // 2
        mode.h_front_porch -= horizontal
        mode.h_back_porch += horizontal
        mode.v_front_porch -= vertical
        mode.v_back_porch += vertical

        recallog("Offseting mode: {} {}".format(horizontal, vertical), log_type="CRT")
        return mode
