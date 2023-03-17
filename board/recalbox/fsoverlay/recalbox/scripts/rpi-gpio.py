import os
import time
from subprocess import check_output, CalledProcessError

import RPi.GPIO as GPIO
from configgen import recalboxFiles # this last one retrieves emulators bin names

################################################################################

RETROARCH_COMMANDS = ("QUIT", "RESET", "FAST_FORWARD", "FAST_FORWARD_HOLD", "LOAD_STATE", "SAVE_STATE", "FULLSCREEN_TOGGLE", "STATE_SLOT_PLUS", "STATE_SLOT_MINUS", "REWIND", "MOVIE_RECORD_TOGGLE", "PAUSE_TOGGLE", "FRAMEADVANCE", "SHADER_NEXT", "SHADER_PREV", "CHEAT_INDEX_PLUS", "CHEAT_INDEX_MINUS", "CHEAT_TOGGLE", "SCREENSHOT", "MUTE", "NETPLAY_FLIP", "SLOWMOTION", "VOLUME_UP", "VOLUME_DOWN", "OVERLAY_NEXT", "DISK_EJECT_TOGGLE", "DISK_NEXT", "DISK_PREV", "GRAB_MOUSE_TOGGLE", "MENU_TOGGLE")

# Functions

def getPIDs(process):
    try:
        pidlist = map(int, check_output(["pidof", process]).split())
        return True
    except  CalledProcessError:
        pidlist = []
        return False


# Function to increase volume
def volup():
    fin, fout = os.popen4("echo `amixer get 'PCM'|grep % |awk '{print $4}'|sed -e 's/\[//' -e 's/%\]//'`")
    volinfo = int(fout.read())
    volinfo = volinfo + 2
    if volinfo <= 100:
        os.system("amixer set 'PCM' " + str(volinfo) + "%")


# Function to decrease volume
def voldown():
    fin, fout = os.popen4("echo `amixer get 'PCM'|grep % |awk '{print $4}'|sed -e 's/\[//' -e 's/%\]//'`")
    volinfo = int(fout.read())
    volinfo = volinfo - 2
    if volinfo >= 50:
        os.system("amixer set 'PCM' " + str(volinfo) + "%")


# Function to mute / unmute
mute = False


def volmute():
    global mute
    if not mute:
        os.system("amixer set 'PCM' mute")
        mute = True
    else:
        os.system("amixer set 'PCM' unmute")
        mute = False


# Function to send actions to retroarch
def retroarch_send(action):
    # addressing information of target
    IPADDR = "127.0.0.1"
    PORTNUM = 55355

    # enter the data content of the UDP packet
    PACKETDATA = action.decode("utf-8")

    # initialize a socket, think of it as a cable
    # SOCK_DGRAM specifies that this is UDP
    import socket
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.sendto(PACKETDATA, (IPADDR, PORTNUM))
    except socket.error:
        print 'Failed to create socket'
    # sys.exit()


# Function to get user actions, and then send them to retroarch or kill other emulators
def useraction(action):
    if action == "QUIT" and not getPIDs("retroarch"): # TODO: check gpi script and use emulationstation child process list
        for exe in recalboxFiles.recalboxBins:
            proc = os.path.basename(exe)
            os.system("killall -9 " + proc)
    elif getPIDs("retroarch") and action in RETROARCH_COMMANDS:
        retroarch_send(action)
    elif action == "VOLUP":
        volup()
    elif action == "VOLDOWN":
        voldown()
    elif action == "VOLMUTE":
        volmute()


# Define a threaded callback function to run in another thread when events are detected
def actionGPIO(channel):
    timer = 0
    hold = False
    while not GPIO.input(channel):
        timer += 1
        if timer >= 20 and not hold:
            hold = True
            print gpioconf.get("GPIO" + str(channel), "hold")
            useraction(gpioconf.get("GPIO" + str(channel), "hold"))
        time.sleep(0.1)

    # timer adds 1 each 0.1 seconds if timer = 10, button is pressed for 1s
    if timer > 0:  # avoid false results
        if timer < 10 and not hold:
            print gpioconf.get("GPIO" + str(channel), "quick")
            useraction(gpioconf.get("GPIO" + str(channel), "quick"))
        elif timer >= 10 and not hold:
            print gpioconf.get("GPIO" + str(channel), "standard")
            useraction(gpioconf.get("GPIO" + str(channel), "standard"))
        elif hold:
            print gpioconf.get("GPIO" + str(channel), "release")
            useraction(gpioconf.get("GPIO" + str(channel), "release"))


################################################################################

# Read the user-defined ini file

import ConfigParser
gpioconf = ConfigParser.ConfigParser()
gpioconf.read('/recalbox/share/system/configs/rpi-gpio.ini')

# GPIO Initialization
GPIO.setmode(GPIO.BCM)

for each_section in gpioconf.sections():
    gpio = each_section.replace('GPIO', '')
    gpio = int(gpio)
    GPIO.setup(gpio, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(gpio, GPIO.BOTH, callback=actionGPIO)

print "Waiting for action"

while True:
    time.sleep(0.2)
