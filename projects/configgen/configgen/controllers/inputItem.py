from __future__ import annotations
from typing import Dict


class InputItemMeta(type):

    # Types
    __TYPE_AXIS   = 0
    __TYPE_HAT    = 1
    __TYPE_BUTTON = 2
    __TYPE_KEY    = 3

    # Converters

    __TYPE_TO_NAME = \
    {
        __TYPE_AXIS  : "axis",
        __TYPE_HAT   : "hat",
        __TYPE_BUTTON: "button",
        __TYPE_KEY   : "key",
    }

    @property
    def ConverterTypeToName(cls) -> Dict[int, str]: return cls.__TYPE_TO_NAME

    __NAME_TO_TYPE = \
    {
        "axis"  : __TYPE_AXIS,
        "hat"   : __TYPE_HAT,
        "button": __TYPE_BUTTON,
        "key"   : __TYPE_KEY,
    }

    @property
    def ConverterNameToType(cls) -> Dict[str, int]: return cls.__NAME_TO_TYPE

    # Type read-only access

    @property
    def TypeAxis(cls): return cls.__TYPE_AXIS

    @property
    def TypeHat(cls): return cls.__TYPE_HAT

    @property
    def TypeButton(cls): return cls.__TYPE_BUTTON

    @property
    def TypeKey(cls): return cls.__TYPE_KEY

    # Items
    __ITEM_FIRST      = 0
    __ITEM_A          = 0
    __ITEM_B          = 1
    __ITEM_X          = 2
    __ITEM_Y          = 3
    __ITEM_START      = 4
    __ITEM_SELECT     = 5
    __ITEM_HOTKEY     = 6
    __ITEM_L1         = 7
    __ITEM_R1         = 8
    __ITEM_L2         = 9
    __ITEM_R2         = 10
    __ITEM_L3         = 11
    __ITEM_R3         = 12
    __ITEM_DPAD_UP    = 13
    __ITEM_DPAD_DOWN  = 14
    __ITEM_DPAD_LEFT  = 15
    __ITEM_DPAD_RIGHT = 16
    __ITEM_JOY1_UP    = 17
    __ITEM_JOY1_LEFT  = 18
    __ITEM_JOY1_DOWN  = 19 # Virtual item
    __ITEM_JOY1_RIGHT = 20 # Virtual item
    __ITEM_JOY2_UP    = 21
    __ITEM_JOY2_LEFT  = 22
    __ITEM_JOY2_DOWN  = 23 # Virtual item
    __ITEM_JOY2_RIGHT = 24 # Virtual item
    __ITEM_LUMUP      = 25
    __ITEM_LUMDOWN    = 26
    __ITEM_VOLUP      = 27
    __ITEM_VOLDOWN    = 28
    __ITEM_LAST       = __ITEM_VOLDOWN

    # Item readonly access

    @property
    def ItemA(cls) -> int: return cls.__ITEM_A;

    @property
    def ItemB(cls) -> int: return cls.__ITEM_B;

    @property
    def ItemX(cls) -> int: return cls.__ITEM_X;

    @property
    def ItemY(cls) -> int: return cls.__ITEM_Y;

    @property
    def ItemStart(cls) -> int: return cls.__ITEM_START;

    @property
    def ItemSelect(cls) -> int: return cls.__ITEM_SELECT;

    @property
    def ItemHotkey(cls) -> int: return cls.__ITEM_HOTKEY;

    @property
    def ItemL1(cls) -> int: return cls.__ITEM_L1;

    @property
    def ItemR1(cls) -> int: return cls.__ITEM_R1;

    @property
    def ItemL2(cls) -> int: return cls.__ITEM_L2;

    @property
    def ItemR2(cls) -> int: return cls.__ITEM_R2;

    @property
    def ItemL3(cls) -> int: return cls.__ITEM_L3;

    @property
    def ItemR3(cls) -> int: return cls.__ITEM_R3;

    @property
    def ItemUp(cls) -> int: return cls.__ITEM_DPAD_UP;

    @property
    def ItemLeft(cls) -> int: return cls.__ITEM_DPAD_LEFT;

    @property
    def ItemDown(cls) -> int: return cls.__ITEM_DPAD_DOWN;

    @property
    def ItemRight(cls) -> int: return cls.__ITEM_DPAD_RIGHT;

    @property
    def ItemJoy1Up(cls) -> int: return cls.__ITEM_JOY1_UP;

    @property
    def ItemJoy1Left(cls) -> int: return cls.__ITEM_JOY1_LEFT;

    @property
    def ItemJoy1Down(cls) -> int: return cls.__ITEM_JOY1_DOWN;

    @property
    def ItemJoy1Right(cls) -> int: return cls.__ITEM_JOY1_RIGHT;

    @property
    def ItemJoy2Up(cls) -> int: return cls.__ITEM_JOY2_UP;

    @property
    def ItemJoy2Left(cls) -> int: return cls.__ITEM_JOY2_LEFT;

    @property
    def ItemJoy2Down(cls) -> int: return cls.__ITEM_JOY2_DOWN;

    @property
    def ItemJoy2Right(cls) -> int: return cls.__ITEM_JOY2_RIGHT;

    @property
    def ItemFirst(cls) -> int: return cls.__ITEM_FIRST;

    @property
    def ItemLast(cls) -> int: return cls.__ITEM_LAST;

    # Converter
    __NAME_TO_ITEM: Dict[str, int] = \
    {
        'a'             : __ITEM_A,
        'b'             : __ITEM_B,
        'x'             : __ITEM_X,
        'y'             : __ITEM_Y,
        'start'         : __ITEM_START,
        'select'        : __ITEM_SELECT,
        'hotkey'        : __ITEM_HOTKEY,
        'l1'            : __ITEM_L1,
        'r1'            : __ITEM_R1,
        'pageup'        : __ITEM_L1, # Old name
        'pagedown'      : __ITEM_R1, # Old name
        'l2'            : __ITEM_L2,
        'r2'            : __ITEM_R2,
        'l3'            : __ITEM_L3,
        'r3'            : __ITEM_R3,
        'up'            : __ITEM_DPAD_UP,
        'down'          : __ITEM_DPAD_DOWN,
        'left'          : __ITEM_DPAD_LEFT,
        'right'         : __ITEM_DPAD_RIGHT,
        'joystick1up'   : __ITEM_JOY1_UP,
        'joystick1left' : __ITEM_JOY1_LEFT,
        'joystick1down' : __ITEM_JOY1_DOWN,
        'joystick1right': __ITEM_JOY1_RIGHT,
        'joystick2up'   : __ITEM_JOY2_UP,
        'joystick2left' : __ITEM_JOY2_LEFT,
        'joystick2down' : __ITEM_JOY2_DOWN,
        'joystick2right': __ITEM_JOY2_RIGHT,
        'lum+'          : __ITEM_LUMUP,
        'lum-'          : __ITEM_LUMDOWN,
        'vol+'          : __ITEM_VOLUP,
        'vol-'          : __ITEM_VOLDOWN,
    }

    @property
    def ConverterNameToItem(cls) -> Dict[str, int]: return cls.__NAME_TO_ITEM;

    __ITEM_TO_NAME: Dict[int, str] = \
    {
        __ITEM_A         : 'a',
        __ITEM_B         : 'b',
        __ITEM_X         : 'x',
        __ITEM_Y         : 'y',
        __ITEM_START     : 'start',
        __ITEM_SELECT    : 'select',
        __ITEM_HOTKEY    : 'hotkey',
        __ITEM_L1        : 'l1',
        __ITEM_R1        : 'r1',
        __ITEM_L2        : 'l2',
        __ITEM_R2        : 'r2',
        __ITEM_L3        : 'l3',
        __ITEM_R3        : 'r3',
        __ITEM_DPAD_UP   : 'up',
        __ITEM_DPAD_DOWN : 'down',
        __ITEM_DPAD_LEFT : 'left',
        __ITEM_DPAD_RIGHT: 'right',
        __ITEM_JOY1_UP   : 'joystick1up',
        __ITEM_JOY1_LEFT : 'joystick1left',
        __ITEM_JOY1_DOWN : 'joystick1down',
        __ITEM_JOY1_RIGHT: 'joystick1right',
        __ITEM_JOY2_UP   : 'joystick2up',
        __ITEM_JOY2_LEFT : 'joystick2left',
        __ITEM_JOY2_DOWN : 'joystick2down',
        __ITEM_JOY2_RIGHT: 'joystick2right',
        __ITEM_LUMUP     : 'lum+',
        __ITEM_LUMDOWN   : 'lum-',
        __ITEM_VOLUP     : 'vol+',
        __ITEM_VOLDOWN   : 'vol-',
    }

    @property
    def ConverterItemToName(cls) -> Dict[int, str]: return cls.__ITEM_TO_NAME;

    __ITEM_TO_GAMEDB_NAME: Dict[int, str] = \
     {
        __ITEM_A         : 'b',
        __ITEM_B         : 'a',
        __ITEM_X         : 'y',
        __ITEM_Y         : 'x',
        __ITEM_START     : 'start',
        __ITEM_SELECT    : 'back',
        __ITEM_HOTKEY    : 'guide',
        __ITEM_L1        : 'leftshoulder',
        __ITEM_R1        : 'rightshoulder',
        __ITEM_L2        : 'lefttrigger',
        __ITEM_R2        : 'righttrigger',
        __ITEM_L3        : 'leftstick',
        __ITEM_R3        : 'rightstick',
        __ITEM_DPAD_UP   : 'dpup',
        __ITEM_DPAD_DOWN : 'dpdown',
        __ITEM_DPAD_LEFT : 'dpleft',
        __ITEM_DPAD_RIGHT: 'dpright',
        __ITEM_JOY1_UP   : '-lefty',
        __ITEM_JOY1_LEFT : '-leftx',
        __ITEM_JOY1_DOWN : '+lefty',
        __ITEM_JOY1_RIGHT: '+leftx',
        __ITEM_JOY2_UP   : '-righty',
        __ITEM_JOY2_LEFT : '-rightx',
        __ITEM_JOY2_DOWN : '+righty',
        __ITEM_JOY2_RIGHT: '+rightx',
        __ITEM_LUMUP     : '',
        __ITEM_LUMDOWN   : '',
        __ITEM_VOLUP     : '',
        __ITEM_VOLDOWN   : '',
     }

    @property
    def ConverterItemToGameDBName(cls) -> Dict[int, str]: return cls.__ITEM_TO_GAMEDB_NAME;


class InputItem(metaclass=InputItemMeta):

    def __init__(self, name: str, typeName: str, identifier: int, value: int, code: int):
        self.__item: int = self.ConvertItem(name)
        self.__type: int = self.ConvertType(typeName)
        self.__id: int = identifier
        self.__value: int = value
        self.__code: int = code

    @staticmethod
    def ConvertType(typeName: str) -> int:
        if typeName in InputItem.ConverterNameToType: return InputItem.ConverterNameToType[typeName]
        print("[InputItem] Unknown type " + typeName)
        raise ValueError

    @staticmethod
    def ConvertItem(itemName: str) -> int:
        if itemName in InputItem.ConverterNameToItem: return InputItem.ConverterNameToItem[itemName]
        print("[InputItem] Unknown item " + itemName)
        raise ValueError

    # Quick type-identifier properties

    @property
    def IsAxis(self) -> bool: return self.__type == InputItem.TypeAxis

    @property
    def IsHat(self) -> bool: return self.__type == InputItem.TypeHat

    @property
    def IsButton(self) -> bool: return self.__type == InputItem.TypeButton

    @property
    def IsKey(self) -> bool: return self.__type == InputItem.TypeKey

    # Human types

    @property
    def IsDPad(self) -> bool: return self.__item in (InputItem.ItemUp  , InputItem.ItemDown,
                                                     InputItem.ItemLeft, InputItem.ItemRight)

    @property
    def IsAnalogJoystick(self) -> bool: return self.__item in (InputItem.ItemJoy1Up  , InputItem.ItemJoy1Down,
                                                               InputItem.ItemJoy1Left, InputItem.ItemJoy1Right,
                                                               InputItem.ItemJoy2Up  , InputItem.ItemJoy2Down,
                                                               InputItem.ItemJoy2Left, InputItem.ItemJoy2Right)

    # Regular getters

    @property
    def Item(self) -> int: return self.__item

    @property
    def Name(self) -> str: return InputItem.ConverterItemToName[self.__item]

    @property
    def Type(self) -> int: return self.__type

    @property
    def TypeName(self) -> str: return InputItem.ConverterTypeToName[self.__type]

    @property
    def Id(self) -> int: return self.__id

    @property
    def Value(self) -> int: return self.__value

    @property
    def Code(self) -> int: return self.__code

    # Identification getters

    @property
    def IsA(self) -> bool: return self.__item == InputItem.ItemA

    @property
    def IsB(self) -> bool: return self.__item == InputItem.ItemB

    @property
    def IsX(self) -> bool: return self.__item == InputItem.ItemX

    @property
    def IsY(self) -> bool: return self.__item == InputItem.ItemY

    @property
    def IsL1(self) -> bool: return self.__item == InputItem.ItemL1

    @property
    def IsR1(self) -> bool: return self.__item == InputItem.ItemR1

    @property
    def IsL2(self) -> bool: return self.__item == InputItem.ItemL2

    @property
    def IsR2(self) -> bool: return self.__item == InputItem.ItemR2

    @property
    def IsL3(self) -> bool: return self.__item == InputItem.ItemL3

    @property
    def IsR3(self) -> bool: return self.__item == InputItem.ItemR3

    @property
    def IsHotkey(self) -> bool: return self.__item == InputItem.ItemHotkey

    @property
    def IsStart(self) -> bool: return self.__item == InputItem.ItemStart

    @property
    def IsSelect(self) -> bool: return self.__item == InputItem.ItemSelect

    @property
    def IsUp(self) -> bool: return self.__item == InputItem.ItemUp

    @property
    def IsRight(self) -> bool: return self.__item == InputItem.ItemRight

    @property
    def IsDown(self) -> bool: return self.__item == InputItem.ItemDown

    @property
    def IsLeft(self) -> bool: return self.__item == InputItem.ItemLeft

    @property
    def IsJoy1Up(self) -> bool: return self.__item == InputItem.ItemJoy1Up

    @property
    def IsJoy1Right(self) -> bool: return self.__item == InputItem.ItemJoy1Right

    @property
    def IsJoy1Down(self) -> bool: return self.__item == InputItem.ItemJoy1Down

    @property
    def IsJoy1Left(self) -> bool: return self.__item == InputItem.ItemJoy1Left

    @property
    def IsJoy2Up(self) -> bool: return self.__item == InputItem.ItemJoy2Up

    @property
    def IsJoy2Right(self) -> bool: return self.__item == InputItem.ItemJoy2Right

    @property
    def IsJoy2Down(self) -> bool: return self.__item == InputItem.ItemJoy2Down

    @property
    def IsJoy2Left(self) -> bool: return self.__item == InputItem.ItemJoy2Left

    # Extra properties

    @staticmethod
    def IsValidItem(item: int) -> bool: return InputItem.ItemFirst < item < InputItem.ItemLast

    @property
    def GameDBType(self) -> str:
        if self.__type == InputItem.TypeAxis: return 'a'
        if self.__type == InputItem.TypeHat: return 'h'
        if self.__type == InputItem.TypeButton: return 'b'
        return '?'

    @property
    def GameDBName(self) -> str: return InputItem.ConverterItemToGameDBName[self.__item]

    # Copy

    def Copy(self, newItem: int, newValue):
        return InputItem(InputItem.ConverterItemToName[newItem],
                         InputItem.ConverterTypeToName[self.__type],
                         self.__id, newValue, self.__code)

    def clone(self, newName: str, newValue: int):
        result = InputItem(newName, "clone", self.__id, newValue, self.__code)
        result.__type = self.__type

InputCollection = Dict[int, InputItem]