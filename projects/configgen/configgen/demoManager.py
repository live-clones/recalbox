#!/usr/bin/env python
import pygame
import threading
import time
import os
import signal

class UserAnyInput:

    def __init__(self):
        pygame.init()
        pygame.joystick.init()
        joysticks = [pygame.joystick.Joystick(x) for x in range(pygame.joystick.get_count())]
        for joystick in joysticks:
            joystick.init()

    def __del__(self):
        pygame.joystick.quit()
        pygame.quit()

    def hasUserInput(self):
        # return true if any key or any button of any pad/mouse has been released
        for event in pygame.event.get():
            if event.type in (pygame.JOYBUTTONUP, pygame.MOUSEBUTTONUP, pygame.MOUSEMOTION, pygame.KEYUP):
                return True
        return False


class DemoTimer(threading.Thread):

    def __init__(self, proc, duration):
        threading.Thread.__init__(self)
        self.inputs = UserAnyInput()
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
            if self.inputs.hasUserInput():
                self.userQuit = True
                print("Demo mode ends upon user request")
                break
            # Process quitted prematurely?
            if self.proc.poll() is not None:
                print("Emulator quitted prematurely")
                break

        self.proc.terminate()
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
                os.kill(self.proc.pid, signal.SIGKILL)


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

