from abc import ABCMeta, abstractmethod

from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings

class Generator(object):
    __metaclass__ = ABCMeta
    @abstractmethod
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        pass
    
    #@abstractmethod
    def config_upgrade(self, version: str):
        print("{} does not support configuration upgrade yet".format(self.__class__.__name__))
        return False
