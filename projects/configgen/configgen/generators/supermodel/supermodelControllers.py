#!/usr/bin/env python
from typing import Dict

import configgen.recalboxFiles as recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.settings.iniSettings import IniSettings
from configgen.controllersConfig import Controller, Input
from configgen.controllersConfig import ControllerDictionary


def generateControllerConfig(self, playersControllers: ControllerDictionary):

    ################ Mapping Conversion Supermodel to Recalbox ################

    # set recalbox Hotkey
    HOTKEY_BUTTONS = \
    {
        "InputUIExit":              "start", 
        "InputUISaveState":         "y",
        "InputUIChangeSlot":        "up",
        "InputUILoadState":         "x",
        "InputUIScreenShot":        "l1",
        "InputUIReset":             "a",
        "InputUIPause":             "b",
    }

    SERVICE_TEST_BUTTON = \
    {    
        "InputServiceA":            "r3",
        "InputTestA":               "l3",
    }

##################### Player 1 controllers ###############################

    BUTTONS_P1 = \
    {
        "InputCoin1":               "select",
        "InputStart1":              "start",
        # Fighting game buttons
        "InputEscape":              "y",
        "InputGuard":               "x",
        "InputKick":                "b",
        "InputPunch":               "a",
        # Spikeout buttons
        "InputBeat":                "b",
        "InputCharge":              "x",
        "InputJump":                "a",
        "InputShift":               "y",
        # Virtua Striker buttons
        "InputLongPass":            "b",
        "InputShoot":               "y",
        "InputShortPass":           "a",
        # Ski Champ controls
        "InputSkiPollLeft":         "l1",
        "InputSkiPollRight":        "r1",
        "InputSkiSelect1":          "y",
        "InputSkiSelect2":          "b",
        "InputSkiSelect3":          "a",
        # Magical Truck Adventure controls
        "InputMagicalPedal1":       "b",
        # Handbrake (Dirt Devils, Sega Rally 2)
        "InputHandBrake":           "y",
        # Harley-Davidson controls
        "InputMusicSelect":         "b",
        "InputRearBrake":           "a",
        # Virtual On buttons
        "InputTwinJoyShot1":        "x",
        "InputTwinJoyShot2":        "y",
        "InputTwinJoyTurbo1":       "l1",
        "InputTwinJoyTurbo2":       "r1",
        # Virtual On macros
        "InputTwinJoyCrouch":       "a",
        "InputTwinJoyJump":         "b",
        # Up/down shifter manual transmission (all racers)
        "InputGearShiftDown":       "l1",
        "InputGearShiftUp":         "r1", 
        # 4-Speed manual transmission (Daytona 2, Sega Rally 2, Scud Race)
        # unsettings N
        "InputGearShiftN":          "NONE",
        # VR4 view change buttons (Daytona 2, Le Mans 24, Scud Race)
        "InputVR1":                 "y",
        "InputVR2":                 "x",
        "InputVR3":                 "a",
        "InputVR4":                 "b",
        # Single view change button (Dirt Devils, ECA, Harley-Davidson, Sega Rally 2)
        "InputViewChange":          "x",
        # Sega Bass Fishing / Get Bass controls
        "InputFishingCast":         "y",
        "InputFishingSelect":       "a",
    }

    DIGITAL_JOYSTICK_P1 = \
    {
        "InputJoyUp":               "up",
        "InputJoyDown":             "down",
        "InputJoyLeft":             "left",
        "InputJoyRight":            "right",
        # Ski Champ controls
        "InputSkiLeft":             "left",
        "InputSkiRight":            "right",
        "InputSkiUp":               "up",
        "InputSkiDown":             "down",
        # Steering wheel
        # Set digital, turn wheel
        "InputSteeringLeft":        "left",
        "InputSteeringRight":       "right",
        # Magical Truck Adventure controls
        "InputMagicalLeverUp1":     "up",
        "InputMagicalLeverDown1":   "down",
        # Sega Bass Fishing / Get Bass controls
        "InputFishingRodLeft":      "left",
        "InputFishingRodRight":     "right",
        "InputFishingRodUp":        "up",
        "InputFishingRodDown":      "down",
        # need setting only AXIS don't need a position (POS/NEG)
        # Magical Truck Adventure controls
        "InputMagicalLever1":       "joystick1up",
        # Steering wheel
        "InputSteering":            "joystick1left",
        # Ski Champ controls
        "InputSkiX":                "joystick1left",
        "InputSkiY":                "joystick1up",
        # Sega Bass Fishing / Get Bass controls
        "InputFishingRodX":         "joystick2left",
        "InputFishingRodY":         "joystick2up",
        "InputFishingStickX":       "joystick1left",
        "InputFishingStickY":       "joystick1up",
    }

    BUTTONS_AXIS = \
    {
        #### NEED A BYPASS BUTTON OR AXIS ####
        # Pedals
        "InputAccelerator":         "r2",
        "InputBrake":               "l2",
        #### NEED A BYPASS BUTTON OR AXIS ####
        "InputFishingReel":         "l2",
        "InputFishingTension":      "r2",
    }

    ANALOG_JOYSTICK_P1 = \
    {
        # Need setting all axis position (POS/NEG)
        # Virtual On individual joystick mapping
        "InputTwinJoyDown1":        "joystick1down",
        "InputTwinJoyDown2":        "joystick2down",
        "InputTwinJoyLeft1":        "joystick1left",
        "InputTwinJoyLeft2":        "joystick2left",
        "InputTwinJoyRight1":       "joystick1right",
        "InputTwinJoyRight2":       "joystick2right",
        "InputTwinJoyUp1":          "joystick1up",
        "InputTwinJoyUp2":          "joystick2up",
        # 4-Speed manual transmission (Daytona 2, Sega Rally 2, Scud Race)
        # Setting on joystick2
        "InputGearShift1":          "joystick2up",
        "InputGearShift2":          "joystick2down",
        "InputGearShift3":          "joystick2left",
        "InputGearShift4":          "joystick2right",
    }

    MOUSE_GAME = \
    {
        # Light guns (Lost World)
        "InputGunX":                "joystick1left",
        "InputGunY":                "joystick1up",
        # Analog guns (Ocean Hunter, LA Machineguns)
        "InputAnalogGunX":          "joystick1left",
        "InputAnalogGunY":          "joystick1up",
        # Analog joystick (Star Wars Trilogy)
        "InputAnalogJoyX":          "joystick1left",
        "InputAnalogJoyY":          "joystick1up",
    }

####################### Players 2 controllers ############################

    BUTTONS_P2 = \
    {
        # Commons buttons
        "InputCoin2":               "select",
        "InputStart2":              "start",
        # Fighting game buttons
        "InputEscape2":             "y",
        "InputGuard2":              "x",
        "InputKick2":               "b",
        "InputPunch2":              "a",
        # Virtua Striker buttons
        "InputLongPass2":           "a",
        "InputShoot2":              "x",
        "InputShortPass2":          "b",
        # Magical Truck Adventure controls
        "InputMagicalPedal2":       "b",
    }

    DIGITAL_JOYSTICK_P2 = \
    {
        "InputJoyDown2":            "down",
        "InputJoyLeft2":            "left",
        "InputJoyRight2":           "right",
        "InputJoyUp2":              "up",
        "InputMagicalLeverDown2":   "down",
        "InputMagicalLeverUp2":     "up",
        # Magical Truck Adventure controls
        "InputMagicalLever2":       "joystick1left", #change to joyleft
    }

    ###### Map an Recalbox direction to the corresponding Supermodel ######

    TYPE_TO_NAME = \
    {
        'axis':                     'AXIS',
        'button':                   'BUTTON',
        'hat':                      'POV',
    }

    HATS_TO_NAME = \
    {
        '1':                        'UP',
        '2':                        'RIGHT',
        '4':                        'DOWN',
        '8':                        'LEFT',
    }

    """SUPERMODEL_DIR = \
    {
        'down':                     'DOWN',
        'left':                     'LEFT',
        'right':                    'RIGHT',
        'up':                       'UP',
    }"""

    SUPERMODEL_JOY = \
    {
        # lEFT JOYSTICK
        'joystick1down':            'YAXIS',
        'joystick1left':            'XAXIS',
        'joystick1right':           'XAXIS',
        'joystick1up':              'YAXIS',
        # RIGHT JOYSTICK
        'joystick2down':            'RYAXIS',
        'joystick2left':            'RXAXIS',
        'joystick2right':           'RXAXIS',
        'joystick2up':              'RYAXIS',
        # AXIS ON L2/R2 BUTTONS
        'l2':                       'ZAXIS',
        'r2':                       'RZAXIS', 
    }

    def getControllerItem(controller: Controller, key: str):
        fake = False
        realKey = key
        if key == 'joystick1right':
            realKey = 'joystick1left'
            fake = True
        if key == 'joystick2right':
            realKey = 'joystick2left'
            fake = True
        if key == 'joystick1down':
            realKey = 'joystick1up'
            fake = True
        if key == 'joystick2down':
            realKey = 'joystick2up'
            fake = True

        # Not available?
        if realKey not in controller.inputs:
            return None

        # Real input?
        if not fake:
            return controller.inputs[key]

        # Build a fake input
        fakeInput = controller.inputs[realKey].clone()
        fakeInput.name = key
        fakeInput.value = str(-int(fakeInput.value))
        return fakeInput

    def getConfigValue(inputItem: Input, targetConf: Dict[str, str]):
        # Output format BUTTONX
        if inputItem.type == 'button':
            return '{}{}'.format(TYPE_TO_NAME[inputItem.type], int(inputItem.id) + 1)
        # Ouput format RYAXIS, RXAXIS, XAXIS, YAXIS
        if inputItem.type == 'axis':
            return '{}'.format(SUPERMODEL_JOY[targetConf[x]])
        # Output format POV1_DOWN
        if inputItem.type == 'hat':
            return '{}{}_{}'.format(TYPE_TO_NAME[inputItem.type], int(inputItem.id) + 1, HATS_TO_NAME[inputItem.value])

        raise TypeError

    def getConfigValueJ1(inputItem: Input):
        return getConfigValue(inputItem, DIGITAL_JOYSTICK_P1)

    def getConfigValueJ2(inputItem):
        return getConfigValue(inputItem, DIGITAL_JOYSTICK_P2)

    # More Games need a Position value of Axis 
    def getPositionConfigValue(inputItem):
        if inputItem.type == 'axis':
            # Output format XAXIS_NEG/XAXIS_POS ,RXAXIS_NEG/RXAXIS_POS
            if inputItem.value == '-1':
                return '{}_NEG'.format(SUPERMODEL_JOY[ANALOG_JOYSTICK_P1[x]])
            else:
                return '{}_POS'.format(SUPERMODEL_JOY[ANALOG_JOYSTICK_P1[x]])

        raise TypeError
    # For R2/L2 axis or button
    def getAxisOrButton(inputItem):
        if inputItem.type == 'button':
            return '{}{}'.format(TYPE_TO_NAME[inputItem.type], int(inputItem.id) + 1)
        elif inputItem.type == 'axis':
            return '{}_POS'.format(SUPERMODEL_JOY[BUTTONS_AXIS[x]])

        raise TypeError

    def getlightGunConfigValue(inputItem):
        if inputItem.type == 'axis':
            return 'MOUSE_{}'.format(SUPERMODEL_JOY[MOUSE_GAME[x]])

        raise TypeError

    # Load Configuration
    supermodelControllersSettings = IniSettings(recalboxFiles.supermodelControlsIni, True)
    supermodelControllersSettings.loadFile(True)
    supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
    supermodelSettings.loadFile(True)

    ## Set default configuration 
    ## set emulated Net options desactived on default doesn't work on linux at the moment
    ## Network board - experimental build for win32 only
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "Network", "0")
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "SimulateNet", "0")
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "PortIn", "1970")
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "PortOut", "1971")
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "AddressOut", '"127.0.0.1"')

    ## Set auto triggers activate on default
    ## automatic reload when off-screen
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputAutoTrigger", "1")
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputAutoTrigger2", "1")

    # Set sensitivity analog configurable in ConfigModel3.ini
    sensitivity = supermodelSettings.getOption("sensitivity", "")
    #InputKeySensitivity
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputKeySensitivity", sensitivity)

    # Set deadzone analog configurable in ConfigModel3.ini
    deadzone = supermodelSettings.getOption("deadzone", "")
    # joystick1 Player 1
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1XDeadZone", deadzone)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1YDeadZone", deadzone)
    # joystick2 player 1
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RXDeadZone", deadzone)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RYDeadZone", deadzone)
    # triggers R2/L2 player 1
#    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RZDeadZone", deadzone)
#    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1ZDeadZone", deadzone)
    # joystick1 Player 2
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2YDeadZone", deadzone)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2ZDeadZone", deadzone)
    # joystick2 Player 2
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2RXDeadZone", deadzone)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2RYDeadZone", deadzone)
    # triggers R2/L2 player 2
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2RZDeadZone", deadzone)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2XDeadZone", deadzone)


    # Set Saturation analog configurable in ConfigModel3.ini
    saturation = supermodelSettings.getOption("saturation", "")
    # joystick1 Players 1
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RXSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RYSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RZSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1XSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1YSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1ZSaturation", saturation)
    # joystick2 Players 2
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2RXSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2RYSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2RZSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2XSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2YSaturation", saturation)
    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy2ZSaturation", saturation)

    for playercontroller in playersControllers:
        pad = playersControllers[playercontroller]
        # we only care about player 1
        if pad.player != "1":
            continue

        # increase +1 controller index 
        padIndex = 'JOY{}'.format(int(pad.index) + 1)

        # Add hotkey buttons
        for x in HOTKEY_BUTTONS:
            if HOTKEY_BUTTONS[x] in pad.inputs:
                inp = pad.inputs[HOTKEY_BUTTONS[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}+{}_{}"'.format(padIndex, getConfigValueJ1(pad.inputs["hotkey"]), padIndex, getConfigValueJ1(inp)))

        # Service and Test menu
        # Set on 0 or 1 in ConfigModel3.ini
        for x in SERVICE_TEST_BUTTON:
            if SERVICE_TEST_BUTTON[x] in pad.inputs:
                inp = pad.inputs[SERVICE_TEST_BUTTON[x]]
                ServiceBtn = supermodelSettings.getOption("service-button", "")
                if ServiceBtn == "1" :
                    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ1(inp)))
                else:
                    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, "NONE")

        for x in BUTTONS_P1:
            if BUTTONS_P1[x] in pad.inputs:
                inp = pad.inputs[BUTTONS_P1[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ1(inp)))

        for x in DIGITAL_JOYSTICK_P1:
            if DIGITAL_JOYSTICK_P1[x] in pad.inputs:
                inp = pad.inputs[DIGITAL_JOYSTICK_P1[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ1(inp)))

        for x in ANALOG_JOYSTICK_P1:   
            if ANALOG_JOYSTICK_P1[x] in pad.inputs: # for up/left direction
                inp = pad.inputs[ANALOG_JOYSTICK_P1[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getPositionConfigValue(inp)))
            else: # for down/right direction on joystick axis
                ReverseInput = getControllerItem(pad, ANALOG_JOYSTICK_P1[x])     
                if ReverseInput is not None:
                    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getPositionConfigValue(ReverseInput)))

        for x in BUTTONS_AXIS:
            if BUTTONS_AXIS[x] in pad.inputs:
                inp = pad.inputs[BUTTONS_AXIS[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getAxisOrButton(inp)))

        for x in MOUSE_GAME:
            if MOUSE_GAME[x] in pad.inputs:
                inp = pad.inputs[MOUSE_GAME[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}"'.format(getlightGunConfigValue(inp)))

        if pad.player == "2":
            continue

        padIndex = 'JOY{}'.format(int(pad.index) + 2)

        for x in BUTTONS_P2:
            if BUTTONS_P2[x] in pad.inputs:
                inp = pad.inputs[BUTTONS_P2[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ2(inp)))

        for x in DIGITAL_JOYSTICK_P2:
            if DIGITAL_JOYSTICK_P2[x] in pad.inputs:
                inp = pad.inputs[DIGITAL_JOYSTICK_P2[x]]
                supermodelControllersSettings.setOption(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ2(inp)))

        break

    # Save configuration
    supermodelControllersSettings.saveFile()
