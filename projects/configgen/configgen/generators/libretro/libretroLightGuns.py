#!/usr/bin/env python3
## Released version: 1.7

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
## v1.7   : Optimize
## V1.8 TO DO: to integrate a popup to inform about buttons to use from wiimote
from typing import Optional, Dict

from pyudev.device._device import Properties, Device

from configgen.Emulator import Emulator
import configgen.recalboxFiles as recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings


def Log(txt: str):
    print("[Configgen.LightGun] " + txt)

class libretroLightGun:
    from xml.etree.ElementTree import Element

    # Information game from ES
    GAME_INFO_PATH = "/tmp/es_state.inf"
    # Key in informatiojn files
    KEY_GAME_NAME = "Game"

    # constructor
    def __init__(self, system: Emulator, rom: str, demo: bool, retroarchConfig: keyValueSettings, coreConfig: keyValueSettings):
        # initial settings
        self.retroarchConfig: keyValueSettings = retroarchConfig
        self.coreConfig: keyValueSettings = coreConfig
        self.system: Emulator = system
        self.rom: str = rom
        self.demo: bool = demo
        self.MouseIndexByPlayer = [0, -1, -1, -1]
        self.__debug: bool = True

    def getOrDefault(self, props: Properties, key: str, default: str) -> str:
        if key in props: return props[key];
        return default

    def findDolphinBarIndexes(self) -> bool:
        # Lookup mouse sub-device in dolphin bar devices & count them
        import pyudev
        player: int = 0
        mouseIndex: int = 0
        devices: Dict[str, Device] = {}
        for device in pyudev.Context().list_devices(subsystem="input", ID_INPUT_MOUSE="1"):
            if device.sys_name.startswith("event"):
                devices[device.sys_name] = device
        for name, device in sorted(devices.items()):
            if self.getOrDefault(device.properties, "ID_SERIAL", "") == "HJZ_Mayflash_Wiimote_PC_Adapter":
                self.MouseIndexByPlayer[player] = mouseIndex
                player += 1
            mouseIndex += 1
            if self.__debug: Log(name + " = " + str(device))

        return len(self.MouseIndexByPlayer) != 0

    # Find game name from /tmp/es_state.inf
    def getGameNameFromESState(self) -> str:
        from configgen.settings.keyValueSettings import keyValueSettings
        gameInfo = keyValueSettings(libretroLightGun.GAME_INFO_PATH, False)
        gameInfo.loadFile(True)
        gameName = gameInfo.getString(libretroLightGun.KEY_GAME_NAME, "")
        del gameInfo
        if gameName != '':
            if self.__debug: Log('Game name found, the game name is :' + gameName)
            pass
        else:
            import os
            gameName = os.path.splitext(os.path.basename(self.rom))[0]
            if self.__debug: Log('System/Game name are not set properly from gamelist/from ES or may be not scrapped ? or empty name ? the game name content is empty ')
        return gameName

    #set inputs depending of keyword set or simple value
    def SetInputs(self, name: str, value: str):
        if self.__debug: Log('    ' + name + ' = ' + value)
        value = value.strip().lower()
        if "gunp" in value:
            index = int(value[4:]) - 1
            ##update value with mouse index of gun for this player
            ivalue: int = self.MouseIndexByPlayer[index]
            value = str(ivalue)
            if self.__debug: Log('       updated with Mouse Index: ' + name + ' = ' + value)
        self.retroarchConfig.setString(name, value)

    # to cancel input device (parameter "input_libretro_device_pX") if the corresponding "input_playerX_mouse_index" is already set to 99 by SetInputs function
    def CancelDevicesNotUsed(self):
        for i in range(4):
            player: str = str(i + 1)
            value: int = self.retroarchConfig.getInt("input_player" + player + "_mouse_index", -1)
            # check if we identified this index as not necessary
            if value < 0:
                if self.__debug: Log('Mouse index cancelled: ' + "input_player" + player + "_mouse_index" + ' = ' + str(value))
                if self.__debug: Log('Device cancelled and options linked: ' + "input_libretro_device_p" + player + ' = ' + '0')
                self.retroarchConfig.setInt("input_libretro_device_p" + player, -1)
                self.retroarchConfig.setString("input_player" + player + "_gun_start", "nul")
                self.retroarchConfig.setString("input_player" + player + "_gun_select", "nul")
                self.retroarchConfig.setString("input_player" + player + "_select", "nul")
                self.retroarchConfig.setString("input_player" + player + "_start", "nul")
                self.retroarchConfig.setString("input_player" + player + "_gun_dpad_up", "nul")
                self.retroarchConfig.setString("input_player" + player + "_gun_dpad_down", "nul")
                self.retroarchConfig.setString("input_player" + player + "_gun_dpad_right", "nul")
                self.retroarchConfig.setString("input_player" + player + "_gun_dpad_left", "nul")

    @staticmethod
    def LookupPlatform(root: Element, name: str) -> [Optional[Element], Optional[Element]]:
        for child in root.iter('system'):
            for platform in child.iter('platform'):
                if platform.text == name:
                    return platform, child
        return None, None

    def SetOptionsFromNode(self, optionNode: Element):
        if optionNode is not None:
            for string in optionNode.iter('string'):
                if self.__debug: Log('    ' + string.attrib["name"] + ' = ' + string.attrib["value"])
                self.coreConfig.setString(string.attrib["name"], '"' + string.attrib["value"] + '"')

    def SetInputsFromNode(self, inputNode: Element):
        if inputNode is not None:
            for string in inputNode.iter('string'):
                self.SetInputs(string.attrib["name"], string.attrib["value"])

    def GetCoreFromNode(self, emulatorNode: Element) -> [Optional[Element], Optional[Element]]:
        coreNode = None
        if emulatorNode is not None:
            if self.__debug: Log('Games Emulator found: ' + emulatorNode.attrib["name"])
            coreNode = emulatorNode.find('core')
            if coreNode is not None:
                if self.__debug: Log('Games Core found: ' + coreNode.text)
        return emulatorNode, coreNode

    # Load all configurations from the lightgun.cfg
    def SetLightGunConfig(self, system_name: str, game_name: str) -> bool:
        if game_name == '':
            if self.__debug: Log('Game name empty, Not Configured !!!')
            return False

        import xml.etree.ElementTree as ET
        tree = ET.parse(recalboxFiles.esLightGun)
        root = tree.getroot()
        
        ## Rom name cleaning (to be lower case and keep only alphanumeric characters):
        game_name = "".join([c for c in game_name.lower() if c in 'abcdefghijklmnopqrstuvwxyz0123456789'])

        ## Save lightgun dedicated emulator & core
        if self.__debug: Log('selected emulator by user:' + self.system.Emulator)
        if self.__debug: Log('selected core by user:' + self.system.Core)
                    
        ## 1) first step check if system and game is supporting lightgun or not
        platform, system = self.LookupPlatform(root, system_name)
        if platform is None:
            if self.__debug: Log('Platform {} not found'.format(system_name))
            return False
        if self.__debug: Log('System found: ' + platform.text)

        if self.__debug: Log('Lookup default emulator/core')
        emulator, core = self.GetCoreFromNode(system.find('emulator'))
        if self.__debug:
            Log('Emulator found in system: ' + emulator.attrib["name"])
            Log('Core found in system: ' + core.text)

        if ((platform.text == "mame") and (core.text == self.system.Core)) or (platform.text != "mame"):
            if self.__debug: Log('Need to find this game: ' + game_name)
            for games in system.iter('games'):
                ##initial value for matching follow-up
                best_matching_lenght = 0
                best_matching_game_name = ""
                best_matching_game = None
                for game in games.iter('game'):
                    game_pattern = game.find('name')
                    ##as optional we have to avoid error if doesn't exist or empty
                    tested = game.attrib["tested"] if 'tested' in game.attrib else ""
                    if game_name.find(game_pattern.text) >= 0 and (tested != "nok"):
                        #a matching found
                        #keep matching to check if best one exist (to manage better case of a game and its versions ;-)
                        if self.__debug: Log('Pattern that match with game name: ' + game_pattern.text)
                        if best_matching_lenght < len(game_pattern.text):
                            best_matching_lenght = len(game_pattern.text)
                            best_matching_game_name = game_pattern.text
                            best_matching_game = game

                if best_matching_game is not None:
                    if self.__debug: Log('Game name best match with pattern: ' + best_matching_game_name)
                    ## now that we are sure to have a matching, we could get the common part
                    ## Mandatory under the root
                    if self.__debug: Log('Common inputs part found to put in retroarchcustom.cfg:')
                    self.SetInputsFromNode(root.find('common_inputs'))

                    ## Optional under the root
                    if self.__debug: Log('Common options part found to put in retroarch-core-options.cfg:')
                    self.SetOptionsFromNode(root.find('common_options'))

                    ## we could get also the inputs from system
                    ## optional by sytem
                    if self.__debug: Log('System inputs to put in retroarchcustom.cfg :')
                    self.SetInputsFromNode(system.find('inputs'))

                    ## we could get also the options from system
                    ## optional by sytem
                    if self.__debug: Log('System options to put in retroarch-core-options.cfg :')
                    self.SetOptionsFromNode(system.find('options'))

                    ## now that we are sure to have a matching, we could get the emulator part first
                    ## optional by games
                    emulatorNode, coreNode = self.GetCoreFromNode(games.find('emulator'))
                    if emulatorNode is not None: emulator = emulatorNode
                    if coreNode is not None: core = coreNode

                    ## we could get also the inputs from games
                    ## optional by games
                    if self.__debug: Log('Games inputs to put in retroarchcustom.cfg :')
                    self.SetInputsFromNode(games.find('inputs'))

                    ## we could get also the options from games
                    ## optional by games
                    if self.__debug: Log('Games options to put in retroarch-core-options.cfg :')
                    self.SetOptionsFromNode(games.find('options'))

                    ## Now for best matching game, we could get the inputs
                    ## optional by game
                    if self.__debug: Log('Game inputs to put in retroarchcustom.cfg :')
                    self.SetInputsFromNode(best_matching_game.find('inputs'))

                    ## and we could get also the options
                    ## optional by game
                    if self.__debug: Log('Game options to put in retroarch-core-options.cfg :')
                    self.SetOptionsFromNode(best_matching_game.find('options'))

                    ## and finally we could also have a specific emulator/core for a game
                    ## optional by game
                    emulatorNode, coreNode = self.GetCoreFromNode(best_matching_game.find('emulator'))
                    if emulatorNode is not None: emulator = emulatorNode
                    if coreNode is not None: core = coreNode

                    ##Cancel Devices where mouse index is finally not set (no enough dolphinbar)
                    self.CancelDevicesNotUsed()

                    ##save both configurations
                    self.retroarchConfig.saveFile()
                    self.coreConfig.saveFile()

                    ##save lightgun dedicated emulator & core
                    self.system.ChangeEmulatorAndCore(emulator.attrib["name"], core.text)
                    if self.__debug:
                        Log('Emulator value :' + self.system.Emulator)
                        Log('Core value :' + self.system.Core)
                        Log('Configured')
                    return True

                if self.__debug: Log('Not Yet Configured')

        if self.__debug: Log('Not Configured !!!')
        return False

    def createLightGunConfiguration(self) -> bool:
        if self.__debug: Log('system: ' + self.system.Name)
        if self.__debug: Log('rom: ' + self.rom)
        
        ## to check if Mayflash_Wiimote_PC_Adapter exists and get mouse indexes
        if not self.findDolphinBarIndexes():
            ##no dolphin bar found, no need to overload configuration in this case
            if self.__debug: Log('No dolphin bar found !!!')
            if self.__debug: Log('Not Configured !!!')
            return False
        
        ## to check if lightgun.cfg exists or not
        import os
        if not os.path.exists(recalboxFiles.esLightGun):
            ##no xml configuration file found for lightgun
            if self.__debug: Log('No lightgun.cfg xml file found !!!')
            if self.__debug: Log('Not Configured !!!')
            return False

        ## get file name from es_state.inf file
        game_name = self.getGameNameFromESState()

        ## set LightGun Configuration for the game selected
        result: bool = self.SetLightGunConfig(self.system.Name, game_name)
        return result
