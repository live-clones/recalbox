#!/usr/bin/env python
import subprocess
import os
import sys

import videoMode
import demoManager

proc = None

# return code when demo mode ends upon user request
USERQUIT = 0x33
USERWANNAPLAY = 0x77

# Set a specific video mode
def runCommand(command, args, demoStartButtons, recalboxSettings):
    global proc

    chosenMode = videoMode.setVideoMode(command.videomode, command.delay)

    command.env.update(os.environ)
    print("running command: "+" ".join(command.array))
    proc = subprocess.Popen(command.array, env=command.env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=command.cwdPath)
    demo = demoManager.DemoManager(proc, args, demoStartButtons)
    exitcode = -1
    try:
        out, err = proc.communicate()
        exitcode = proc.returncode
        sys.stdout.write(out)
        sys.stderr.write(err)
    except:
        print("emulator exited")

    userQuit = demo.userQuitted()
    userWannaPlay = demo.userWannaPlay()
    del demo

    if command.postExec is not None :
        command.postExec()

    if chosenMode != 'default':
        videoMode.setPreffered(recalboxSettings)

    if userQuit: return USERQUIT
    if userWannaPlay: return USERWANNAPLAY
    return exitcode
