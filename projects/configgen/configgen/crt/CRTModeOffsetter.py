from configgen.crt.Mode import Mode
from configgen.utils.recallog import recallog


class CRTModeOffsetter:

    def processMode(self, mode: Mode, horizontal_offset: int, vertical_offset: int) -> Mode:
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
