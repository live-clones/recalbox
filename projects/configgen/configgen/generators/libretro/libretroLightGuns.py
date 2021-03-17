#!/usr/bin/env python
## Released version: 1.6.6
##IMPORT STD---------------------------------------------------------------------
import os
import sys
import time
import logging
import subprocess
import linecache

##IMPORT RECALBOX
import recalboxFiles
from settings.keyValueSettings import keyValueSettings

##IMPORT for parsing/xml functions
##for regular expression
import re
##for XML parsing
import xml.etree.ElementTree as ET
from xml.dom import minidom

## remove "//" for robustness (if we move from HOME_INIT to HOME later)
esLightGun = recalboxFiles.esLightGun.replace("//","/")
GameListFileName= recalboxFiles.GameListFileName

# Information game from ES
GAME_INFO_PATH = "/tmp/es_state.inf"
# Key in informatiojn files
KEY_GAME_NAME = "Game"

##Versions history:
##
## V1: initial version
##
## V1.1: to manage several platform in same system: to regroup for example naomi/naomigd/atomiswave
## example : 
##    <system>
##        <platform>naomi</platform>
##        <platform>naomigd</platform>
##    </system>
##
## V1.2: to manage group of games with dedicated inputs/options/emulator/core - new tag <games> is mandatory but we could have several by system
## example : 
##    <games>
##      <emulator name="libretro">
##          <core>flycast</core> 
##      </emulator>
##      <inputs>
##      </inputs>
##      <options>
##      </options>
##    <game>
##            <name>exampleofgamename</name>
##        </game>
##        <game>
##            <name>exampleofgamename2</name>
##        </game>
##    </games>
##
## V1.3: For mame: to manage piority of core selected depending of systems availability
## example: (possible in system, games and game level)
## if mame, we search emulator by emulator already selected with the good romset.
## <system>
##      <platform>mame</platform>
##      <emulator name="libretro">
##          <core>mame</core> 
## </system>
## <system>
##      <platform>mame</platform>
##      <emulator name="libretro">
##          <core>mame2003_plus</core> 
##      </emulator>
## </system>
##
## V1.4: To find dynamically the event index to help to set mouse index in retroarch
## a) Creation of function "findDolphinBarIndexes"
## b) if no dolphin bar found, no override done
## c) if no lightgun.cfg exists, no override done
## d) bug corrected: replace of re.search by IN command for search in nodes of gamelist.xml and using encoding to UTF-8
## 
## V1.5:
## Usage of keyword value from CGF file to be replace dynamically by index (event value) of mouse found.
## a) KEYWORDS list to replace dynamically:
## GUNP1 or gunp1
## GUNP2 or gunp2
## GUNP3 or gunp3
## b) SetInputs function: created to manage replace of GUNP1/2/3 by MouseIndexes
## c) CancelDevicesNotUsed function: created to set to 0 input devices not used finally (to avoid too cursors on screen ;-)!
##
## V1.6.0 : To force mouse index to 0 for player 1 on PC X86_64 using Xorg, only 1 player possible on PC for the moment
## V1.6.1 : To force mouse index to 0 for player 1 on all platforms to avoid a bug of indexation when we have only one player.
## V1.6.2 : To manage system directories better as for "Ports"/"amstradcpc" and add robustnees/performance optimization.
## V1.6.3 : To get name of game from "/tmp/es_state.inf" and to avoid to use gamelist files (too slow) 
## V1.6.4 : To change way to recalculate the indexation of mouse for retroarch (using udev database)
## V1.6.5 : Cleaning + deactivate log by default + to support some keyboards including mouse and identified differently by retroarch (now, we count only header values as js + input + event to anticipate indexation)
## V1.6.6 : To support "!" characters in filename to search (remove of _ in regex also)
## V1.7 TO DO: to integrate a popup to inform about buttons to use from wiimote

def Log(txt): 
    #uncomment/comment the following line to activate/desactivate additional logs on this script
    #print(txt)
    return 0
    
class libretroLightGun:
    
    # constructor
    def __init__(self, system, rom, demo, retroarchConfig, coreConfig):
        # initial settings
        self.retroarchConfig = retroarchConfig
        self.coreConfig = coreConfig
        self.system = system
        self.rom = rom
        self.demo = demo
        ## init for all indexes
        self.MouseIndexByPlayer = ["nul","nul","nul"]

    def findDolphinBarIndexes(self):
        playerindex = 0
        nodeindex = 0
        ## Take order and number of mouse from udev DB
        input1, output1, error1 = os.popen3('udevadm info -e | grep /dev/input/mouse')
        for line1 in output1:
            mousenode = line1.replace("E: DEVNAME=","").rstrip()
            Log('mouse node found: ' + line1.rstrip())
            input2, output2, error2 = os.popen3('cat /proc/bus/input/devices |grep -rni ' + mousenode.replace("/dev/input/","").rstrip())
            input3, output3, error3 = os.popen3('udevadm info ' + mousenode + ' | grep -rni S:')
            input4, output4, error4 = os.popen3('udevadm info ' + mousenode + ' | grep -rni ID_INPUT_MOUSE')
            for line2 in output2:
                Log('   Handlers found:' + line2.rstrip())
                device = output3.readline().rstrip()
                Log("   Device found : " + device)
                id_input_mouse = output4.readline().rstrip()
                Log("   Mouse found : " + id_input_mouse)
                
                #searh dolphin bar 
                if (re.search("Mayflash_Wiimote_PC_Adapter",device)):
                    self.MouseIndexByPlayer[playerindex] = str(nodeindex)
                    Log('           Player : ' + str(playerindex+1))
                    Log('           Mouse Index for Retroarch/UDEV : ' + self.MouseIndexByPlayer[playerindex])
                    playerindex = playerindex + 1
                
                #count number of nodes found for the same devices    
                #we count spaces finally to know number of nodes
                #and only for MOUSE identified (and not TOUCHPAD for example)
                if(re.search("ID_INPUT_MOUSE=1",id_input_mouse)):
                    number_nodes_found = line2.rstrip().count('js') + line2.rstrip().count('event') + line2.rstrip().count('mouse')
                    nodeindex = nodeindex + number_nodes_found
                else:
                    Log("   WARNING: It's not considered as usual mouse by retroarch using udev : ignored in the count of mouse index")
                Log("") #to jump one line in log

    #find game name from /tmp/es_state.inf 
    def getGameNameFromESState(self):
        gameInfo = keyValueSettings(GAME_INFO_PATH, False)
        gameInfo.loadFile(True)
        game_name = gameInfo.getOption(KEY_GAME_NAME, "Unknown")
        del gameInfo
        if game_name != '':
            Log('Game name found, the game name is :' + game_name)
        else:
            Log('System/Game name are not set properly from gamelist/from ES or may be not scrapped ? or empty name ? the game name content is empty ')
        return game_name

    #set inputs depedening of keyword set or simple value
    def SetInputs(self,name,value):
        Log('    ' + name + ' = ' + value)
        if ("gunp" in value.lower()):
            index = int(value.lower().strip().split('gunp')[1])-1
            ##update value with mouse index of gun for this player
            value = self.MouseIndexByPlayer[index]
            if (value == "nul"):
                value = "99" ##to avoid to take nul value as "0" default value
                Log('       value set to "99" to avoid issue if we put "nul" !')
            Log('       updated with Mouse Index: ' + name + ' = ' + value)
        self.retroarchConfig.setOption(name,value)

    #to cancel input device (parameter "input_libretro_device_pX") if the corresponding "input_playerX_mouse_index" is already set to 99 by SetInputs function
    def CancelDevicesNotUsed(self):
        for i in range(4):
            value = "0"
            value = self.retroarchConfig.getOption("input_player" + str(i) + "_mouse_index",value)
            #check if we identified this index as not necessary
            if(value == "99"):
                Log('Mouse index cancelled: ' + "input_player" + str(i) + "_mouse_index" + ' = ' + value)
                Log('Device cancelled and options linked: ' + "input_libretro_device_p" + str(i) + ' = ' + '0')
                self.retroarchConfig.setOption("input_libretro_device_p" + str(i),"0")
                self.retroarchConfig.setOption("input_player" + str(i) + "_gun_start","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_gun_select","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_select","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_start","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_gun_dpad_up","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_gun_dpad_down","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_gun_dpad_right","nul")
                self.retroarchConfig.setOption("input_player" + str(i) + "_gun_dpad_left","nul")

    # Load all configurations from the lightgun.cfg
    def SetLightGunConfig(self, system_name,game_name):
        if(game_name == ''):        
            Log('Game name empty, Not Configured !!!')
            return 'Not configured'
        systems = dict()
        tree = ET.parse(esLightGun)
        root = tree.getroot()
        
        ##rom name cleaning (to be lower case and keep only alphanumeric characters):
        game_name=game_name.lower()
        game_name = re.sub(r'[^a-z0-9!]+', '', game_name)
        
        ##save lightgun dedicated emulator & core
        Log('selected emulator by user:' + self.system.config['emulator'])
        Log('selected core by user:' + self.system.config['core'])
                    
        ##1) first step check if system and game is supporting lightgun or not
        for child in root:
            if child.tag == 'system':
                for platform in child.iter('platform'):
                    if platform.text == system_name:
                        Log('System found: ' + platform.text)
                        emulator = child.find('emulator')
                        Log('Emulator found in system: ' + emulator.attrib["name"])
                        core = emulator.find('core')
                        Log('Core found in system: ' + core.text)
                        if ((platform.text == "mame") and (core.text == self.system.config['core'])) or (platform.text != "mame"):
                            Log('Need to find this game: ' + game_name)
                            for games in child.iter('games'):
                                ##initial value for matching follow-up
                                best_matching_lenght = 0
                                best_matching_game_name = ""
                                for game in games.iter('game'):
                                    game_pattern = game.find('name')
                                    ##as optional we have to avoid error if doesn't exist or empty
                                    tested = ""
                                    if 'tested' in game.attrib:
                                        tested = game.attrib["tested"]
                                    if re.search(game_pattern.text,game_name) and (tested != "nok") :
                                        #a matching found
                                        #keep matching to check if best one exist (to manage better case of a game and its versions ;-)
                                        Log('Pattern that match with game name: ' + game_pattern.text)
                                        if best_matching_lenght < len(game_pattern.text):
                                            best_matching_lenght = len(game_pattern.text)
                                            best_matching_game_name = game_pattern.text
                                            best_matching_game = game
                                            
                                if best_matching_lenght != 0:
                                    Log('Game name best match with pattern: ' + best_matching_game_name)
                                    ## now that we are sure to have a matching, we could get the common part
                                    ## Mandatory under the root
                                    common_inputs = root.find('common_inputs')
                                    Log('Common inputs part found to put in retroarchcustom.cfg:')
                                    for string in common_inputs.iter('string'):
                                        self.SetInputs(string.attrib["name"],string.attrib["value"])
                                    
                                    ## Optional under the root
                                    common_options = root.find('common_options')
                                    Log('Common options part found to put in retroarch-core-options.cfg:')
                                    for string in common_options.iter('string'):
                                        Log('    ' + string.attrib["name"]+ ' = ' + string.attrib["value"])
                                        self.coreConfig.setOption(string.attrib["name"],string.attrib["value"])
                                    
                                    ## we could get also the inputs from system
                                    ## optional by sytem
                                    Log('System inputs to put in retroarchcustom.cfg :')
                                    inputs = child.find('inputs')
                                    if inputs is not None:
                                        for string in inputs.iter('string'):
                                            self.SetInputs(string.attrib["name"],string.attrib["value"])
                                    
                                    ## we could get also the options from system
                                    ## optional by sytem
                                    Log('System options to put in retroarch-core-options.cfg :')
                                    options = child.find('options')
                                    if options is not None:
                                        for string in options.iter('string'):
                                            Log('    ' + string.attrib["name"]+ ' = ' + string.attrib["value"])	
                                            self.coreConfig.setOption(string.attrib["name"],'"' + string.attrib["value"] + '"')
                                                                    
                                    ## now that we are sure to have a matching, we could get the emulator part first
                                    ## optional by games
                                    emulator_games = games.find('emulator')
                                    if emulator_games is not None:
                                        Log('Games Emulator found: ' + emulator_games.attrib["name"])
                                        emulator = emulator_games
                                        core_games = emulator.find('core')
                                        if core_games is not None:
                                            Log('Games Core found: ' + core_games.text)
                                            core = core_games
                                    
                                    ## we could get also the inputs from games
                                    ## optional by games
                                    Log('Games inputs to put in retroarchcustom.cfg :')
                                    inputs = games.find('inputs')
                                    if inputs is not None:
                                        for string in inputs.iter('string'):
                                            self.SetInputs(string.attrib["name"],string.attrib["value"])
                                    
                                    ## we could get also the options from games
                                    ## optional by games
                                    Log('Games options to put in retroarch-core-options.cfg :')
                                    options = games.find('options')
                                    if options is not None:
                                        for string in options.iter('string'):
                                            Log('    ' + string.attrib["name"]+ ' = ' + string.attrib["value"])	
                                            self.coreConfig.setOption(string.attrib["name"],'"' + string.attrib["value"] + '"')
                                    
                                    ## Now for best matching game, we could get the inputs
                                    ## optional by game
                                    inputs = best_matching_game.find('inputs')
                                    Log('Game inputs to put in retroarchcustom.cfg :')
                                    if inputs is not None:
                                        for string in inputs.iter('string'):
                                            self.SetInputs(string.attrib["name"],string.attrib["value"])
                                    
                                    ## and we could get also the options
                                    ## optional by game
                                    options = best_matching_game.find('options')
                                    Log('Game options to put in retroarch-core-options.cfg :')
                                    if options is not None:
                                        for string in options.iter('string'):
                                            Log('    ' + string.attrib["name"]+ ' = ' + string.attrib["value"])
                                            self.coreConfig.setOption(string.attrib["name"],'"' + string.attrib["value"] + '"')
                                    
                                    ## and finally we could also have a specific emulator/core for a game
                                    ## optional by game
                                    emulator_game = best_matching_game.find('emulator')
                                    if emulator_game is not None:
                                        Log('Game Emulator found: ' + emulator_game.attrib["name"])
                                        emulator = emulator_game
                                        core_game = emulator.find('core')
                                        if core_game is not None:
                                            Log('Game Core found: ' + core_game.text)
                                            core = core_game
                                    ##Cancel Devices where mouse index is finally not set (no enough dolphinbar)
                                    self.CancelDevicesNotUsed()
                                    ##save both configurations
                                    self.retroarchConfig.saveFile()
                                    self.coreConfig.saveFile()
                                    ##save lightgun dedicated emulator & core
                                    self.system.config['emulator'] = emulator.attrib["name"]
                                    self.system.config['core'] = core.text
                                    Log('Emulator value :' + self.system.config['emulator'])
                                    Log('Core value :' + self.system.config['core'])
                                    Log('Configured')
                                    return 'configured'
                                else:
                                    Log('Not Yet Configured')
        Log('Not Configured !!!')                    
        return 'Not configured'

    def createLightGunConfiguration(self):
        Log('system: ' + self.system.name)
        Log('rom: ' + self.rom)
        
        ## to check if Mayflash_Wiimote_PC_Adapter exists and get mouse indexes
        self.findDolphinBarIndexes()
        if self.MouseIndexByPlayer[0] == "nul":
            ##no dolphin bar found, no need to overload configuration in this case
            Log('No dolphin bar found !!!')
            Log('Not Configured !!!')                    
            return 'Not configured'
        
        ## to check if lightgun.cfg exists or not
        if os.path.exists(esLightGun) == False:
            ##no xml configuration file found for lightgun
            Log('No lightgun.cfg xml file found !!!')
            Log('Not Configured !!!')                    
            return 'Not configured'

        ## get file name from es_state.inf file
        game_name = self.getGameNameFromESState()

        ## set LightGun Configuration for the game selected
        result = self.SetLightGunConfig(self.system.name,game_name)
        return result