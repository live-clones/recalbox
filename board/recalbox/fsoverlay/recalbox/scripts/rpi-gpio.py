import RPi.GPIO as GPIO
import ConfigParser
import string
import time
from subprocess import check_output, CalledProcessError
import os
import thread
import datetime
import socket
import sys
import argparse
from datetime import datetime
from configgen import recalboxFiles
# this last one retrieves emulators bin names

################################################################################

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
	if (volinfo<=100):
		os.system("amixer set 'PCM' "+str(volinfo)+"%")

# Function to decrease volume
def voldown():
	fin, fout = os.popen4("echo `amixer get 'PCM'|grep % |awk '{print $4}'|sed -e 's/\[//' -e 's/%\]//'`")
	volinfo = int(fout.read())
	volinfo = volinfo - 2
	if (volinfo>=50):
		os.system("amixer set 'PCM' "+str(volinfo)+"%")

# Function to mute / unmute
mute = False
def volmute():
	global mute
	if (mute == False):
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
	PACKETDATA = action

	# initialize a socket, think of it as a cable
	# SOCK_DGRAM specifies that this is UDP
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	except socket.error:
		print 'Failed to create socket'
		#sys.exit()
	#s.connect((IPADDR, PORTNUM))
 
	# connect the socket, think of it as connecting the cable to the address location

	#data = bytes(PACKETDATA)

	s.sendto(PACKETDATA, (IPADDR, PORTNUM))

# Function to get user actions, and then send them to retroarch or kill other emulators
def useraction(action):
	if (action == "QUIT" and getPIDs("retroarch") == False):
		for bin in recalboxFiles.recalboxBins:
			proc = os.path.basename(bin)
			os.system("killall -9 "+proc)
	elif (getPIDs("retroarch") == True and (action == "QUIT" or action == "RESET" or action == "FAST_FORWARD" or action == "FAST_FORWARD_HOLD" or action == "LOAD_STATE" or action == "SAVE_STATE" or action == "FULLSCREEN_TOGGLE" or action == "STATE_SLOT_PLUS" or action == "STATE_SLOT_MINUS" or action == "REWIND" or action == "MOVIE_RECORD_TOGGLE" or action == "PAUSE_TOGGLE" or action == "FRAMEADVANCE" or action == "SHADER_NEXT" or action == "SHADER_PREV" or action == "CHEAT_INDEX_PLUS" or action == "CHEAT_INDEX_MINUS" or action == "CHEAT_TOGGLE" or action == "SCREENSHOT" or action == "MUTE" or action == "NETPLAY_FLIP" or action == "SLOWMOTION" or action == "VOLUME_UP" or action == "VOLUME_DOWN" or action == "OVERLAY_NEXT" or action == "DISK_EJECT_TOGGLE" or action == "DISK_NEXT" or action == "DISK_PREV" or action == "GRAB_MOUSE_TOGGLE" or action == "MENU_TOGGLE")):
		retroarch_send(action)
	elif (action == "VOLUP"):
		volup()
	elif (action == "VOLDOWN"):
		voldown()
	elif (action == "VOLMUTE"):
		volmute()

# Define a threaded callback function to run in another thread when events are detected
def actionGPIO(channel):
	timer = 0
	flag = True
	hold = False
	while flag:
		if GPIO.input(channel) == False:
			timer += 1
			if (timer >=20 and hold == False):
				hold = True
				print gpioconf.get("GPIO"+str(channel),"hold")
				useraction(gpioconf.get("GPIO"+str(channel),"hold"))
		elif GPIO.input(channel) == True:
			#timer adds 1 each 0.1 seconds if timer = 10, button is pressed for 1s
			if (timer > 0): #avoid false results
				if (timer <10 and hold == False):
					print gpioconf.get("GPIO"+str(channel),"quick")
					useraction(gpioconf.get("GPIO"+str(channel),"quick"))
				elif (timer >=10 and hold == False):
					print gpioconf.get("GPIO"+str(channel),"standard")
					useraction(gpioconf.get("GPIO"+str(channel),"standard"))
				elif (hold == True):
					print gpioconf.get("GPIO"+str(channel),"release")
					useraction(gpioconf.get("GPIO"+str(channel),"release"))
			timer = 0
			flag = False
			hold = False
		time.sleep(0.1)

################################################################################

# Read the user-defined ini file

gpioconf=ConfigParser.ConfigParser()
gpioconf.read('/recalbox/share/system/configs/rpi-gpio.ini')

# GPIO Initialization
GPIO.setmode(GPIO.BCM)

for each_section in gpioconf.sections():
	gpio = each_section.replace('GPIO','')
	gpio=int(gpio)
	GPIO.setup(gpio, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.add_event_detect(gpio, GPIO.BOTH, callback=actionGPIO)

print "Waiting for action"

while True:
        time.sleep(0.2)

