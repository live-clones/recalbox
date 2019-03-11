#!/usr/bin/env python
import subprocess
import os
import sys

import videoMode
import demoManager

proc = None

# return code when demo mode ends upon user request
USERQUIT = 0x33

# Set a specific video mode
def runCommand(command, args):
    global proc

    chosenMode = videoMode.setVideoMode(command.videomode, command.delay)

    command.env.update(os.environ)
    print("running command: "+" ".join(command.array))
    proc = subprocess.Popen(command.array, env=command.env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=command.cwdPath)
    demo = demoManager.DemoManager(proc, args)
    exitcode = -1
    try:
        out, err = proc.communicate()
        exitcode = proc.returncode
        sys.stdout.write(out)
        sys.stderr.write(err)
    except:
        print("emulator exited")

    userQuit = demo.userQuitted()
    del demo

    if command.postExec is not None :
        command.postExec()

    if chosenMode != 'default':
        videoMode.setPreffered()

    return USERQUIT if userQuit else exitcode
