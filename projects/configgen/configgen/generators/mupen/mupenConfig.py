#!/usr/bin/env python
import os, sys
import recalboxFiles
import settings
from settings.unixSettings import UnixSettings
import subprocess
import json
from configgen.utils.videoMode import *

mupenSettings = UnixSettings(recalboxFiles.mupenCustom, separator=' ')

GlideN64FBEmulation_whitelist = ["ocarina", "empire", "pokemon", "rayman", "donald", "diddy", "beetle", "tennis", "instinct", "gemini", "twins", "majora", "quake", "ridge"]
GLideN64LegacyBlending_blacklist = ["empire", "beetle", "donkey", "zelda", "bomberman", "party"]
GLideN64NativeResolution_blacklist = ["majora"]

def writeMupenConfig(system, controllers, rom):
	setPaths()
	writeHotKeyConfig(controllers)
	if system.config['videomode'] != 'default':
	        setRealResolution(system.config['videomode'].strip())
	# ~ mupenSettings.save('Fullscreen', "True")
	# ~ mupenSettings.save('ScreenWidth', "")
	# ~ mupenSettings.save('ScreenHeight', "")

	#Draw or not FPS
	if system.config['showFPS'] == 'true':
		mupenSettings.save('ShowFPS', 'True')
                # show_fps is used for Video-Glide64mk2 & Video-GlideN64
                mupenSettings.save('show_fps', '4')
	else:
		mupenSettings.save('ShowFPS', 'False')
                mupenSettings.save('show_fps', '8')

	#Write GlideN64 config
	romName = os.path.basename(rom)

	#Crop resulted image.
	mupenSettings.save('CropMode', '1')
	#Bilinear filtering mode.
	mupenSettings.save('bilinearMode', '1')
	#Size of texture cache in megabytes.
	mupenSettings.save('CacheSize', '100')
	#Enable color buffer copy to RDRAM.
	mupenSettings.save('EnableCopyColorToRDRAM', '0')
	#Enable frame and|or depth buffer emulation.
	mupenSettings.save('EnableFBEmulation', 'False')
	#Do not use shaders to emulate N64 blending modes. Works faster on slow GPU. Can cause glitches.
	mupenSettings.save('EnableLegacyBlending', 'True')
	#Frame buffer size is the factor of N64 native resolution.
	mupenSettings.save('UseNativeResolutionFactor', '1')

	for n in GlideN64FBEmulation_whitelist:
		if n in romName.lower():
			mupenSettings.save('EnableFBEmulation', 'True')

	for n in GLideN64LegacyBlending_blacklist:
		if n in romName.lower():
			mupenSettings.save('EnableLegacyBlending', 'False')

	for n in GLideN64NativeResolution_blacklist:
		if n in romName.lower():
			mupenSettings.save('UseNativeResolutionFactor', '0')

def writeHotKeyConfig(controllers):
	if '1' in controllers:
		if 'hotkey' in controllers['1'].inputs:
			if 'start' in controllers['1'].inputs:
				mupenSettings.save('Joy Mapping Stop', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['start'])))
			if 'y' in controllers['1'].inputs:	
				mupenSettings.save('Joy Mapping Save State', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['y'])))
			if 'x' in controllers['1'].inputs:	
				mupenSettings.save('Joy Mapping Load State', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['x'])))
			if 'pageup' in controllers['1'].inputs:	
				mupenSettings.save('Joy Mapping Screenshot', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['pageup'])))
			if 'up' in controllers['1'].inputs:	
				mupenSettings.save('Joy Mapping Increment Slot', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['up'])))
			if 'right' in controllers['1'].inputs:	
				mupenSettings.save('Joy Mapping Fast Forward', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['right'])))


def createButtonCode(button):
	if(button.type == 'axis'):
		if button.value == '-1':
			return 'A'+button.id+'-'
		else:
			return 'A'+button.id+'+'
	if(button.type == 'button'):
		return 'B'+button.id
	if(button.type == 'hat'):
		return 'H'+button.id+'V'+button.value


def setRealResolution(videoConfig):
	if videoConfig == "auto":
	    videoSetting = autoMode()
	elif "auto" in videoConfig:
	    realSetting = videoConfig.split(' ', 1)[1]
	    videoSetting = autoMode(realSetting)
	else:
	    videoSetting = videoConfig
	print videoSetting
	if videoSetting == "default":
	    wdith, height = getCurrentResulution()
	    mupenSettings.save('ScreenWidth', "{}".format("width"))
	    mupenSettings.save('ScreenHeight', "{}".format("height"))
	    return
	
	group, mode, drive = videoSetting.split(' ')

	proc = subprocess.Popen(["tvservice -j -m {}".format(group)], stdout=subprocess.PIPE, shell=True)
	(out, err) = proc.communicate()
	#print "program output:", out
	tvmodes = json.loads(out)
	
	for tvmode in tvmodes:
		if tvmode["code"] == int(mode):
			mupenSettings.save('ScreenWidth', "{}".format(tvmode["width"]))
			mupenSettings.save('ScreenHeight', "{}".format(tvmode["height"]))
			return

	sys.exit("The resolution for '{} {} {}' is not supported by your monitor".format(group, mode, drive))


def setPaths():
	mupenSettings.save('ScreenshotPath', recalboxFiles.SCREENSHOTS)
	mupenSettings.save('SaveStatePath', recalboxFiles.mupenSaves)
	mupenSettings.save('SaveSRAMPath', recalboxFiles.mupenSaves)
