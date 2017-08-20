#!/usr/bin/env python
import subprocess
import os
import sys

import videoMode

proc = None

# Set a specific video mode
def runCommand(command):
    global proc

    videoMode.setVideoMode(command.videomode, command.delay)

    command.env.update(os.environ)
    proc = subprocess.Popen(command.array, env=command.env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=command.cwdPath)
    exitcode = -1
    try:
        out, err = proc.communicate()
        exitcode = proc.returncode
        sys.stdout.write(out)
        sys.stderr.write(err)
    except:
        print("emulator exited")
    
    if command.postExec is not None :
        command.postExec()
    
    if command.videomode != 'default':
        videoMode.setPreffered()

    return exitcode
