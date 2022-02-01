#!/usr/bin/env python
import pytest

import configgen.utils.videoMode as videomode


@pytest.mark.parametrize(
    "given_videomode,expected_modeline",
    [
        ("CEA 4 HDMI", "tvservice -e 'CEA 4 HDMI'"),
        ("hdmi_cvt 800 480 60 6", "vcgencmd hdmi_cvt 800 480 60 6 && tvservice -e 'DMT 87'"),
        ("hdmi_timings 640 1 100 100 100 480 1 13 13 13 0 0 0 65 0 31800000 1", "vcgencmd hdmi_timings 640 1 100 100 100 480 1 13 13 13 0 0 0 65 0 31800000 1 && tvservice -e 'DMT 87'")
    ],
)
def test_createVideoModeLine(given_videomode, expected_modeline):
    generated_modeline = videomode.createVideoModeLine(given_videomode)
    assert generated_modeline == expected_modeline
