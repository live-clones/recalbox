from configgen.crt.Mode import Mode
from configgen.crt.MonitorRange import MonitorRange


class CRTModeGenerator:

    def modeline_get_line_params(self, t_mode: Mode, m_range: MonitorRange) -> int:

        h_front_porch_min: float = m_range.h_front_porch * .90
        h_sync_pulse_min: float = m_range.h_sync_pulse * .90
        h_back_porch_min = m_range.h_back_porch * .90

        line_time = 1 / t_mode.hfreq * 1000000

        hh = round(t_mode.hactive / 8, 0)
        hs = he = ht = 1

        while True:
            char_time = line_time / (hh + hs + he + ht)
            if hs * char_time < h_front_porch_min or abs((hs + 1) * char_time - m_range.h_front_porch) < abs(
                    hs * char_time - m_range.h_front_porch):
                hs += 1
            if he * char_time < h_sync_pulse_min or abs((he + 1) * char_time - m_range.h_sync_pulse) < abs(
                    he * char_time - m_range.h_sync_pulse):
                he += 1
            if ht * char_time < h_back_porch_min or abs((ht + 1) * char_time - m_range.h_back_porch) < abs(
                    ht * char_time - m_range.h_back_porch):
                ht += 1
            new_char_time = line_time / (hh + hs + he + ht)
            if new_char_time == char_time:
                break

        hhi = (hh + hs) * 8
        hhe = (hh + hs + he) * 8
        hht = (hh + hs + he + ht) * 8

        t_mode.h_front_porch = int(hhi - t_mode.width)
        t_mode.h_sync = int(hhe - t_mode.width - t_mode.h_front_porch)
        t_mode.h_back_porch = int(hht - t_mode.width - t_mode.h_front_porch - t_mode.h_sync)
        return hht

    def total_lines_for_yres(self, yres: int, vfreq: float, m_range: MonitorRange, scan_factor: int) -> int:
        vvt = max(yres / scan_factor + round(
            vfreq * yres / (scan_factor * (1.0 - vfreq * m_range.vertical_blank)) * m_range.vertical_blank, 0), 1)
        while (vfreq * vvt < m_range.h_freq_min) and (vfreq * (vvt + 1) < m_range.h_freq_max):
            vvt += 1
        return int(vvt)

    def generate(self, width: int, height: int, framerate: float, m_range: MonitorRange,
                 interlaced: bool = False) -> Mode:
        mode = Mode()
        mode.width = width
        mode.height = height
        mode.framerate = framerate
        if mode.framerate < 65:
            mode.emulator_refresh = f"{framerate}"
        else:
            mode.emulator_refresh = "60"

        scan_factor = 1

        # Check ranges
        if framerate < m_range.v_freq_min or framerate > m_range.v_freq_max:
            raise Exception("Not in v range")
        if interlaced:
            if height < m_range.interlaced_lines_min or height > m_range.interlaced_lines_max:
                raise Exception("Height > max interlaced lines")
            scan_factor = 2
            mode.interlaced = 1
        else:
            if height < m_range.progressive_lines_min or height > m_range.progressive_lines_max:
                raise Exception("Height > max progressive lines")

        vvt_ini = self.total_lines_for_yres(height, framerate, m_range, scan_factor) + (0.5 if interlaced else 0)
        vtotal = vvt_ini * scan_factor

        mode.hactive = width
        mode.vactive = height
        mode.vfreq = framerate
        mode.hfreq = mode.vfreq * vvt_ini

        htotal = self.modeline_get_line_params(mode, m_range)

        mode.clock = int(htotal * mode.hfreq)

        v_blank_lines = int(mode.hfreq * m_range.vertical_blank) + (0.5 if interlaced else 0)
        v_margin = (vtotal - mode.vactive - v_blank_lines * scan_factor) / 2
        mode.v_front_porch = int(max(round(mode.hfreq * m_range.v_front_porch * scan_factor + v_margin, 0), 1))
        mode.v_sync = int(max(round(mode.hfreq * m_range.v_sync_pulse * scan_factor, 0), 1))
        mode.v_back_porch = int(vtotal - mode.vactive - mode.v_front_porch - mode.v_sync)

        mode.vfreq = (mode.hfreq / vtotal) * scan_factor
        mode.ratio = 1

        return mode
