#!/usr/bin/env python
import os
import sys
import recalboxFiles
import re
import time
from settings.unixSettings import UnixSettings
from utils.recallog import recallog
import subprocess
import json

# Set a specific video mode
def setVideoMode(videoMode, delay):
    # The user mentionned default for the videomode
    if videoMode == "default":
        return

    if videoMode == "auto":
        videoSetting = autoMode()
        # autoMode can have replied "default"
        if videoSetting == "default":
            return
        cmd = createVideoModeLine(videoSetting)
    else :
        cmd = createVideoModeLine(videoMode)

    os.system(cmd)
    time.sleep(delay)

def createVideoModeLine(videoMode):
    # pattern (CEA|DMT) [0-9]{1,2} HDMI
    if re.match("^(CEA|DMT) [0-9]{1,2}( HDMI)?$", videoMode):
        return "tvservice -e '{}'".format(videoMode)
    if re.match("^hdmi_cvt [\d\s]{10,20}$", videoMode):
        return "vcgencmd {} && tvservice -e 'DMT 87'".format(videoMode)
    if re.match("^hdmi_timings [\d\s]{48,58}$", videoMode):
        return "vcgencmd {} && tvservice -e 'DMT 87'".format(videoMode)
    recallog('auto mode -> had to set default')
    return ''

# Set a specific video mode
def isSupported(group="CEA", mode='', drive="HDMI"):
    groups = ['CEA', 'DMT']
    if group not in groups:
        sys.exit("{} is an unknown group. Can't switch to {} {} {}".format(group, group, mode, drive))

    drives = ['HDMI', 'DVI']
    if drive not in drives:
        sys.exit("{} is an unknown drive. Can't switch to {} {} {}".format(drive, group, mode, drive))
        
    proc = subprocess.Popen(["tvservice -j -m {}".format(group)], stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()
    #print "program output:", out
    tvmodes = json.loads(out)

    for tvmode in tvmodes:
        if tvmode["code"] == int(mode):
            return True

    recallog("The resolution for '{} {} {}' is not supported by your monitor".format(group, mode, drive))
    return False

# Switch to prefered mode
def setPreffered():
    recalSettings = UnixSettings(recalboxFiles.recalboxConf)
    esVideoMode = recalSettings.load('system.es.videomode')
    if esVideoMode is None:
        os.system("tvservice -p")
    else:
        setVideoMode(esVideoMode)

# Check auto mode, return the expected value
def autoMode():
    #Resolutions to handle :
    #state 0x40001 [NTSC 4:3], 720x480 @ 60.00Hz, interlaced
    #state 0x12000a [HDMI CEA (16) RGB lim 16:9], 1920x1080 @ 60.00Hz, progressive
    #state 0x400000 [LCD], 800x480 @ 0.00Hz, progressive
    #state 0x400000 [LCD], 320x240 @ 0.00Hz, progressive
    #state 0x120006 [DVI DMT (58) RGB full 16:10], 1680x1050 @ 60.00Hz, progressive
    
    proc = subprocess.Popen(["tvservice -s"], stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()
    
    # This one does match what i need ! Everything ! Passes the 5 cases listed above
    regex = r".*\[(.*[^0-9:]+) ?([0-9]{1,2})?:?([0-9]{1})?\], ([0-9]{3,4})x([0-9]{3,4}) @ ([0-9.]{,6})Hz, (progressive|interlaced).*"

    matches = re.match(regex, out)
    if not matches: 
        # We should log the out var and log that it do'sn't match any known pattern
        recallog('auto mode -> had to set default')
        return "default"
    details, wRatio, hRatio, width, height, refreshRate, progressiveOrInterlace = matches.groups()
    
    # Now the magic
    # if the screen supports CEA 4, and its format is at least 16:9, go for CEA 4
    if isSupported('CEA', 4, 'HDMI') and wRatio == '16':
        recallog("auto mode -> CEA 4 HDMI is valid")
        return "CEA 4 HDMI"
    # Otherwise (composite output, 5:4 screens, mini DPI screens etc ...) -> default
    else :
        recallog("auto mode -> CEA 4 HDMI not supported, fallback to default")
        return "default"
