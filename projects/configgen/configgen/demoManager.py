#!/usr/bin/env python
import threading
import time
import os
import signal
import fcntl
import struct

class InputEventManager:

    EVENT_FORMAT = "llHHI"
    EVENT_SIZE   = struct.calcsize(EVENT_FORMAT)

    def __init__(self):
        self.inputFiles = []
        for i in range(0,9):
            fileName = "/dev/input/event" + str(i)
            if os.path.exists(fileName):
                fileDescriptor = open(fileName, "rb")
                # Configure the NON blocking I/O
                fcntl.fcntl(fileDescriptor, fcntl.F_SETFL, fcntl.fcntl(fileDescriptor, fcntl.F_GETFL) | os.O_NONBLOCK)
                self.inputFiles.append(fileDescriptor)

    def __exit__(self):
        for f in self.inputFiles:
            f.close()

    def getEvent(self, fileDescriptor):
        try:
            return fileDescriptor.read(self.EVENT_SIZE)
        except IOError:
            return None

    def hasUserEvent(self):
        for fileDescriptor in self.inputFiles:
            event = self.getEvent(fileDescriptor)
            while event:
                (_, _, eventType, eventCode, eventValue) = struct.unpack(self.EVENT_FORMAT, event)
                # mouse button UP or keyboard's key UP or pad button UP
                if eventType == 1 and eventValue == 0:
                    return True
                # mouse move
                if eventType == 2 and eventCode in (0, 1):
                    return True
                # dpad returned in neutral position
                if eventType == 3 and eventValue == 0:
                    return True
                # do not check analog joysticks since they may generate unexpected micro-moves events
                #if eventType != 0 or eventCode != 0 or eventValue != 0:
                #    print("Type {} - Code {} - Value {}".format(eventType, eventCode, eventValue))
                event = self.getEvent(fileDescriptor)
        return False


class DemoTimer(threading.Thread):

    def __init__(self, proc, duration):
        threading.Thread.__init__(self)
        self.inputs = InputEventManager()
        self.duration = duration
        self.userQuit = False
        self.proc = proc

    def __del__(self):
        del self.inputs

    def userQuitted(self):
        return self.userQuit

    def run(self):
        refresh = 0.2  # refresh time in second
        duration = self.duration * int(1.0 / refresh)
        while duration > 0:
            duration -= 1
            time.sleep(refresh)
            # User action?
            if self.inputs.hasUserEvent():
                self.userQuit = True
                print("Demo mode ends upon user request")
                break
            # Process quitted prematurely?
            if self.proc.poll() is not None:
                print("Emulator quitted prematurely")
                break

        try:
            self.proc.terminate()
        except OSError:
            pass

        # Wait for the process to quit
        if self.proc.poll() is None:
            duration = 10 * int(1.0 / refresh) # 10 seconds waiting for process death
            while duration > 0:
                duration -= 1
                time.sleep(refresh)
                if self.proc.poll() is not None:
                    break
            # Still alive?
            if self.proc.poll() is None:
                try:
                    os.kill(self.proc.pid, signal.SIGKILL)
                except OSError:
                    pass


class DemoManager:

    def __init__(self, proc, args):
        self.userQuit = False
        self.thread = None
        if args.demo:
            # get duration
            duration = args.demoduration
            if duration is None:
                duration = 120  # 2mn

            # run thread
            self.thread = DemoTimer(proc, duration)
            self.thread.start()

    def __del__(self):
        if self.thread is not None:
            self.thread.join()
            del self.thread

    def userQuitted(self):
        if self.thread is not None:
            return self.thread.userQuitted()
        return False

