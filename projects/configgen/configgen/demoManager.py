#!/usr/bin/env python
import threading
import os
import struct

from configgen.demoInfo import demoInformation

def Log(txt: str):
    print(txt)
    #with open("/recalbox/share/system/out.txt", "a") as f:
    #    f.write(txt + '\n')
    #    f.flush()

class InputEvents:

    def __init__(self):
        pass

    NONE = 0   # No event
    PLAY = 1   # The user pressed start and want to play
    OTHER = 2  # Any other action


class InputEventManager:

    EVENT_MAXIMUM = 32
    EVENT_FORMAT = "llHHI"
    EVENT_SIZE   = struct.calcsize(EVENT_FORMAT)

    def __init__(self, demoStartButtons):
        from typing import List, Union
        from typing import IO
        self.fileDescriptors: List[Union[IO, None]] = [None] * self.EVENT_MAXIMUM
        self.eventFileNameFlags = 0
        self.startMap = demoStartButtons
        self.eventToStart = dict()
        self.updateAvailableEvents()

    def __exit__(self):
        for i in range(0, self.EVENT_MAXIMUM - 1):
            self.closeFileDescriptor(i)

    @staticmethod
    def scanAvailableEvents():
        result = 0
        for i in range(0, InputEventManager.EVENT_MAXIMUM - 1):
            fileName = "/dev/input/event" + str(i)
            if os.path.exists(fileName):
                result = result | (1 << i)
        return result

    def updateAvailableEvents(self):
        newFlags = self.scanAvailableEvents()
        if newFlags != self.eventFileNameFlags:
            self.eventFileNameFlags = newFlags
            for i in range(0, self.EVENT_MAXIMUM - 1):
                if (newFlags & (1 << i)) != 0:
                    self.openFileDescriptor(i)
        #Log("Scan: "+hex(newFlags))
        return newFlags

    def openFileDescriptor(self, index):
        self.closeFileDescriptor(index)
        try:
            name = "/dev/input/event" + str(index)
            self.fileDescriptors[index] = open(name, "rb")
            # Configure NON blocking I/O
            fcntl = __import__("fcntl")
            fcntl.fcntl(self.fileDescriptors[index], fcntl.F_SETFL, fcntl.fcntl(self.fileDescriptors[index], fcntl.F_GETFL) | os.O_NONBLOCK)
            Log("Opened " + name)
            if name in self.startMap:
                self.eventToStart[index] = int(self.startMap[name])
        except IOError:
            Log("Open error")
            pass

    def closeFileDescriptor(self, index):
        try:
            if self.fileDescriptors[index] is not None:
                self.fileDescriptors[index].close()
            Log("Closed " + "/dev/input/event" + str(index))
        except IOError:
            Log("close error")
            pass

    def getEvent(self, index):
        try:
            return self.fileDescriptors[index].read(self.EVENT_SIZE)
        except IOError:
            return None

    def hasUserEvent(self):
        try:
            flags = self.updateAvailableEvents()
            for i in range(0, self.EVENT_MAXIMUM - 1):
                if (flags & (1 << i)) != 0:
                    event = self.getEvent(i)
                    while event:
                        (_, _, eventType, eventCode, eventValue) = struct.unpack(self.EVENT_FORMAT, event)
                        #if eventType != 0 or eventCode != 0 or eventValue != 0:
                        #    Log("From {} - Type {} - Code {} - Value {}".format(i, eventType, eventCode, eventValue))
                        # mouse button UP or keyboard's key UP or pad button UP
                        if eventType == 1 and eventValue == 0:
                            # Keyboard: Return or pad Return
                            if eventCode == 96 or eventCode == 28:
                                return InputEvents.PLAY
                            if i in self.eventToStart:
                                if eventCode == self.eventToStart[i]:
                                    return InputEvents.PLAY
                            return InputEvents.OTHER
                        # mouse move
                        if eventType == 2 and eventCode in (0, 1):
                            return InputEvents.OTHER
                        # dpad returned in neutral position
                        if eventType == 3 and eventValue == 0:
                            return InputEvents.OTHER
                        # do not check analog joysticks since they may generate unexpected micro-moves events
                        event = self.getEvent(i)
        except IOError:
            pass
        return InputEvents.NONE


class DemoTimer(threading.Thread):

    def __init__(self, proc, duration, outScreenDuration, demoStartButtons):
        threading.Thread.__init__(self)
        self.inputs = InputEventManager(demoStartButtons)
        self.duration = duration
        self.outScreenDuration = outScreenDuration
        self.userQuit = False
        self.userWannaPlay = False
        self.proc = proc

    def __del__(self):
        del self.inputs

    def userQuitted(self):
        return self.userQuit

    def userWantedToPlay(self):
        return self.userWannaPlay

    def terminateProcess(self, refresh, time):
        try:
            self.proc.terminate()
            Log("Terminated")
        except Exception as ex:
            Log("Error terminating process: {}".format(ex))

        # Wait for the process to quit
        if self.proc.poll() is None:
            duration = 10 * int(1.0 / refresh) # 10 seconds waiting for process death
            Log("Wait for terminate")
            while duration > 0:
                duration -= 1
                time.sleep(refresh)
                if self.proc.poll() is not None:
                    break
            # Still alive?
            if self.proc.poll() is None:
                Log("Kill!")
                try:
                    import signal
                    self.proc.send_signal(signal.SIGKILL)
                except Exception as ex:
                    Log("Error sending kill signal: {}".format(ex))

    def run(self):
        import time
        refresh = 0.2  # refresh time in second
        outDuration = self.outScreenDuration * int(1.0 / refresh)
        duration = (self.duration * int(1.0 / refresh)) + outDuration
        outScreen = None
        try:
            while duration > 0:
                duration -= 1
                #Log("Duration: {}".format(duration))
                time.sleep(refresh)
                # User action?
                event = self.inputs.hasUserEvent()
                if event == InputEvents.PLAY:
                    self.userWannaPlay = True
                    Log("Demo mode ends because the user want to play the current game")
                    break
                if event == InputEvents.OTHER:
                    self.userQuit = True
                    Log("Demo mode ends upon user request")
                    break
                # Not yet in outscreen?
                if outScreen is None:
                    # Process quitted prematurely?
                    if self.proc.poll() is not None:
                        Log("Emulator quitted prematurely")
                        break
                    # Outscreen?
                    if duration < outDuration:
                        self.terminateProcess(refresh, time)
                        Log("Need info screen")
                        try:
                            outScreen = demoInformation()
                            outScreen.display()
                        except Exception as ex:
                            Log("Error diplaying demo info screen: {}".format(ex))
                            duration = 0
        except Exception as ex:
            Log("Demo exited unexpectedly: {}".format(ex))

        if outScreen is not None:
            del outScreen
        else:
            self.terminateProcess(refresh, time)


class DemoManager:

    def __init__(self, proc, args, demoStartButtons):
        self.thread = None
        if args.demo:
            # get duration
            duration = args.demoduration
            if duration is None:
                duration = 120  # 2mn

            # get out-screen duration
            outScreenDuration = args.demoinfoduration
            if outScreenDuration is None:
                outScreenDuration = 6  # 6s

            # run thread
            self.thread = DemoTimer(proc, duration, outScreenDuration, demoStartButtons)
            self.thread.start()

    def __del__(self):
        if self.thread is not None:
            self.thread.join()
            del self.thread

    def userQuitted(self):
        if self.thread is not None:
            return self.thread.userQuitted()
        return False

    def userWannaPlay(self):
        if self.thread is not None:
            return self.thread.userWantedToPlay()
        return False
