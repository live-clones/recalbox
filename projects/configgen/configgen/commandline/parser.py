from __future__ import annotations
from typing import Dict, List, Any

from configgen.commandline.command import ParserCommand

# For those who're wondering why we've written a custom parser, that's pretty simple:
# The regular ArgumentParser is slow as hell.
# It takes 200ms on a Raspberry pi4 for just a few arguments.
class Parser:

    class Namespace:

        def __init__(self, args: Dict[str, Any]):
            self.__args: Dict[str, Any] = dict(args)

        def __getattr__(self, item: str):
            return self.__args[item]

        def __setattr__(self, key, value):
            if key != "_Namespace__args":
                raise PermissionError
            for k, v in value.items():
              self.__dict__[k.strip('-')] = v

        def __delattr__(self, item):
            raise PermissionError

        def Kwargs(self):
            return self.__args


    def __init__(self):
        self.__commands: Dict[str, ParserCommand] = {}
        self.__processed: Dict[str, ParserCommand] = {}

    def AddString(self, command: str, helpmsg: str, required: bool = True, default: str = "") -> Parser:
        self.__commands[command] = ParserCommand(command, helpmsg, ParserCommand.Str, True, required).SetDefaultStr(default)
        return self

    def AddInt(self, command: str, helpmsg: str, required: bool = True, default: int = 0) -> Parser:
        self.__commands[command] = ParserCommand(command, helpmsg, ParserCommand.Int, True, required).SetDefaultInt(default)
        return self

    def AddBool(self, command: str, helpmsg: str, required: bool = True, default: bool = False) -> Parser:
        self.__commands[command] = ParserCommand(command, helpmsg, ParserCommand.Bool, True, required).SetDefaultBool(default)
        return self

    def AddSimple(self, command: str, helpmsg: str, required: bool = True) -> Parser:
        self.__commands[command] = ParserCommand(command, helpmsg, ParserCommand.Bool, False, required)
        return self

    def Parse(self) -> Namespace:
        import sys
        args: List[str] = sys.argv  # raw argument list
        result: Dict[str, Any] = {} # result dictionnary

        # Parse
        index: int = 1
        while index < len(args):
            # Get new command
            cmd: str = args[index]
            index += 1
            if not cmd in self.__commands:
                if cmd in self.__processed: self.__Error("Command '{}' found twice.".format(cmd))
                else:                       self.__Error("Unknown command '{}'".format(cmd))
            command: ParserCommand = self.__commands[cmd]

            # Process command
            if command.HasParameter:
                if index >= len(args): self.__Error("Missing parameter for argument '{}'".format(cmd))
                if command.Type == ParserCommand.Str:  result[cmd] = args[index]
                if command.Type == ParserCommand.Int:  result[cmd] = int(args[index])
                if command.Type == ParserCommand.Bool: result[cmd] = args[index] == '1'
                index += 1
            else:
                result[cmd] = True

            # Remove processed command
            self.__processed[cmd] = command
            del self.__commands[cmd]

        # Check all required args are there and set default parameters
        for command in self.__commands.values():
            if command.Required:
                self.__Error("Missing required argument " + command.Command)
            else:
                cmd: str = command.Command
                if command.Type == ParserCommand.Str:  result[cmd] = command.DefaultAsStr
                if command.Type == ParserCommand.Int:  result[cmd] = command.DefaultAsInt
                if command.Type == ParserCommand.Bool: result[cmd] = command.DefaultAsBool

        namespace: Parser.Namespace = Parser.Namespace(result)
        return namespace

    def __Error(self, error: str):
        print("Error: " +error)
        self.__Help()
        exit(-1)

    @staticmethod
    def __FormatArgType(command: ParserCommand):
        if command.Type == ParserCommand.Str: return "<string>"
        if command.Type == ParserCommand.Int: return "<int>"
        if command.Type == ParserCommand.Bool: return "<0 or 1>"
        raise ValueError

    def __Help(self):
        print("Usage:")

        # Required first
        print("")
        print("  Required arguments:")
        for command in self.__commands.values():
            if command.Required:
                print(f"    {command.Command:30} {self.__FormatArgType(command):10} : {command.Help}")

        # Required first
        print("")
        print("  Optional arguments:")
        for command in self.__commands.values():
            if not command.Required:
                print(f"    {command.Command:30} {self.__FormatArgType(command):10} : {command.Help}")
