import recalboxFiles
from settings.keyValueSettings import keyValueSettings


class LibretroCores:

    def __init__(self, system, settings, controllers):
        self.system = system
        self.settings = settings
        self.controllers = controllers

    @staticmethod
    def configureBlueMSX(coreSettings):
        coreSettings.setOption("bluemsx_msxtype", '"Auto"')

    @staticmethod
    def configureAtari5200(coreSettings):
        coreSettings.setOption("atari800_system", '"5200"')

        a800settings = keyValueSettings(recalboxFiles.atari800CustomConfig)
        a800settings.loadFile(True)
        a800settings.setOption("MACHINE_TYPE", "Atari 520")
        a800settings.setOption("RAM_SIZE", "16")
        a800settings.setOption("STEREO_POKEY", "0")
        a800settings.setOption("BUILTIN_BASIC", "0")
        a800settings.saveFile()

    @staticmethod
    def configureAtari800(coreSettings):
        coreSettings.setOption("atari800_system", '"800XL (64K)"')

        a800settings = keyValueSettings(recalboxFiles.atari800CustomConfig)
        a800settings.loadFile(True)
        a800settings.setOption("MACHINE_TYPE", "Atari XL/XE")
        a800settings.setOption("RAM_SIZE", "64")
        a800settings.setOption("STEREO_POKEY", "1")
        a800settings.setOption("BUILTIN_BASIC", "1")
        a800settings.saveFile()

    # Fill cores configuration
    def fillCoresConfiguration(self):
        recalbox = self.system.config
        settings = self.settings

        # Specific configuration handlers per system
        specificSystemHandlers =\
        {
            "atari5200" : LibretroCores.configureAtari5200,
            "atari800" : LibretroCores.configureAtari800
        }

        # Get handler and execute
        if self.system.name in specificSystemHandlers:
            function = specificSystemHandlers.get(self.system.name)
            function(settings)

        # Specific configuration handlers per core
        specificCoreHandlers =\
        {
            "bluemsx" : LibretroCores.configureBlueMSX,
        }

        # Get handler and execute
        core = recalbox["core"]
        if core in specificCoreHandlers:
            function = specificCoreHandlers.get(core)
            function(settings)

