#!/usr/bin/env python
proc = None

# return code when demo mode ends upon user request
USERQUIT = 0x33
USERWANNAPLAY = 0x77

# Set a specific video mode
def runCommand(command, args, demoStartButtons, recalboxSettings, fixedScreenSize):
    global proc

    # Switch video mode if required
    chosenMode = None
    if not fixedScreenSize:
        import utils.videoMode as videoMode
        chosenMode = videoMode.setVideoMode(command.videomode, command.delay)

    # Update environment
    import os
    command.env.update(os.environ)
    print("running command: "+" ".join(command.array))

    # Run emulator process
    import subprocess
    proc = subprocess.Popen(command.array, env=command.env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=command.cwdPath)

    # Signal handling
    def signal_handler(_, __):
        print('Exiting')
        if proc:
            print('killing runner.proc')
            proc.kill()

    # Get signals
    import signal
    signal.signal(signal.SIGINT, signal_handler)

    # Run demo mode is required
    demo = None
    if args.demo:
        import demoManager
        demo = demoManager.DemoManager(proc, args, demoStartButtons)

    exitcode = -1
    try:
        out, err = proc.communicate()
        exitcode = proc.returncode
        if args.verbose:
            import sys
            sys.stdout.write(out)
            sys.stderr.write(err)
    except Exception as e:
        print("Emulator exited unexpectedly!\nException: {}".format(e))

    userQuit = False
    userWannaPlay = False
    if args.demo:
        userQuit = demo.userQuitted()
        userWannaPlay = demo.userWannaPlay()
        del demo

    if command.postExec is not None :
        command.postExec()

    if not fixedScreenSize:
        if chosenMode != 'default':
            import utils.videoMode as videoMode
            videoMode.setPreffered(recalboxSettings)

    if userQuit: return USERQUIT
    if userWannaPlay: return USERWANNAPLAY
    return exitcode

