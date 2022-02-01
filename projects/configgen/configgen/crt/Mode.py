import re


class Mode:
    def __init__(self, timings, refresh_rate="60"):
        self.width = 0
        self.h_front_porch = 0
        self.h_sync = 0
        self.h_back_porch = 0
        self.height = 0
        self.v_front_porch = 0
        self.v_sync = 0
        self.v_back_porch = 0
        self.v_back_porch = 0
        self.framerate = 0
        self.interlaced = 0
        self.clock = 0
        self.ratio = 0
        self.refresh_rate = refresh_rate
        self.parse(timings)

    def parse(self, timings):
        mode_re = re.compile(r"(\d+) 1 (\d+) (\d+) (\d+) (\d+) 1 (\d+) (\d+) (\d+) 0 0 0 (\d+) (\d) (\d+) (\d)")
        match = mode_re.match(timings)
        if match:
            self.width = int(match.group(1))
            self.h_front_porch = int(match.group(2))
            self.h_sync = int(match.group(3))
            self.h_back_porch = int(match.group(4))
            self.height = int(match.group(5))
            self.v_front_porch = int(match.group(6))
            self.v_sync = int(match.group(7))
            self.v_back_porch = int(match.group(8))
            self.framerate = int(match.group(9))
            self.interlaced = int(match.group(10))
            self.clock = int(match.group(11))
            self.ratio = int(match.group(12))
        else:
            raise Exception("Invalid mode")

    def timings(self) -> str:
        return "{} 1 {} {} {} {} 1 {} {} {} 0 0 0 {} {} {} {}".format(self.width, self.h_front_porch,
                                                                      self.h_sync, self.h_back_porch,
                                                                      self.height, self.v_front_porch,
                                                                      self.v_sync, self.v_back_porch,
                                                                      self.framerate, self.interlaced,
                                                                      self.clock, self.ratio)

    def __eq__(self, o: object) -> bool:
        return isinstance(o, Mode) and o.timings() == self.timings() and o.refresh_rate == self.refresh_rate
