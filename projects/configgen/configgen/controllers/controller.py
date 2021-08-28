#!/usr/bin/env python
from __future__ import annotations
from typing import Dict, List, Optional, ValuesView

# Do not simplify - Allow mocking of Input file path
from configgen.controllers.inputItem import InputItem, InputCollection
import configgen.recalboxFiles as recalboxFiles

esInputs = recalboxFiles.esInputs


class Controller:

    def __init__(self, deviceName: str, typeName: str, guid: str, playerIndex: int, sdlIndex: int,
                 inputs: Optional[InputCollection], devicePath: str, axesCount: int, hatsCount: int, buttonCount: int):
        self.__type: str = typeName
        self.__deviceName: str = deviceName
        self.__guid: str = guid
        self.__sdlIndex: int = sdlIndex       # SDL index - starting from 0
        self.__playerIndex: int = playerIndex # Player index - starting from 1
        self.__naturalIndex: int = 0          # Natural index - starting from 0
        self.__devicePath:str = devicePath
        self.__axesCount: int = axesCount
        self.__hatsCount: int = hatsCount
        self.__buttonsCount: int = buttonCount
        self.__inputs: Optional[InputCollection] = inputs if inputs is not None else {}
        self.axisesNumber: List[int] = self.setAxisNumberList() if inputs is not None else []

    # Getter

    @property
    def Type(self) -> str: return self.__type

    @property
    def GUID(self) -> str: return self.__guid

    @property
    def DeviceName(self) -> str: return self.__deviceName

    @property
    def DevicePath(self) -> str: return self.__devicePath

    @property
    def ButtonCount(self) -> int: return self.__buttonsCount

    @property
    def HatCount(self) -> int: return self.__hatsCount

    @property
    def AxisCount(self) -> int: return self.__axesCount

    @property
    def PlayerIndex(self) -> int: return self.__playerIndex

    @property
    def SdlIndex(self) -> int: return self.__sdlIndex

    @property
    def NaturalIndex(self) -> int: return self.__naturalIndex

    @property
    def HasStart(self) -> bool: return InputItem.ItemStart in self.__inputs
    @property
    def Start(self) -> InputItem: return self.__inputs[InputItem.ItemStart]

    @property
    def HasSelect(self) -> bool: return InputItem.ItemSelect in self.__inputs
    @property
    def Select(self) -> InputItem: return self.__inputs[InputItem.ItemSelect]

    @property
    def HasHotkey(self) -> bool: return InputItem.ItemHotkey in self.__inputs
    @property
    def Hotkey(self) -> InputItem: return self.__inputs[InputItem.ItemHotkey]

    @property
    def HasA(self) -> bool: return InputItem.ItemA in self.__inputs
    @property
    def A(self) -> InputItem: return self.__inputs[InputItem.ItemA]

    @property
    def HasB(self) -> bool: return InputItem.ItemB in self.__inputs
    @property
    def B(self) -> InputItem: return self.__inputs[InputItem.ItemB]

    @property
    def HasX(self) -> bool: return InputItem.ItemX in self.__inputs
    @property
    def X(self) -> InputItem: return self.__inputs[InputItem.ItemX]

    @property
    def HasY(self) -> bool: return InputItem.ItemY in self.__inputs
    @property
    def Y(self) -> InputItem: return self.__inputs[InputItem.ItemY]

    @property
    def L1(self) -> InputItem: return self.__inputs[InputItem.ItemL1]
    @property
    def HasL1(self) -> bool: return InputItem.ItemL1 in self.__inputs

    @property
    def R1(self) -> InputItem: return self.__inputs[InputItem.ItemR1]
    @property
    def HasR1(self) -> bool: return InputItem.ItemR1 in self.__inputs

    @property
    def L2(self) -> InputItem: return self.__inputs[InputItem.ItemL2]
    @property
    def HasL2(self) -> bool: return InputItem.ItemL2 in self.__inputs

    @property
    def R2(self) -> InputItem: return self.__inputs[InputItem.ItemR2]
    @property
    def HasR2(self) -> bool: return InputItem.ItemR2 in self.__inputs

    @property
    def Up(self) -> InputItem: return self.__inputs[InputItem.ItemUp]
    @property
    def HasUp(self) -> bool: return InputItem.ItemUp in self.__inputs

    @property
    def Right(self) -> InputItem: return self.__inputs[InputItem.ItemRight]
    @property
    def HasRight(self) -> bool: return InputItem.ItemRight in self.__inputs

    @property
    def Down(self) -> InputItem: return self.__inputs[InputItem.ItemDown]
    @property
    def HasDown(self) -> bool: return InputItem.ItemDown in self.__inputs

    @property
    def Left(self) -> InputItem: return self.__inputs[InputItem.ItemLeft]
    @property
    def HasLeft(self) -> bool: return InputItem.ItemLeft in self.__inputs

    @property
    def Joy1Up(self) -> InputItem: return self.__inputs[InputItem.ItemJoy1Up]
    @property
    def HasJoy1Up(self) -> bool: return InputItem.ItemJoy1Up in self.__inputs

    @property
    def Joy1Right(self) -> InputItem: return self.__inputs[InputItem.ItemJoy1Right]
    @property
    def HasJoy1Right(self) -> bool: return InputItem.ItemJoy1Right in self.__inputs

    @property
    def Joy1Down(self) -> InputItem: return self.__inputs[InputItem.ItemJoy1Down]
    @property
    def HasJoy1Down(self) -> bool: return InputItem.ItemJoy1Down in self.__inputs

    @property
    def Joy1Left(self) -> InputItem: return self.__inputs[InputItem.ItemJoy1Left]
    @property
    def HasJoy1Left(self) -> bool: return InputItem.ItemJoy1Left in self.__inputs

    @property
    def Joy2Up(self) -> InputItem: return self.__inputs[InputItem.ItemJoy2Up]
    @property
    def HasJoy2Up(self) -> bool: return InputItem.ItemJoy2Up in self.__inputs

    @property
    def Joy2Right(self) -> InputItem: return self.__inputs[InputItem.ItemJoy2Right]
    @property
    def HasJoy2Right(self) -> bool: return InputItem.ItemJoy2Right in self.__inputs

    @property
    def Joy2Down(self) -> InputItem: return self.__inputs[InputItem.ItemJoy2Down]
    @property
    def HasJoy2Down(self) -> bool: return InputItem.ItemJoy2Down in self.__inputs

    @property
    def Joy2Left(self) -> InputItem: return self.__inputs[InputItem.ItemJoy2Left]
    @property
    def HasJoy2Left(self) -> bool: return InputItem.ItemJoy2Left in self.__inputs

    @property
    def AvailableInput(self) -> ValuesView[InputItem]: return self.__inputs.values()

    # Unitary access to avoid value injection
    def Input(self, index: int) -> InputItem: return self.__inputs[index]

    def HasInput(self, item: int) -> bool: return item in self.__inputs

    def generateSDLGameDBLine(self):
        oppositeHat = {
             1 :  4,
             2 :  8,
             3 : 12,
             4 :  1,
             5 :  0, # Not possible
             6 :  9,
             7 :  0, # Not possible
             8 :  2,
             9 :  6,
            10 :  0, # Not possible
            11 :  0, # Not possible
            12 :  3,
            13 :  0, # Not possible
            14 :  0, # Not possible
            15 :  0, # Not possible
        }

        result = "{},{},platform:Linux,".format(self.__guid, self.__deviceName.replace(',', ' '))
        for name, inputItem in self.__inputs.items():
            sdlName = inputItem.GameDBName
            sdlPrefix = inputItem.GameDBType
            if sdlName in ('leftx', 'lefty', 'rightx', 'righty') and inputItem.Type in (InputItem.TypeButton, InputItem.TypeHat):
                if inputItem.IsHat:
                    result += "-{}:h{}.{},".format(sdlName, inputItem.Id, inputItem.Value)
                    result += "+{}:h{}.{},".format(sdlName, inputItem.Id, oppositeHat[inputItem.Value])
                else:
                    result += "-{}:{}{},".format(sdlName, sdlPrefix, inputItem.Id)
                    result += "+{}:{}{},".format(sdlName, sdlPrefix, inputItem.Id)
            elif sdlName in ('dpup', 'dpdown', 'dpleft', 'dpright') and inputItem.IsAxis:
                sign = int(inputItem.Value) if sdlName in ('dpup', 'dpleft') else -int(inputItem.Value)
                result += "{}:{}a{},".format(sdlName, '-' if sign < 0 else '+', inputItem.Id)
            elif inputItem.IsHat:
                result += "{}:h{}.{},".format(sdlName, inputItem.Id, inputItem.Value)
            else:
                result += "{}:{}{},".format(sdlName, sdlPrefix, inputItem.Id)

        return result

    def count(self, inputType: int):
        count = 0
        for idx, inp in self.__inputs.items():
            if inp.Type == inputType:
                count += 1
        return count

    def setAxisNumberList(self) -> List[int]:
        # First let's find and sort all those available axes in the controller
        # Remember : a pad can have 6 axes and 1 hat, but only 4 axes are mapped
        # All those tricks are just to mimic https://github.com/xbmc/peripheral.joystick/blob/master/src/api/udev/JoystickUdev.cpp#L321-L334
        axises = []
        for idx, inp in self.__inputs.items():
            if inp.IsAxis:
                # As of now our patched SDL2 gives the same axes number to up/down and left/right. But it's wrong
                leftcode = self.__inputs[InputItem.ItemLeft].Code if InputItem.ItemLeft in self.__inputs else -1
                if inp.Item in (InputItem.ItemUp, InputItem.ItemDown) and inp.Code == leftcode:
                    code = inp.Code + 1
                else:
                    code = inp.Code
                if code not in axises:
                    axises.append(code)
        axises.sort()
        # Complete with existing axes but not in es_input.cfg
        # In the case of a 6 axes pad, complete with the 2 unassigned axes
        for i in range(len(axises), int(self.__axesCount)):
            axises.append(None)

        # Now add hats because they are after all axes
        hats = []
        for idx, inp in self.__inputs.items():
            if inp.IsHat:
                # As of now our patched SDL2 gives the same axes number to up/down and left/right. But it's wrong
                leftcode = self.__inputs[InputItem.ItemLeft].Code if InputItem.ItemLeft in self.__inputs else -1
                if inp.Item in (InputItem.ItemUp, InputItem.ItemDown) and inp.Code == leftcode:
                    code = inp.Code + 1
                else:
                    code = inp.Code
                if code not in hats:
                    hats.append(code)
        hats.sort()
        # Time to sort this
        axises.extend(hats)
        return axises

    def getAxisNumber(self, inputItem: InputItem):
        # ~ print self.axisesNumber
        if inputItem.Type not in (InputItem.TypeAxis, InputItem.TypeHat): return None
        if inputItem.IsHat:
            if inputItem.Item in (InputItem.ItemUp, InputItem.ItemDown) and inputItem.Code == self.__inputs[InputItem.ItemLeft].Code:
                code = inputItem.Code + 1
            else:
                code = inputItem.Code
        else:
            code = inputItem.Code
        return self.axisesNumber.index(code)

    def getTotalAxisNumber(self):
        return int(self.__axesCount) + 2 * int(self.__hatsCount)

    @staticmethod
    def __CompositeIdentifierFrom(configName: str, guid: str, axeCount: int, hatCount: int, buttonCount: int) -> str:
        return "{}-{}-{}:{}:{}".format(configName, guid,
                                       str(axeCount) if axeCount >= 0 else '*',
                                       str(hatCount) if hatCount >= 0 else '*',
                                       str(buttonCount) if buttonCount >= 0 else '*')

    def __CompositeIdentifier(self) -> str:
        return "{}-{}-{}:{}:{}".format(self.__deviceName, self.__guid, self.__axesCount, self.__hatsCount, self.__buttonsCount)

    def __CompositeAbstractIdentifier(self) -> str:
        return "{}-{}-*:*:*".format(self.__deviceName, self.__guid)

    @staticmethod
    def __HasOpposite(item: InputItem) -> bool:
        return item.Item in (InputItem.ItemJoy1Up, InputItem.ItemJoy1Left,
                             InputItem.ItemJoy2Up, InputItem.ItemJoy2Left)

    @staticmethod
    def __CreateOpposite(item: InputItem) -> (int, InputItem):
        opposites: Dict[int, int] = \
        {
            InputItem.ItemJoy1Up  : InputItem.ItemJoy1Down ,
            InputItem.ItemJoy1Left: InputItem.ItemJoy1Right,
            InputItem.ItemJoy2Up  : InputItem.ItemJoy2Down,
            InputItem.ItemJoy2Left: InputItem.ItemJoy2Right,
        }
        if item.Item in opposites:
            if item.Type == InputItem.TypeAxis:
                opposite = item.Copy(opposites[item.Item], -item.Value)
                return opposite.Item, opposite

        raise ValueError

    # Load all controllers from the es_input.cfg
    @staticmethod
    def LoadControllerConfigurationFromFile() -> ControllerCollection:
        controllers: ControllerCollection = {}
        import xml.etree.ElementTree as ET
        tree = ET.parse(esInputs)
        root = tree.getroot()
        for controller in root.findall(".//inputConfig"):
            newController = Controller(deviceName=controller.get("deviceName"), typeName=controller.get("type"),
                                       guid=controller.get("deviceGUID"), playerIndex=0, sdlIndex=0, inputs=None,
                                       devicePath="", axesCount=int(controller.get("deviceNbAxes")),
                                       hatsCount=int(controller.get("deviceNbHats")),
                                       buttonCount=int(controller.get("deviceNbButtons")))
            controllers[newController.__CompositeIdentifier()] = newController
            controllers[newController.__CompositeAbstractIdentifier()] = newController
            for inp in controller.findall("input"):

                # Read input item attributes
                itemName  = inp.get("name" , None)
                itemType  = inp.get("type" , None)
                itemId    = inp.get("id"   , None)
                itemValue = inp.get("value", None)
                itemCode  = inp.get("code" , None)
                if itemName  is None: print("[Controller] Missing name in pad definition");  continue;
                if itemType  is None: print("[Controller] Missing type in pad definition");  continue;
                if itemId    is None: print("[Controller] Missing id in pad definition");    continue;
                if itemValue is None: print("[Controller] Missing value in pad definition"); continue;
                if itemCode  is None: print("[Controller] Missing code in pad definition");  continue;

                # Store input item
                inputItem = InputItem(itemName, itemType, int(itemId), int(itemValue), int(itemCode))
                newController.__inputs[inputItem.Item] = inputItem

                # Create opposite?
                if Controller.__HasOpposite(inputItem):
                    (oppositeItem, itemOpposite) = Controller.__CreateOpposite(inputItem)
                    newController.__inputs[oppositeItem] = itemOpposite

        return controllers

    @staticmethod
    def __LoadStrFromKwargs(key: str, defaultvalue: str, **kwargs) -> str:
        value: Optional[str] = kwargs.get(key, None)
        return value if value is not None else defaultvalue

    @staticmethod
    def __LoadIntFromKwargs(key: str, defaultvalue: int, **kwargs) -> int:
        value: Optional[str] = kwargs.get(key, None)
        return int(value) if value is not None else defaultvalue

    @staticmethod
    def __StoreNaturalIndexes(controllers: ControllerPerPlayer):
        orderedControllers: ControllerCollection = {}
        for controller in controllers.values():
            orderedControllers[controller.DevicePath] = controller
        i = 0
        for key in sorted(orderedControllers):
            controller: Controller = orderedControllers[key]
            controller.__naturalIndex = i
            i += 1

    @staticmethod
    def LoadDemoControllerConfigurations(**kwargs) -> Dict[int, int]:
        startPerPlayer: Dict[int, int] = {}

        controllers: ControllerCollection = Controller.LoadControllerConfigurationFromFile()
        for i in range(1, 11):
            nbbuttons: int = Controller.__LoadIntFromKwargs('p{}nbbuttons'.format(i), -1, **kwargs)
            nbhats: int    = Controller.__LoadIntFromKwargs('p{}nbhats'.format(i), -1, **kwargs)
            nbaxes: int    = Controller.__LoadIntFromKwargs('p{}nbaxes'.format(i), -1, **kwargs)
            dev: str       = Controller.__LoadStrFromKwargs('p{}devicepath'.format(i), "missing-device", **kwargs)
            guid: str      = Controller.__LoadStrFromKwargs('p{}guid'.format(i), "missing-guid", **kwargs)
            name: str      = Controller.__LoadStrFromKwargs('p{}name'.format(i), "missong-name", **kwargs)
            index: int     = Controller.__LoadIntFromKwargs('p{}index'.format(i), -1, **kwargs)

            newController = Controller.__FindBestControllerConfig(controllers, i, guid, index, name, dev, nbaxes, nbhats, nbbuttons)
            if newController:
                startPerPlayer[i] = newController.__inputs[InputItem.ItemStart].Id

        return startPerPlayer

    @staticmethod
    def LoadUserControllerConfigurations(**kwargs) -> ControllerPerPlayer:
        playerControllers: ControllerPerPlayer = {}

        controllers: ControllerCollection = Controller.LoadControllerConfigurationFromFile()
        for i in range(1, 11):
            index: int     = Controller.__LoadIntFromKwargs('p{}index'.format(i), -1, **kwargs)
            name: str      = Controller.__LoadStrFromKwargs('p{}name'.format(i), "missong-name", **kwargs)
            guid: str      = Controller.__LoadStrFromKwargs('p{}guid'.format(i), "missing-guid", **kwargs)
            dev: str       = Controller.__LoadStrFromKwargs('p{}devicepath'.format(i), "missing-device", **kwargs)
            nbaxes: int    = Controller.__LoadIntFromKwargs('p{}nbaxes'.format(i), -1, **kwargs)
            nbhats: int    = Controller.__LoadIntFromKwargs('p{}nbhats'.format(i), -1, **kwargs)
            nbbuttons: int = Controller.__LoadIntFromKwargs('p{}nbbuttons'.format(i), -1, **kwargs)

            newController = Controller.__FindBestControllerConfig(controllers, i, guid, index, name, dev, nbaxes, nbhats, nbbuttons)
            if newController:
                playerControllers[i] = newController

        Controller.__StoreNaturalIndexes(playerControllers)

        return playerControllers

    @staticmethod
    def __FindBestControllerConfig(controllers: ControllerCollection, playerIndex: int, guid: str, sdlIndex, deviceName: str,
                                   devicePath: str, axesCount: int, hatsCount: int, buttonsCount: int) -> Optional[Controller]:
        compositeId = Controller.__CompositeIdentifierFrom(deviceName, guid, axesCount, hatsCount, buttonsCount)
        if compositeId in controllers:
            controller = controllers[compositeId]
            return Controller(deviceName=deviceName, typeName=controller.Type, guid=guid,
                              playerIndex=playerIndex, sdlIndex=sdlIndex, inputs=controller.__inputs,
                              devicePath=devicePath, axesCount=axesCount, hatsCount=hatsCount,
                              buttonCount=buttonsCount)
        return None

    @staticmethod
    def GenerateSDLGameDatabase(controllers, outputFile ="/tmp/gamecontrollerdb.txt"):
        finalData = ["# Recalbox controller list"]
        for idx, controller in controllers.items(): finalData.append(controller.generateSDLGameDBLine())
        with open(outputFile, "w") as text_file: text_file.write("\n".join(finalData))
        return outputFile

# Type def for convenience
ControllerCollection = Dict[str, Controller]
ControllerPerPlayer = Dict[int, Controller]
