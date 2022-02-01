from __future__ import annotations

class CommandMeta(type):

    __TYPE_STR = 0
    __TYPE_INT = 1
    __TYPE_BOOL = 2

    @property
    def Str(cls) -> int: return cls.__TYPE_STR

    @property
    def Int(cls) -> int: return cls.__TYPE_INT

    @property
    def Bool(cls) -> int: return cls.__TYPE_BOOL


class ParserCommand(metaclass=CommandMeta):

    def __init__(self, command: str, chelp: str, ctype: int, hasparam: bool, required: bool):
        self.__command: str = command
        self.__help: str = chelp
        self.__type: int = ctype
        self.__hasparam: bool = hasparam
        self.__required: bool = required
        self.__defaultStr: str = ""
        self.__defaultInt: int = 0
        self.__defaultBool: bool = False

    def SetDefaultStr(self, default: str) -> ParserCommand:
        self.__defaultStr = default
        return self

    def SetDefaultInt(self, default: int) -> ParserCommand:
        self.__defaultInt = default
        return self

    def SetDefaultBool(self, default: bool) -> ParserCommand:
        self.__defaultBool = default
        return self

    @property
    def Command(self) -> str: return self.__command

    @property
    def Help(self) -> str: return self.__help

    @property
    def Type(self) -> int: return self.__type

    @property
    def HasParameter(self) -> bool: return self.__hasparam

    @property
    def Required(self) -> bool: return self.__required

    @property
    def DefaultAsStr(self) -> str: return self.__defaultStr

    @property
    def DefaultAsInt(self) -> int: return self.__defaultInt

    @property
    def DefaultAsBool(self) -> bool: return self.__defaultBool