#!/usr/bin/env python
import sys
import os
#import recalboxFiles
#import utils.slugify as slugify

#from settings.unixSettings import UnixSettings


sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

#libretroSettings = UnixSettings(recalboxFiles.retroarchCustom, separator=' ')
#coreSettings = UnixSettings(recalboxFiles.retroarchCoreCustom, separator=' ')
#recalboxConfSettings = UnixSettings(recalboxFiles.recalboxConf)

#settingsRoot = recalboxFiles.retroarchRoot


class LibretroControllers:

    # Map an emulationstation direction to the corresponding retroarch
    retroarchdirs = \
    {
        'up': 'up',
        'down': 'down',
        'left': 'left',
        'right': 'right'
    }

    # Map an emulationstation joystick to the corresponding retroarch
    retroarchjoysticks = \
    {
        'joystick1up': 'l_y',
        'joystick1left': 'l_x',
        'joystick2up': 'r_y',
        'joystick2left': 'r_x'
    }

    # Map an emulationstation input type to the corresponding retroarch type
    typetoname = \
    {
        'button': 'btn',
        'hat': 'btn',
        'axis': 'axis',
        'key': 'key'
    }

    # Map an emulationstation input hat to the corresponding retroarch hat value
    hatstoname = \
    {
        '1': 'up',
        '2': 'right',
        '4': 'down',
        '8': 'left'
    }

    # List of controllers that works only with sdl2 driver
    sdl2driverControllers = \
    [
        'Bluetooth Wireless Controller',
        'szmy-power',
        'XiaoMi Bluetooth Wireless GameController',
        'ipega Bluetooth Gamepad',
        'ipega Bluetooth Gamepad   ',
        'Moga Pro 2 HID'
    ]

    # Map buttons to the corresponding retroarch specials keys
    retroarchspecialsnomenu = \
    {
        'x': 'load_state',
        'y': 'save_state',
        'pageup': 'screenshot',
        'start': 'exit_emulator',
        'up': 'state_slot_increase',
        'down': 'state_slot_decrease',
        'left': 'rewind',
        'right': 'hold_fast_forward',
        'l2': 'shader_prev',
        'r2': 'shader_next',
        'a': 'reset'
    }

    # Clone previous dictionnary and add 'b': 'menu_toggle'
    retroarchspecials = dict(b='menu_toggle', **retroarchspecialsnomenu)

    # Map an emulationstation button name to the corresponding retroarch name
    retroarchbtns = \
    {
        'a': 'a',
        'b': 'b',
        'x': 'x',
        'y': 'y',
        'pageup': 'l',
        'pagedown': 'r',
        'l2': 'l2',
        'r2': 'r2',
        'l3': 'l3',
        'r3': 'r3',
        'start': 'start',
        'select': 'select'
    }

    def __init__(self, system, settings, controllers):
        self.system = system
        self.settings = settings
        self.controllers = controllers

    # Fill controllers configuration
    def fillControllersConfiguration(self):
        recalbox = self.system.config

        # Cleanup all
        self.cleanUpControllers()

        # Build configurations
        for controller in self.controllers:
            self.buildController(self.controllers[controller], controller)

        # Set Hotkey
        self.setHotKey()

        # Set driver
        if recalbox.get("inputdriver", "auto") in ("auto", None):
            recalbox["inputdriver"] = self.getInputDriver()
        self.settings.setOption("input_joypad_driver", recalbox["inputdriver"])

        return self.settings

    # Find the best driver for controllers
    def getInputDriver(self):
        for controller in self.controllers:
            for controllerName in self.sdl2driverControllers:
                if controllerName in self.controllers[controller].realName:
                    return "sdl2"
        return "udev"

    # Remove all controller configurations
    def cleanUpControllers(self):
        self.settings.removeOptionStartingWith("input_player")
        for specialkey in self.retroarchspecials.itervalues():
            self.settings.removeOptionStartingWith("input_" + specialkey)

    # Write the hotkey for player 1
    def setHotKey(self):
        if "1" in self.controllers:
            if "hotkey" in self.controllers["1"].inputs:
                self.settings.setOption("input_enable_hotkey_btn", self.controllers["1"].inputs["hotkey"].id)

    # Return the retroarch analog_dpad_mode
    @staticmethod
    def getAnalogMode(controller):
        # if system.name != 'psx':
        for dirkey in LibretroControllers.retroarchdirs:
            if dirkey in controller.inputs:
                if controller.inputs[dirkey].type in ("button", "hat"):
                    return "1"
        return "0"

    # Return the playstation analog mode for a controller
    @staticmethod
    def getAnalogCoreMode(controller):
        for dirkey in LibretroControllers.retroarchdirs:
            if dirkey in controller.inputs:
                if controller.inputs[dirkey].type in ("button", "hat"):
                    return 'analog'
        return 'standard'

    # Returns the value to write in retroarch config file, depending on the type
    @staticmethod
    def getConfigValue(inp):
        if inp.type == 'button':
            return inp.id
        if inp.type == 'axis':
            if inp.value == '-1':
                return '-%s' % inp.id
            else:
                return '+%s' % inp.id
        if inp.type == 'hat':
            return 'h' + inp.id + LibretroControllers.hatstoname[inp.value]
        if inp.type == 'key':
            return inp.id

        raise TypeError

    # Write a configuration for a specified controller
    def buildController(self, controller, playerIndex):
        settings = self.settings

        # Get specials string or default
        specials = self.system.config.get("specials", "default")

        # config['input_device'] = '"%s"' % controller.realName
        for btnkey in self.retroarchbtns:
            btnvalue = self.retroarchbtns[btnkey]
            if btnkey in controller.inputs:
                inp = controller.inputs[btnkey]
                settings.setOption("input_player%s_%s_%s" % (controller.player, btnvalue, self.typetoname[inp.type]),
                                   self.getConfigValue(inp))
        for dirkey in self.retroarchdirs:
            dirvalue = self.retroarchdirs[dirkey]
            if dirkey in controller.inputs:
                inp = controller.inputs[dirkey]
                settings.setOption("input_player%s_%s_%s" % (controller.player, dirvalue, self.typetoname[inp.type]),
                                   self.getConfigValue(inp))
        for jskey in self.retroarchjoysticks:
            jsvalue = self.retroarchjoysticks[jskey]
            if jskey in controller.inputs:
                inp = controller.inputs[jskey]
                settings.setOption("input_player%s_%s_minus_axis" % (controller.player, jsvalue), "-%s" % inp.id)
                settings.setOption("input_player%s_%s_plus_axis" % (controller.player, jsvalue),  "+%s" % inp.id)
        if controller.player == "1":
            specialMap = dict()
            if specials == "nomenu":
                specialMap = self.retroarchspecialsnomenu
            if specials == "default":
                specialMap = self.retroarchspecials
            for specialkey in specialMap:
                specialvalue = specialMap[specialkey]
                if specialkey in controller.inputs:
                    inp = controller.inputs[specialkey]
                    settings.setOption("input_%s_%s" % (specialvalue, self.typetoname[inp.type]), self.getConfigValue(inp))
            specialvalue = self.retroarchspecials["start"]
            inp = controller.inputs["start"]
            settings.setOption("input_%s_%s" % (specialvalue, self.typetoname[inp.type]), self.getConfigValue(inp))

        # Assign pad to player
        settings.setOption("input_player{}_joypad_index".format(playerIndex), controller.index)
        settings.setOption("input_player{}_analog_dpad_mode".format(playerIndex), self.getAnalogMode(controller))



