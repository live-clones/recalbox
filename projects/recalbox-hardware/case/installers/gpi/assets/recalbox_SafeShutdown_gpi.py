import RPi.GPIO as GPIO
from multiprocessing import Process

# Initialize pins
powerPin = 26
powerenPin = 27 

# Initialize GPIO settings
def init():
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(powerPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.setup(powerenPin, GPIO.OUT)
	GPIO.output(powerenPin, GPIO.HIGH)
	GPIO.setwarnings(False)


# Lookup emulationstation
def lookupEmulationStation():
	import psutil
	# Run through all process
	for p in psutil.process_iter():
		if "/usr/bin/emulationstation" in p.cmdline():
			print("Emulation-Station found")
			return p
	print("Emulation-Station NOT found")
	return None


# Kill process recursively
def killEmulationStationTree():
	import psutil
	try:
		# Get emulationstation process
		parent = lookupEmulationStation()
		if parent is None: return
		allProcess = { parent }

		# Get all childrens recursively
		children = parent.children(recursive=True)
		print("Got children: {}".format(children))

		# Tell EmulationStation to quit demo/mode and/or to exit gracefully
		try:
			import os
			try:
				os.makedirs("/tmp/externalnotifications")
			except: pass
			with open("/tmp/externalnotifications/emulationstation.quitnow", "w") as sf:
				sf.write("exit")
				sf.flush()
				sf.close()
		except IOError:
			pass  # Can't wait or retry

		# Quit emulationstation gracefully
		parent.terminate()
		print("Emulationstation terminated")

		# Kill all children
		for child in children:
			allProcess.add(child)
			child.kill()
		print("Children killed")

		# Wait for everyone
		_, stillAlive = psutil.wait_procs(allProcess, 20)
		print("Stillalive: {}".format(stillAlive))

		# if emulation is still alive, give it a last chance to quit
		if parent in stillAlive:
			psutil.wait_procs(stillAlive, 20)
			print("Last chance end")

	except psutil.NoSuchProcess:
		print("exception in killEmulationStationTree")
		pass


# Waits for user to hold button up to 1 second before issuing poweroff command
def poweroff():
	while True:
		GPIO.wait_for_edge(powerPin, GPIO.FALLING)

		# Stop all
		killEmulationStationTree()

		# Fast stop
		import os
		os.system("reboot -f")


if __name__ == "__main__":

	#initialize GPIO settings
	init()
	#create a multiprocessing.Process instance for each function to enable parallelism 
	powerProcess = Process(target = poweroff)
	powerProcess.start()
	powerProcess.join()
	GPIO.cleanup()
