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
from typing import Optional
from xml.etree.ElementTree import Element

from pyudev.device._device import Properties

from configgen.Emulator import Emulator
import configgen.recalboxFiles as recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings


class libretroLightGun:

    # Information game from ES
    __GAME_INFO_PATH = "/tmp/es_state.inf"
    # Key in information files
    __KEY_GAME_NAME = "Game"

    # constructor
    def __init__(self, system: Emulator, rom: str, demo: bool, retroarchConfig: keyValueSettings, coreConfig: keyValueSettings):
        # initial settings
        self.__retroarchConfig: keyValueSettings = retroarchConfig
        self.__coreConfig: keyValueSettings = coreConfig
        self.__System: Emulator = system
        self.__Rom: str = rom
        self.__Demo: bool = demo
        self.__MouseIndexByPlayer = [0, -1, -1, -1]
        self.__debug: bool = True

    def __Log(self, txt: str):
        print("[Configgen.LightGun] " + txt)

    @staticmethod
    def __getOrDefault(props: Properties, key: str, default: str) -> str:
        if key in props: return props[key];
        return default

    def __findDolphinBarIndexes(self) -> bool:
        # Lookup mouse sub-device in dolphin bar devices & count them
        if self.__debug: self.__Log('Seeking for Dolphinbars...')
        import pyudev
        player: int = 0
        mouseIndex: int = 0
        # Use the libudev enumerator as Retroarch does to keep devices in the same order
        for device in pyudev.Context().list_devices(subsystem="input", ID_INPUT_MOUSE="1"):
            if device.sys_name.startswith("event"):
                if self.__getOrDefault(device.properties, "ID_SERIAL", "") == "HJZ_Mayflash_Wiimote_PC_Adapter":
                    self.__MouseIndexByPlayer[player] = mouseIndex
                    player += 1
                if self.__debug: self.__Log("  Found mouse #{} : {}".format(mouseIndex, self.__getOrDefault(device.properties, "ID_SERIAL", "Unknown")))
                mouseIndex += 1

        if self.__debug: self.__Log("  Found {} Dolphinbars among {} Mouses".format(player, mouseIndex))
        return player != 0

    def __findGunconIndexes(self) -> bool:
        # Lookup mouse sub-device in dolphin bar devices & count them
        if self.__debug: self.__Log('Seeking for Guncons...')
        import pyudev
        # todo pass players from lightguns as parameter to add from the list
        player: int = 0
        mouseIndex: int = 0
        for device in pyudev.Context().list_devices(subsystem="input", ID_INPUT_MOUSE="1"):
            if self.__getOrDefault(device.properties, "NAME", "") == '"Namco GunCon 2"':
                print("found")
                self.__MouseIndexByPlayer[player] = mouseIndex
                player += 1

        if self.__debug: self.__Log("  Found {} Guncons among {} Mouses".format(player, mouseIndex))
        return player != 0

    # Find game name from /tmp/es_state.inf
    def __getGameNameFromESState(self) -> str:
        from configgen.settings.keyValueSettings import keyValueSettings
        gameInfo = keyValueSettings(libretroLightGun.__GAME_INFO_PATH, False)
        gameInfo.loadFile(True)
        gameName = gameInfo.getString(libretroLightGun.__KEY_GAME_NAME, "")
        del gameInfo
        if gameName != '':
            if self.__debug: self.__Log('Game name found, the game name is :' + gameName)
            pass
        else:
            import os
            gameName = os.path.splitext(os.path.basename(self.__Rom))[0]
            if self.__debug: self.__Log('Cannot get current playing game. Get rom name as game name.')
        return gameName

    # Cancel input device (parameter "input_libretro_device_pX") if the corresponding "input_playerX_mouse_index" is already set to -1 by SetInputs function
    def __cancelDevicesNotUsed(self):
        for i in range(4):
            player: str = str(i + 1)
            value: int = self.__retroarchConfig.getInt("input_player" + player + "_mouse_index", -1)
            # check if we identified this index as not necessary
            if value < 0:
                if self.__debug: self.__Log('Player #{} mouse index cancelled: input_player{}_mouse_index = {}'.format(player, player, value))
                self.__retroarchConfig.setInt("input_libretro_device_p" + player, 0)
                self.__retroarchConfig.setString("input_player" + player + "_gun_start", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_gun_select", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_select", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_start", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_gun_dpad_up", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_gun_dpad_down", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_gun_dpad_right", "nul")
                self.__retroarchConfig.setString("input_player" + player + "_gun_dpad_left", "nul")

    # Lookup system - Return system node
    def __lookupPlatform(self, root: Element, name: str) -> Optional[Element]:
        finalName = "|{}|".format(name)
        for child in root.findall('system'):
            names: str = "|{}|".format(child.attrib["name"])
            # MAME special case: rely on user core
            if names == "mame":
                for emulatorListNode in child.findall("emulatorList"):
                    for emulatorNode in emulatorListNode.findall("emulator"):
                        if self.__System.Core == emulatorNode.attrib["core"]:
                            return child
            # Normal system
            if finalName in names: return child
        # Not found
        return None

    # Lookup emulator/core - Return emulator node
    def __lookupCore(self, systemNode: Element) -> Optional[Element]:
        # Lookup user choice first
        for emulatorListNode in systemNode.findall("emulatorList"):
            for emulatorNode in emulatorListNode.findall("emulator"):
                if self.__System.Core == emulatorNode.attrib["core"]:
                    return emulatorNode
        # Lookup lowest priority
        priority: int = -1
        chosenNode: Optional[Element] = None
        for emulatorListNode in systemNode.findall("emulatorList"):
            for emulatorNode in emulatorListNode.findall("emulator"):
                currentPriority: int = int(emulatorNode.attrib["priority"])
                if currentPriority < priority or priority < 0:
                    priority = currentPriority
                    chosenNode = emulatorNode
        return chosenNode

    # Lookup game - return game node
    def __lookupGame(self, system: Element, requestedGameName: str) -> (Optional[Element], Optional[Element]):
        bestMatchingLength: int = 0
        gameNode: Optional[Element] = None
        gameListNode: Optional[Element] = None
        for games in system.findall('gameList'):
            ##initial value for matching follow-up
            for game in games.findall('game'):
                currentName: str = game.attrib['name']
                ##as optional we have to avoid error if doesn't exist or empty
                tested: bool = (game.attrib["tested"] == "true") if 'tested' in game.attrib else True
                if requestedGameName.find(currentName) >= 0 and tested:
                    # Matching found - keep seeking for the best matching
                    if bestMatchingLength < len(currentName):
                        bestMatchingLength = len(currentName)
                        gameNode = game
                        gameListNode = games
                        if self.__debug: self.__Log('  Matching game name: ' + currentName)
        if self.__debug: self.__Log('  Final best matching game name: ' + gameNode.attrib['name'] if gameNode is not None else "NONE")
        return gameNode, gameListNode

    def __setCoreOptionsFromNode(self, rootNode: Element, emulatorNode: Element, guntype: str):
        for coreOptionsNode in rootNode.findall("coreOptions"):
            coreFilter: str = coreOptionsNode.attrib["filter"] if "filter" in coreOptionsNode.attrib else "*"

            if coreFilter == "*" or coreFilter == emulatorNode.attrib["core"]:
                for option in coreOptionsNode.findall('option'):
                    guntypeFilter: str = option.attrib["guntype"] if "guntype" in option.attrib else "*"
                    if (guntypeFilter == "*" or guntypeFilter == guntype):
                        self.__coreConfig.setString(option.attrib["name"], '"' + option.attrib["value"] + '"')

    # Set inputs depending of keyword set or simple value
    def __setOption(self, name: str, value: str):
        prevValue = value
        value: str = value.strip().lower()
        if value.startswith("gunp"):
            index: int = int(value[4:]) - 1
            value = str(self.__MouseIndexByPlayer[index])
        if self.__debug: self.__Log("    {} = {} {}".format(name, value, "(was {})".format(prevValue) if value != prevValue else ""))
        self.__retroarchConfig.setString(name, value)

    def __setEmulatorOptionsFromNode(self, rootNode: Element, emulatorNode: Element, guntype: str):
        for emulatorOptionsNode in rootNode.findall("emulatorOptions"):
            emulatorFilter: str = emulatorOptionsNode.attrib["filter"] if "filter" in emulatorOptionsNode.attrib else "*"
            guntypeFilter: str = emulatorOptionsNode.attrib["guntype"] if "guntype" in emulatorOptionsNode.attrib else "*"
            if (guntypeFilter == "*" or guntypeFilter == guntype) and (emulatorFilter == "*" or emulatorFilter == emulatorNode.attrib["name"]):
                for option in emulatorOptionsNode.findall('option'):
                    guntypeFilter: str = option.attrib["guntype"] if "guntype" in option.attrib else "*"
                    if (guntypeFilter == "*" or guntypeFilter == guntype):
                        self.__setOption(option.attrib["name"], option.attrib["value"])

    def __setConfigurationFromNode(self, stage: str, rootNode: Element, emulatorNode: Element, emulatorOptions: bool, coreOptions: bool, updateEmulatorAndCore: bool, guntype: str) -> Element:
        if self.__debug: self.__Log("Configuring stage: {}".format(stage))
        # Start with updating the emulator so that the following option update match the new emulator
        if updateEmulatorAndCore:
            if self.__debug: self.__Log('  Update emulator and core')
            newNode: Optional[Element] = self.__lookupCore(rootNode)
            if newNode is not None:
                emulatorNode = newNode
                if self.__debug:
                    if self.__debug: self.__Log('    Emulator changed to: {} / {}'.format(emulatorNode.attrib["name"], emulatorNode.attrib["core"]))
        # Then set emulator options
        if emulatorOptions:
            if self.__debug: self.__Log('  Update emulator options')
            self.__setEmulatorOptionsFromNode(rootNode, emulatorNode, guntype)
        # And finally core options
        if coreOptions:
            if self.__debug: self.__Log('  Update core options')
            self.__setCoreOptionsFromNode(rootNode, emulatorNode, guntype)

        return emulatorNode

    # Load all configurations from the lightgun.cfg
    def __setLightGunConfig(self, system_name: str, requestedGameName: str, guntype: str) -> bool:
        if requestedGameName == '':
            if self.__debug: self.__Log('Game name empty, Not Configured !!!')
            return False

        import xml.etree.ElementTree as ET
        tree = ET.parse(recalboxFiles.esLightGun)
        root = tree.getroot()
        
        ## Rom name cleaning (alphanumeric lowercase only):
        requestedGameName: str = "".join([c for c in requestedGameName.lower() if c in 'abcdefghijklmnopqrstuvwxyz0123456789'])

        ## Log original lightgun emulator & core
        if self.__debug: self.__Log('Original emulator: {} / {}'.format(self.__System.Emulator, self.__System.Core))

        # Lookup system node
        system: Optional[Element] = self.__lookupPlatform(root, system_name)
        if system is None:
            if self.__debug: self.__Log('System {} not found'.format(system_name))
            return False
        if self.__debug: self.__Log('System matched in database: ' + system_name)

        # Lookup best emulator/core: either the user choice or the best choice regarding priorities
        emulatorNode: Optional[Element] = self.__lookupCore(system)
        if self.__debug: self.__Log('  Emulator changed to: {} / {}'.format(emulatorNode.attrib["name"], emulatorNode.attrib["core"]))

        # Lookup game
        if self.__debug: self.__Log('Requested game: ' + requestedGameName)
        gameNode, gameListNode = self.__lookupGame(system, requestedGameName)

        if gameNode is not None:
            # Set config from root node - Do not update emulator from root node
            emulatorNode = self.__setConfigurationFromNode("Root", root, emulatorNode, True, True, False, guntype)

            # Then override from system node - Do not update emulator from system node as our primary source is the system node
            emulatorNode = self.__setConfigurationFromNode("System", system, emulatorNode, True, True, False, guntype)

            # Then override from gameList node
            emulatorNode = self.__setConfigurationFromNode("GameList", gameListNode, emulatorNode, True, True, True, guntype)

            # Finally override from game node
            emulatorNode = self.__setConfigurationFromNode("Game", gameNode, emulatorNode, True, True, True, guntype)

            # Cancel Devices where mouse index is finally not set
            self.__cancelDevicesNotUsed()

            # Save both configurations
            self.__retroarchConfig.saveFile()
            self.__coreConfig.saveFile()

            # Save lightgun dedicated emulator & core
            self.__System.ChangeEmulatorAndCore(emulatorNode.attrib["name"], emulatorNode.attrib["core"])
            if self.__debug: self.__Log('Configuration successful')
            return True

        if self.__debug: self.__Log('No matching game found in database.')
        return False

    def createLightGunConfiguration(self) -> bool:
        if self.__debug: self.__Log('Playing {} on system {}'.format(self.__Rom, self.__System.Name))

        guntype: str = "none"
        if self.__findDolphinBarIndexes():
            guntype = "dolphinbar"
        else:
            if self.__debug: self.__Log('No dolphin bar found.')

        if self.__findGunconIndexes():
            guntype = "guncon2"
        else:
            if self.__debug: self.__Log('No guncon 2 found.')

        # No gun found, we can skip the configuration
        if guntype == "none":
            return False

        ## to check if lightgun.cfg exists or not
        import os
        if not os.path.exists(recalboxFiles.esLightGun):
            # No xml configuration file found for lightgun
            if self.__debug: self.__Log('Fatal erreor: No lightgun database found.')
            return False

        ## get file name from es_state.inf file
        gameName = self.__getGameNameFromESState()

        ## set LightGun Configuration for the game selected
        result: bool = self.__setLightGunConfig(self.__System.Name, gameName, guntype)
        return result
