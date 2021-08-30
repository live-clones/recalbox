#!/usr/bin/env python
from typing import Dict

from configgen.controllers.controller import Controller, InputItem
from configgen.controllers.controller import ControllerPerPlayer
import configgen.recalboxFiles as recalboxFiles


def generateControllerConfig(self, playersControllers: ControllerPerPlayer):

    ################ Mapping Conversion Supermodel to Recalbox ################

    # set recalbox Hotkey
    HOTKEY_BUTTONS: Dict[str, int] = \
    {
        "InputUIExit":              InputItem.ItemStart,
        "InputUISaveState":         InputItem.ItemY,
        "InputUIChangeSlot":        InputItem.ItemUp,
        "InputUILoadState":         InputItem.ItemX,
        "InputUIScreenShot":        InputItem.ItemL1,
        "InputUIReset":             InputItem.ItemA,
        "InputUIPause":             InputItem.ItemB,
    }

    SERVICE_TEST_BUTTON: Dict[str, int] = \
    {    
        "InputServiceA":            InputItem.ItemR3,
        "InputTestA":               InputItem.ItemL3,
    }

##################### Player 1 controllers ###############################

    BUTTONS_P1: Dict[str, int] = \
    {
        "InputCoin1":               InputItem.ItemSelect,
        "InputStart1":              InputItem.ItemStart,
        # Fighting game buttons
        "InputEscape":              InputItem.ItemY,
        "InputGuard":               InputItem.ItemX,
        "InputKick":                InputItem.ItemB,
        "InputPunch":               InputItem.ItemA,
        # Spikeout buttons
        "InputBeat":                InputItem.ItemB,
        "InputCharge":              InputItem.ItemX,
        "InputJump":                InputItem.ItemA,
        "InputShift":               InputItem.ItemY,
        # Virtua Striker buttons
        "InputLongPass":            InputItem.ItemB,
        "InputShoot":               InputItem.ItemY,
        "InputShortPass":           InputItem.ItemA,
        # Ski Champ controls
        "InputSkiPollLeft":         InputItem.ItemL1,
        "InputSkiPollRight":        InputItem.ItemR1,
        "InputSkiSelect1":          InputItem.ItemY,
        "InputSkiSelect2":          InputItem.ItemB,
        "InputSkiSelect3":          InputItem.ItemA,
        # Magical Truck Adventure controls
        "InputMagicalPedal1":       InputItem.ItemB,
        # Handbrake (Dirt Devils, Sega Rally 2)
        "InputHandBrake":           InputItem.ItemY,
        # Harley-Davidson controls
        "InputMusicSelect":         InputItem.ItemB,
        "InputRearBrake":           InputItem.ItemA,
        # Virtual On buttons
        "InputTwinJoyShot1":        InputItem.ItemX,
        "InputTwinJoyShot2":        InputItem.ItemY,
        "InputTwinJoyTurbo1":       InputItem.ItemL1,
        "InputTwinJoyTurbo2":       InputItem.ItemR1,
        # Virtual On macros
        "InputTwinJoyCrouch":       InputItem.ItemA,
        "InputTwinJoyJump":         InputItem.ItemB,
        # Up/down shifter manual transmission (all racers)
        "InputGearShiftDown":       InputItem.ItemL1,
        "InputGearShiftUp":         InputItem.ItemR1,
        # 4-Speed manual transmission (Daytona 2, Sega Rally 2, Scud Race)
        # unsettings N
        "InputGearShiftN":          -1,
        # VR4 view change buttons (Daytona 2, Le Mans 24, Scud Race)
        "InputVR1":                 InputItem.ItemY,
        "InputVR2":                 InputItem.ItemX,
        "InputVR3":                 InputItem.ItemA,
        "InputVR4":                 InputItem.ItemB,
        # Single view change button (Dirt Devils, ECA, Harley-Davidson, Sega Rally 2)
        "InputViewChange":          InputItem.ItemX,
        # Sega Bass Fishing / Get Bass controls
        "InputFishingCast":         InputItem.ItemY,
        "InputFishingSelect":       InputItem.ItemA,
    }

    DIGITAL_JOYSTICK_P1: Dict[str, int] = \
    {
        "InputJoyUp":               InputItem.ItemUp,
        "InputJoyDown":             InputItem.ItemDown,
        "InputJoyLeft":             InputItem.ItemLeft,
        "InputJoyRight":            InputItem.ItemRight,
        # Ski Champ controls
        "InputSkiLeft":             InputItem.ItemLeft,
        "InputSkiRight":            InputItem.ItemRight,
        "InputSkiUp":               InputItem.ItemUp,
        "InputSkiDown":             InputItem.ItemDown,
        # Steering wheel
        # Set digital, turn wheel
        "InputSteeringLeft":        InputItem.ItemLeft,
        "InputSteeringRight":       InputItem.ItemRight,
        # Magical Truck Adventure controls
        "InputMagicalLeverUp1":     InputItem.ItemUp,
        "InputMagicalLeverDown1":   InputItem.ItemDown,
        # Sega Bass Fishing / Get Bass controls
        "InputFishingRodLeft":      InputItem.ItemLeft,
        "InputFishingRodRight":     InputItem.ItemRight,
        "InputFishingRodUp":        InputItem.ItemUp,
        "InputFishingRodDown":      InputItem.ItemDown,
        # need setting only AXIS don't need a position (POS/NEG)
        # Magical Truck Adventure controls
        "InputMagicalLever1":       InputItem.ItemJoy1Up,
        # Steering wheel
        "InputSteering":            InputItem.ItemJoy1Left,
        # Ski Champ controls
        "InputSkiX":                InputItem.ItemJoy1Left,
        "InputSkiY":                InputItem.ItemJoy1Up,
        # Sega Bass Fishing / Get Bass controls
        "InputFishingRodX":         InputItem.ItemJoy2Left,
        "InputFishingRodY":         InputItem.ItemJoy2Up,
        "InputFishingStickX":       InputItem.ItemJoy1Left,
        "InputFishingStickY":       InputItem.ItemJoy1Up,
    }

    BUTTONS_AXIS: Dict[str, int] = \
    {
        #### NEED A BYPASS BUTTON OR AXIS ####
        # Pedals
        "InputAccelerator":         InputItem.ItemR2,
        "InputBrake":               InputItem.ItemL2,
        #### NEED A BYPASS BUTTON OR AXIS ####
        "InputFishingReel":         InputItem.ItemL2,
        "InputFishingTension":      InputItem.ItemR2,
    }

    ANALOG_JOYSTICK_P1: Dict[str, int] = \
    {
        # Need setting all axis position (POS/NEG)
        # Virtual On individual joystick mapping
        "InputTwinJoyDown1":        InputItem.ItemJoy1Down,
        "InputTwinJoyDown2":        InputItem.ItemJoy2Down,
        "InputTwinJoyLeft1":        InputItem.ItemJoy1Left,
        "InputTwinJoyLeft2":        InputItem.ItemJoy2Left,
        "InputTwinJoyRight1":       InputItem.ItemJoy1Right,
        "InputTwinJoyRight2":       InputItem.ItemJoy2Right,
        "InputTwinJoyUp1":          InputItem.ItemJoy1Up,
        "InputTwinJoyUp2":          InputItem.ItemJoy2Up,
        # 4-Speed manual transmission (Daytona 2, Sega Rally 2, Scud Race)
        # Setting on joystick2
        "InputGearShift1":          InputItem.ItemJoy2Up,
        "InputGearShift2":          InputItem.ItemJoy2Down,
        "InputGearShift3":          InputItem.ItemJoy2Left,
        "InputGearShift4":          InputItem.ItemJoy2Right,
    }

    MOUSE_GAME: Dict[str, int] = \
    {
        # Light guns (Lost World)
        "InputGunX":                InputItem.ItemJoy1Left,
        "InputGunY":                InputItem.ItemJoy1Up,
        # Analog guns (Ocean Hunter, LA Machineguns)
        "InputAnalogGunX":          InputItem.ItemJoy1Left,
        "InputAnalogGunY":          InputItem.ItemJoy1Up,
        # Analog joystick (Star Wars Trilogy)
        "InputAnalogJoyX":          InputItem.ItemJoy1Left,
        "InputAnalogJoyY":          InputItem.ItemJoy1Up,
    }

    ####################### Players 2 controllers ############################

    BUTTONS_P2: Dict[str, int] = \
    {
        # Commons buttons
        "InputCoin2":               InputItem.ItemSelect,
        "InputStart2":              InputItem.ItemStart,
        # Fighting game buttons
        "InputEscape2":             InputItem.ItemY,
        "InputGuard2":              InputItem.ItemX,
        "InputKick2":               InputItem.ItemB,
        "InputPunch2":              InputItem.ItemA,
        # Virtua Striker buttons
        "InputLongPass2":           InputItem.ItemA,
        "InputShoot2":              InputItem.ItemX,
        "InputShortPass2":          InputItem.ItemB,
        # Magical Truck Adventure controls
        "InputMagicalPedal2":       InputItem.ItemB,
    }

    DIGITAL_JOYSTICK_P2: Dict[str, int] = \
    {
        "InputJoyDown2":            InputItem.ItemDown,
        "InputJoyLeft2":            InputItem.ItemLeft,
        "InputJoyRight2":           InputItem.ItemRight,
        "InputJoyUp2":              InputItem.ItemUp,
        "InputMagicalLeverDown2":   InputItem.ItemDown,
        "InputMagicalLeverUp2":     InputItem.ItemUp,
        # Magical Truck Adventure controls
        "InputMagicalLever2":       InputItem.ItemJoy1Left, #change to joyleft
    }

    ###### Map an Recalbox direction to the corresponding Supermodel ######

    TYPE_TO_NAME: Dict[int, str] = \
    {
        InputItem.TypeAxis:   'AXIS',
        InputItem.TypeButton: 'BUTTON',
        InputItem.TypeHat:    'POV',
    }

    HATS_TO_NAME: Dict[int, str] = \
    {
        1:                        'UP',
        2:                        'RIGHT',
        4:                        'DOWN',
        8:                        'LEFT',
    }

    """SUPERMODEL_DIR = \
    {
        'down':                     'DOWN',
        'left':                     'LEFT',
        'right':                    'RIGHT',
        'up':                       'UP',
    }"""

    SUPERMODEL_JOY: Dict[int, str] = \
    {
        # lEFT JOYSTICK
        InputItem.ItemJoy1Down:            'YAXIS',
        InputItem.ItemJoy1Left:            'XAXIS',
        InputItem.ItemJoy1Right:           'XAXIS',
        InputItem.ItemJoy1Up:              'YAXIS',
        # RIGHT JOYSTICK
        InputItem.ItemJoy2Down:            'RYAXIS',
        InputItem.ItemJoy2Left:            'RXAXIS',
        InputItem.ItemJoy2Right:           'RXAXIS',
        InputItem.ItemJoy2Up:              'RYAXIS',
        # AXIS ON L2/R2 BUTTONS
        InputItem.ItemL2:                  'ZAXIS',
        InputItem.ItemR2:                  'RZAXIS',
    }

    def getConfigValue(inputItem: InputItem, targetConf: Dict[str, int]):
        # Output format BUTTONX
        if inputItem.IsButton:
            return '{}{}'.format(TYPE_TO_NAME[inputItem.Type], inputItem.Id + 1)
        # Ouput format RYAXIS, RXAXIS, XAXIS, YAXIS
        if inputItem.IsAxis:
            return '{}'.format(SUPERMODEL_JOY[targetConf[x]])
        # Output format POV1_DOWN
        if inputItem.IsHat:
            return '{}{}_{}'.format(TYPE_TO_NAME[inputItem.Type], inputItem.Id + 1, HATS_TO_NAME[inputItem.Value])

        raise TypeError

    def getConfigValueJ1(inputItem: InputItem):
        return getConfigValue(inputItem, DIGITAL_JOYSTICK_P1)

    def getConfigValueJ2(inputItem: InputItem):
        return getConfigValue(inputItem, DIGITAL_JOYSTICK_P2)

    # More Games need a Position value of Axis 
    def getPositionConfigValue(inputItem: InputItem):
        if inputItem.IsAxis:
            # Output format XAXIS_NEG/XAXIS_POS ,RXAXIS_NEG/RXAXIS_POS
            if inputItem.Value == -1:
                return '{}_NEG'.format(SUPERMODEL_JOY[ANALOG_JOYSTICK_P1[x]])
            else:
                return '{}_POS'.format(SUPERMODEL_JOY[ANALOG_JOYSTICK_P1[x]])

        raise TypeError
    # For R2/L2 axis or button
    def getAxisOrButton(inputItem: InputItem):
        if inputItem.IsButton:
            return '{}{}'.format(TYPE_TO_NAME[inputItem.Type], inputItem.Id + 1)
        elif inputItem.IsAxis:
            return '{}_POS'.format(SUPERMODEL_JOY[BUTTONS_AXIS[x]])

        raise TypeError

    def getlightGunConfigValue(inputItem: InputItem):
        if inputItem.IsAxis:
            return 'MOUSE_{}'.format(SUPERMODEL_JOY[MOUSE_GAME[x]])

        raise TypeError

    # Load Configuration
    from configgen.settings.iniSettings import IniSettings
    supermodelControllersSettings = IniSettings(recalboxFiles.supermodelControlsIni, True)
    supermodelControllersSettings.loadFile(True)
    from configgen.settings.keyValueSettings import keyValueSettings
    supermodelSettings = keyValueSettings(recalboxFiles.supermodelConfigFile)
    supermodelSettings.loadFile(True)

    ## Set default configuration 
    ## set emulated Net options desactived on default doesn't work on linux at the moment
    ## Network board - experimental build for win32 only
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "AddressOut", '"127.0.0.1"') \
                                 .setInt(self.SECTION_GLOBAL, "Network", 0) \
                                 .setInt(self.SECTION_GLOBAL, "SimulateNet", 0) \
                                 .setInt(self.SECTION_GLOBAL, "PortIn", 1970) \
                                 .setInt(self.SECTION_GLOBAL, "PortOut", 1971)

    ## Set auto triggers activate on default
    ## automatic reload when off-screen
    supermodelControllersSettings.setInt(self.SECTION_GLOBAL, "InputAutoTrigger", 1) \
                                 .setInt(self.SECTION_GLOBAL, "InputAutoTrigger2", 1)

    # Set sensitivity analog configurable in ConfigModel3.ini
    #InputKeySensitivity
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputKeySensitivity", supermodelSettings.getString("sensitivity", ""))

    # Set deadzone analog configurable in ConfigModel3.ini
    deadzone = supermodelSettings.getString("deadzone", "")
    # joystick1 Player 1
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy1XDeadZone", deadzone) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1YDeadZone", deadzone)
    # joystick2 player 1
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy1RXDeadZone", deadzone) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1RYDeadZone", deadzone)
    # triggers R2/L2 player 1
#    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1RZDeadZone", deadzone)
#    supermodelControllersSettings.setOption(self.SECTION_GLOBAL, "InputJoy1ZDeadZone", deadzone)
    # joystick1 Player 2
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy2YDeadZone", deadzone) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2ZDeadZone", deadzone)
    # joystick2 Player 2
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy2RXDeadZone", deadzone) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2RYDeadZone", deadzone)
    # triggers R2/L2 player 2
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy2RZDeadZone", deadzone) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2XDeadZone", deadzone)


    # Set Saturation analog configurable in ConfigModel3.ini
    saturation = supermodelSettings.getString("saturation", "")
    # joystick1 Players 1
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy1RXSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1RYSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1RZSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1XSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1YSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy1ZSaturation", saturation)
    # joystick2 Players 2
    supermodelControllersSettings.setString(self.SECTION_GLOBAL, "InputJoy2RXSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2RYSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2RZSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2XSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2YSaturation", saturation) \
                                 .setString(self.SECTION_GLOBAL, "InputJoy2ZSaturation", saturation)

    for playercontroller in playersControllers:
        pad: Controller = playersControllers[playercontroller]
        # we only care about player 1
        if pad.PlayerIndex != 1:
            continue

        # increase +1 controller index 
        padIndex = 'JOY{}'.format(pad.SdlIndex + 1)

        # Add hotkey buttons
        for x in HOTKEY_BUTTONS:
            if pad.HasInput(HOTKEY_BUTTONS[x]):
                inp = pad.Input(HOTKEY_BUTTONS[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}+{}_{}"'.format(padIndex, getConfigValueJ1(pad.Input(InputItem.ItemHotkey)), padIndex, getConfigValueJ1(inp)))

        # Service and Test menu
        # Set on 0 or 1 in ConfigModel3.ini
        for x in SERVICE_TEST_BUTTON:
            if pad.HasInput(SERVICE_TEST_BUTTON[x]):
                inp = pad.Input(SERVICE_TEST_BUTTON[x])
                ServiceBtn = supermodelSettings.setString("service-button", "")
                if ServiceBtn == "1" :
                    supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ1(inp)))
                else:
                    supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, "NONE")

        for x in BUTTONS_P1:
            if pad.HasInput(BUTTONS_P1[x]):
                inp = pad.Input(BUTTONS_P1[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ1(inp)))

        for x in DIGITAL_JOYSTICK_P1:
            if pad.HasInput(DIGITAL_JOYSTICK_P1[x]):
                inp = pad.Input(DIGITAL_JOYSTICK_P1[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ1(inp)))

        for x in ANALOG_JOYSTICK_P1:
            if pad.HasInput(ANALOG_JOYSTICK_P1[x]): # for up/left direction
                inp = pad.Input(ANALOG_JOYSTICK_P1[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getPositionConfigValue(inp)))

        for x in BUTTONS_AXIS:
            if pad.HasInput(BUTTONS_AXIS[x]):
                inp = pad.Input(BUTTONS_AXIS[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getAxisOrButton(inp)))

        for x in MOUSE_GAME:
            if pad.HasInput(MOUSE_GAME[x]):
                inp = pad.Input(MOUSE_GAME[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}"'.format(getlightGunConfigValue(inp)))

        if pad.PlayerIndex == 2:
            continue

        padIndex = 'JOY{}'.format(pad.SdlIndex + 2)

        for x in BUTTONS_P2:
            if pad.HasInput(BUTTONS_P2[x]):
                inp = pad.Input(BUTTONS_P2[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ2(inp)))

        for x in DIGITAL_JOYSTICK_P2:
            if pad.HasInput(DIGITAL_JOYSTICK_P2[x]):
                inp = pad.Input(DIGITAL_JOYSTICK_P2[x])
                supermodelControllersSettings.setString(self.SECTION_GLOBAL, x, '"{}_{}"'.format(padIndex, getConfigValueJ2(inp)))

        break

    # Save configuration
    supermodelControllersSettings.saveFile()
