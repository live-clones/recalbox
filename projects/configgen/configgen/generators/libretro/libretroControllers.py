#!/usr/bin/env python
from typing import Dict, List

from configgen.Emulator import Emulator
from configgen.controllers.controller import Controller, InputItem, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class LibretroControllers:

    # Map an emulationstation direction to the corresponding retroarch
    retroarchdirs: Dict[int, str] = \
    {
        InputItem.ItemUp   : 'up',
        InputItem.ItemDown : 'down',
        InputItem.ItemLeft : 'left',
        InputItem.ItemRight: 'right'
    }

    # Map an emulationstation joystick to the corresponding retroarch
    retroarchjoysticks: Dict[int, str] = \
    {
        InputItem.ItemJoy1Up  : 'l_y',
        InputItem.ItemJoy1Left: 'l_x',
        InputItem.ItemJoy2Up  : 'r_y',
        InputItem.ItemJoy2Left: 'r_x'
    }

    # Map an emulationstation input type to the corresponding retroarch type
    # 6.1: add _ in front of suffixes to allow empty ones
    typetoname: Dict[int, str] = \
    {
        InputItem.TypeButton: '_btn',
        InputItem.TypeHat   : '_btn',
        InputItem.TypeAxis  : '_axis',
        InputItem.TypeKey   : ''  # 6.1: no prefix for keys
    }

    # Map an emulationstation input hat to the corresponding retroarch hat value
    hatstoname: Dict[int, str] = \
    {
        1: 'up',
        2: 'right',
        4: 'down',
        8: 'left'
    }

    # List of controllers that works only with sdl2 driver
    sdl2driverControllers: List[str] = \
    [
        'Bluetooth Wireless Controller',
        'szmy-power',
        'XiaoMi Bluetooth Wireless GameController',
        'ipega Bluetooth Gamepad',
        'ipega Bluetooth Gamepad   ',
        'Moga Pro 2 HID'
    ]

    # Map buttons to the corresponding retroarch specials keys
    retroarchspecialsnomenu: Dict[int, str] = \
    {
        InputItem.ItemX        : 'load_state',
        InputItem.ItemY        : 'save_state',
        InputItem.ItemL1       : 'screenshot',
        InputItem.ItemStart    : 'exit_emulator',
        InputItem.ItemUp       : 'state_slot_increase',
        InputItem.ItemDown     : 'state_slot_decrease',
        InputItem.ItemLeft     : 'rewind',
        InputItem.ItemRight    : 'hold_fast_forward',
        InputItem.ItemL2       : 'shader_prev',
        InputItem.ItemR2       : 'shader_next',
        InputItem.ItemA        : 'reset',
        # Added in 6.1
        InputItem.ItemR3       : 'recording_toggle',
        InputItem.ItemJoy1Left : 'disk_prev',
        InputItem.ItemJoy1Right: 'disk_next',
        InputItem.ItemJoy1Up   : 'disk_eject_toggle',
        InputItem.ItemJoy1Down : 'ai_service',
        InputItem.ItemJoy2Left : 'cheat_index_minus',
        InputItem.ItemJoy2Right: 'cheat_index_plus',
        InputItem.ItemJoy2Up   : 'cheat_toggle',
        InputItem.ItemJoy2Down : 'fps_toggle',
    }

    # Map an emulationstation button name to the corresponding retroarch name
    retroarchbtns: Dict[int, str] = \
    {
        InputItem.ItemA     : 'a',
        InputItem.ItemB     : 'b',
        InputItem.ItemX     : 'x',
        InputItem.ItemY     : 'y',
        InputItem.ItemL1    : 'l',
        InputItem.ItemR1    : 'r',
        InputItem.ItemL2    : 'l2',
        InputItem.ItemR2    : 'r2',
        InputItem.ItemL3    : 'l3',
        InputItem.ItemR3    : 'r3',
        InputItem.ItemStart : 'start',
        InputItem.ItemSelect: 'select'
    }

    def __init__(self, system: Emulator, recalboxOptions: keyValueSettings, settings: keyValueSettings, controllers: ControllerPerPlayer, nodefaultkeymap: bool):
        self.system: Emulator = system
        self.recalboxOptions: keyValueSettings = recalboxOptions
        self.settings: keyValueSettings = settings
        self.controllers: ControllerPerPlayer = controllers
        self.nodefaultkeymap: bool = nodefaultkeymap
        self.retroarchspecials: Dict[int, str] = dict(LibretroControllers.retroarchspecialsnomenu)
        self.retroarchspecials[InputItem.ItemB] = 'menu_toggle'

    # Fill controllers configuration
    def fillControllersConfiguration(self) -> keyValueSettings:
        inputDriver: str = self.recalboxOptions.getString("global.inputdriver", self.recalboxOptions.getString(self.system.Name + ".inputdriver", 'auto'))

        # Cleanup all
        self.cleanUpControllers()

        # Build configurations
        for player in self.controllers:
            self.buildController(self.controllers[player], player)

        # Set Hotkey
        self.setHotKey()

        # Set driver
        if inputDriver == "auto": inputDriver = self.getInputDriver()
        self.settings.setString("input_joypad_driver", inputDriver)

        return self.settings

    # Find the best driver for controllers
    def getInputDriver(self) -> str:
        for controller in self.controllers:
            for controllerName in self.sdl2driverControllers:
                if controllerName in self.controllers[controller].DeviceName:
                    return "sdl2"
        return "udev"

    # Remove all controller configurations
    def cleanUpControllers(self):
        self.settings.removeOptionStartingWith("input_player")
        for specialkey in self.retroarchspecials.values():
            self.settings.removeOptionStartingWith("input_" + specialkey)

    # Write the hotkey for player 1
    def setHotKey(self):
        if 1 in self.controllers:
            if self.controllers[1].HasHotkey:
                self.settings.setInt("input_enable_hotkey_btn", self.controllers[1].Hotkey.Id)

    # Return the retroarch analog_dpad_mode
    @staticmethod
    def getAnalogMode(controller: Controller) -> int:
        # if system.Name != 'psx':
        for dirkey in LibretroControllers.retroarchdirs:
            if controller.HasInput(dirkey):
                if controller.Input(dirkey).Type in (InputItem.TypeButton, InputItem.TypeHat):
                    return 1
        return 0

    # Return the playstation analog mode for a controller
    @staticmethod
    def getAnalogCoreMode(controller: Controller) -> str:
        for dirkey in LibretroControllers.retroarchdirs:
            if controller.HasInput(dirkey):
                if controller.Input(dirkey).Type in (InputItem.TypeButton, InputItem.TypeHat):
                    return 'analog'
        return 'standard'

    # Returns the value to write in retroarch config file, depending on the type
    @staticmethod
    def getConfigValue(inp: InputItem) -> str:
        if inp.IsButton: return str(inp.Id)
        if inp.IsAxis:   return ('-' if inp.Value < 0 else '+') + str(inp.Id)
        if inp.IsHat:    return 'h' + str(inp.Id) + LibretroControllers.hatstoname[inp.Value]
        if inp.IsKey:    return str(inp.Id)

        raise TypeError

    # Write a configuration for a specified controller
    def buildController(self, controller: Controller, playerIndex: int):
        settings = self.settings

        # Get specials string or default
        specials = self.system.SpecialKeys

        # config['input_device'] = '"%s"' % controller.RealName
        for btnkey in self.retroarchbtns:
            btnvalue = self.retroarchbtns[btnkey]
            if controller.HasInput(btnkey):
                inp: InputItem = controller.Input(btnkey)
                settings.setString("input_player%s_%s%s" % (controller.PlayerIndex, btnvalue, self.typetoname[inp.Type]),
                                   self.getConfigValue(inp))
        for dirkey in self.retroarchdirs:
            dirvalue = self.retroarchdirs[dirkey]
            if controller.HasInput(dirkey):
                inp = controller.Input(dirkey)
                settings.setString("input_player%s_%s%s" % (controller.PlayerIndex, dirvalue, self.typetoname[inp.Type]),
                                   self.getConfigValue(inp))
        for jskey in self.retroarchjoysticks:
            jsvalue = self.retroarchjoysticks[jskey]
            if controller.HasInput(jskey):
                inp = controller.Input(jskey)
                settings.setString("input_player%s_%s_minus_axis" % (controller.PlayerIndex, jsvalue), '-' + str(inp.Id))
                settings.setString("input_player%s_%s_plus_axis" % (controller.PlayerIndex, jsvalue),  '+' + str(inp.Id))

        if controller.PlayerIndex == 1:
            specialMap: Dict[int, str] = {}
            if specials == "nomenu":  specialMap = self.retroarchspecialsnomenu
            if specials == "default": specialMap = self.retroarchspecials
            for item in specialMap:
                if controller.HasInput(item):
                    value: str = specialMap[item]
                    inp = controller.Input(item)
                    settings.setString("input_%s%s" % (value, self.typetoname[inp.Type]), self.getConfigValue(inp))
            if controller.HasStart:
                specialvalue = self.retroarchspecials[InputItem.ItemStart]
                inp = controller.Start
                settings.setString("input_%s%s" % (specialvalue, self.typetoname[inp.Type]), self.getConfigValue(inp))

        # No default keymap?
        if self.nodefaultkeymap:
            for btnkey in self.retroarchbtns:
                settings.setString("input_player%s_%s" % (controller.PlayerIndex, self.retroarchbtns[btnkey]), '"nul"')
            for dirkey in self.retroarchdirs:
                settings.setString("input_player%s_%s" % (controller.PlayerIndex, self.retroarchdirs[dirkey]), '"nul"')
            settings.setString("input_enable_hotkey", '"nul"')
        else:
            for btnkey in self.retroarchbtns:
                settings.removeOption("input_player%s_%s" % (controller.PlayerIndex, self.retroarchbtns[btnkey]))
            for dirkey in self.retroarchdirs:
                settings.removeOption("input_player%s_%s" % (controller.PlayerIndex, self.retroarchdirs[dirkey]))
            settings.setString("input_enable_hotkey", '"escape"')

        # Assign pad to player
        settings.setInt("input_player{}_joypad_index".format(playerIndex), controller.SdlIndex)
        settings.setInt("input_player{}_analog_dpad_mode".format(playerIndex), self.getAnalogMode(controller))



