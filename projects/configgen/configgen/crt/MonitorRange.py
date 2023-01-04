import dataclasses


class MonitorRange:

    def __init__(self, h_freq_min: int,
                 h_freq_max: int,
                 v_freq_min: float,
                 v_freq_max: float,

                 h_front_porch: float,
                 h_sync_pulse: float,
                 h_back_porch: float,
                 v_front_porch: float,
                 v_sync_pulse: float,
                 v_back_porch: float,
                 h_sync_polarity: int,
                 v_sync_polarity: int,
                 progressive_lines_min: int,
                 progressive_lines_max: int,
                 interlaced_lines_min: int,
                 interlaced_lines_max: int) -> None:
        self.h_freq_min: int = h_freq_min
        self.h_freq_max: int = h_freq_max
        self.v_freq_min: float = v_freq_min
        self.v_freq_max: float = v_freq_max

        self.h_front_porch: float = h_front_porch
        self.h_sync_pulse: float = h_sync_pulse
        self.h_back_porch: float = h_back_porch
        self.v_front_porch: float = v_front_porch / 1000
        self.v_sync_pulse: float = v_sync_pulse / 1000
        self.v_back_porch: float = v_back_porch / 1000
        self.h_sync_polarity: int = h_sync_polarity
        self.v_sync_polarity: int = v_sync_polarity
        self.progressive_lines_min: int = progressive_lines_min
        self.progressive_lines_max: int = progressive_lines_max
        self.interlaced_lines_min: int = interlaced_lines_min
        self.interlaced_lines_max: int = interlaced_lines_max

        self.vertical_blank: float = self.v_front_porch + self.v_sync_pulse + self.v_back_porch
