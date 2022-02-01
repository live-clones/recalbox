import RPi.GPIO as GPIO
import os
import time
import subprocess
from multiprocessing import Process

# initialize pins
powerPin = 3  # pin 5
ledPin = 14  # TXD
resetPin = 2  # pin 3
powerenPin = 4  # pin 5

# get script directory
scriptDir = os.path.dirname(os.path.realpath(__file__))


def init():
    '''initialize GPIO settings'''
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(powerPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(resetPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(ledPin, GPIO.OUT)
    GPIO.setup(powerenPin, GPIO.OUT)
    GPIO.output(powerenPin, GPIO.HIGH)
    GPIO.setwarnings(False)


def poweroff():
    '''waits for user to hold button up to 1 second before issuing poweroff command'''
    while True:
        GPIO.wait_for_edge(powerPin, GPIO.FALLING)
        os.chdir(scriptDir)
        output = int(
            subprocess.check_output(
                [f"{scriptDir}/recalbox_SafeShutdown.sh", "--espid"]
            )
        )
        if output:
            os.system(f"{scriptDir}/recalbox_SafeShutdown.sh --shutdown")
        else:
            os.system("shutdown -h now")


def ledBlink():
    '''blinks the LED to signal button being pushed'''
    while True:
        GPIO.output(ledPin, GPIO.HIGH)
        GPIO.wait_for_edge(powerPin, GPIO.FALLING)
        while GPIO.input(powerPin) == GPIO.LOW:
            GPIO.output(ledPin, GPIO.LOW)
            time.sleep(0.2)
            GPIO.output(ledPin, GPIO.HIGH)
            time.sleep(0.2)


def reset():
    '''resets the pi'''
    while True:
        GPIO.wait_for_edge(resetPin, GPIO.FALLING)
        os.chdir(scriptDir)
        output = int(
            subprocess.check_output(
                [f"{scriptDir}/recalbox_SafeShutdown.sh", "--espid"]
            )
        )
        output_rc = int(
            subprocess.check_output(
                [f"{scriptDir}/recalbox_SafeShutdown.sh", "--emupid"]
            )
        )
        if output_rc:
            os.system(f"{scriptDir}/recalbox_SafeShutdown.sh --emukill")
        elif output:
            os.system(f"{scriptDir}/recalbox_SafeShutdown.sh --restart")
        else:
            os.system("shutdown -r now")


if __name__ == "__main__":
    # initialize GPIO settings
    init()
    # create a multiprocessing.Process instance for each function to enable parallelism
    powerProcess = Process(target=poweroff)
    powerProcess.start()
    ledProcess = Process(target=ledBlink)
    ledProcess.start()
    resetProcess = Process(target=reset)
    resetProcess.start()

    powerProcess.join()
    ledProcess.join()
    resetProcess.join()

GPIO.cleanup()
