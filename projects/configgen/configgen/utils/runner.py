#!/usr/bin/env python
proc = None

# return code when demo mode ends upon user request
USERQUIT = 0x33
USERWANNAPLAY = 0x77

# Set a specific video mode
def runCommand(command, args, demoStartButtons, recalboxSettings, fixedScreenSize):

    # Switch video mode if required
    videoMode = None
    chosenMode = None
    if not fixedScreenSize:
        videoMode = __import__("utils.videoMode", fromlist=[None])
        chosenMode = videoMode.setVideoMode(command.videomode, command.delay)

    # Update environment
    os = __import__("os")
    command.env.update(os.environ)
    print("running command: "+" ".join(command.array))

    # Get signals
    signal = __import__("signal")
    signal.signal(signal.SIGINT, signal_handler)

    # Run eulator process
    subprocess = __import__("subprocess")
    proc = subprocess.Popen(command.array, env=command.env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=command.cwdPath)

    # Run demo mode is required
    demo = None
    if args.demo:
        demoManager = __import__("demoManager")
        demo = demoManager.DemoManager(proc, args, demoStartButtons)

    exitcode = -1
    try:
        out, err = proc.communicate()
        exitcode = proc.returncode
        sys = __import__("sys")
        sys.stdout.write(out)
        sys.stderr.write(err)
    except:
        print("emulator exited")

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
            videoMode.setPreffered(recalboxSettings)

    if userQuit: return USERQUIT
    if userWannaPlay: return USERWANNAPLAY
    return exitcode


def signal_handler(_, __):
    print('Exiting')
    if proc:
        print('killing runner.proc')
        proc.kill()

