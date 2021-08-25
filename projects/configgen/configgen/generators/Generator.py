from abc import ABCMeta, abstractmethod
from typing import Dict

from Emulator import Emulator
from controllersConfig import Controller
from settings.keyValueSettings import keyValueSettings

# Type def for convenience
ControllerDictionary = Dict[str, Controller]

class Generator(object):
    __metaclass__ = ABCMeta
    @abstractmethod
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        pass
    
    #@abstractmethod
    def config_upgrade(self, version):
        print("{} does not support configuration upgrade yet".format(self.__class__.__name__))
        return False
