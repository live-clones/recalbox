import recalboxFiles
from settings.keyValueSettings import keyValueSettings


class LibretroCores:

    def __init__(self, system, settings, controllers):
        self.system = system
        self.settings = settings
        self.controllers = controllers

    #
    # Core specific configurations
    #

    @staticmethod
    def configureBlueMSX(coreSettings):
        # Default core configuration - Actual value for legacy msx folder
        coreSettings.setOption("bluemsx_msxtype", '"Auto"')

    @staticmethod
    def configureMAME2003plus(coreSettings):
        coreSettings.setOption("mame2003-plus_analog", '"digital"')

    @staticmethod
    def configureOpera(coreSettings):
        coreSettings.setOption("opera_dsp_threaded", '"enabled"')

    @staticmethod
    def configurePARALLELN64(coreSettings):
        coreSettings.setOption("parallel-n64-boot-device", '"Default"')
        coreSettings.setOption("parallel-n64-64dd-hardware", '"disabled"')
        coreSettings.setOption("parallel-n64-gfxplugin", '"auto"')

    @staticmethod
    # (pc98) force the "joymode" option to have automatic joystick support.
    def configureNPKAI(coreSettings):
        coreSettings.setOption("np2kai_joymode", '"Keypad"')

    #
    # System specific configurations
    #

    @staticmethod
    def configureAmigaCDTV(coreSettings):
        coreSettings.setOption("puae_model", '"CDTV"')

    @staticmethod
    def configureAmigaCD32(coreSettings):
        coreSettings.setOption("puae_model", '"CD32"')

    @staticmethod
    def configureAmiga1200(coreSettings):
        coreSettings.setOption("puae_model", '"A1200"')

    @staticmethod
    def configureAmiga600(coreSettings):
        coreSettings.setOption("puae_model", '"A500PLUS"')

    @staticmethod
    def configureAmstradCPC(coreSettings):
        coreSettings.setOption("cap32_model", '"6128"')

    @staticmethod
    def configureAmstradGX4000(coreSettings):
        coreSettings.setOption("cap32_model", '"6128+"')

    @staticmethod
    def configureAtari5200(coreSettings):
        coreSettings.setOption("atari800_system", '"5200"')

        a800settings = keyValueSettings(recalboxFiles.atari800CustomConfig)
        a800settings.loadFile(True)
        a800settings.setOption("MACHINE_TYPE", "Atari 5200")
        a800settings.setOption("RAM_SIZE", "16")
        a800settings.setOption("STEREO_POKEY", "0")
        a800settings.setOption("BUILTIN_BASIC", "0")
        a800settings.saveFile()

    @staticmethod
    def configureAtari800(coreSettings):
        coreSettings.setOption("atari800_system", '"130XE (128K)"')

        a800settings = keyValueSettings(recalboxFiles.atari800CustomConfig)
        a800settings.loadFile(True)
        a800settings.setOption("MACHINE_TYPE", "Atari XL/XE")
        a800settings.setOption("RAM_SIZE", "64")
        a800settings.setOption("STEREO_POKEY", "1")
        a800settings.setOption("BUILTIN_BASIC", "1")
        a800settings.saveFile()

    @staticmethod
    def configure64DD(coreSettings):
        coreSettings.setOption("parallel-n64-boot-device", '"64DD IPL"')
        coreSettings.setOption("parallel-n64-64dd-hardware", '"enabled"')
        coreSettings.setOption("parallel-n64-gfxplugin", '"gln64"')

    @staticmethod
    def configureSpectravideo(coreSettings):
        coreSettings.setOption("bluemsx_msxtype", '"SVI - Spectravideo SVI-328 MK2"')

    @staticmethod
    def configureMsx1(coreSettings):
        coreSettings.setOption("bluemsx_msxtype", '"MSX"')

    @staticmethod
    def configureMsx2(coreSettings):
        coreSettings.setOption("bluemsx_msxtype", '"MSX2+"')

    @staticmethod
    def configureMsxTurboR(coreSettings):
        coreSettings.setOption("bluemsx_msxtype", '"MSXturboR"')

    @staticmethod
    def configureOdyssey2(coreSettings):
        coreSettings.setOption("o2em_bios", '"o2rom.bin"')

    @staticmethod
    def configureVideoPacPlus(coreSettings):
        coreSettings.setOption("o2em_bios", '"g7400.bin"')

    # Fill cores configuration
    def fillCoresConfiguration(self):
        recalbox = self.system.config
        settings = self.settings

        # Specific configuration handlers per core
        specificCoreHandlers =\
        {
            "bluemsx" : LibretroCores.configureBlueMSX,
            "mame2003_plus": LibretroCores.configureMAME2003plus,
            "opera": LibretroCores.configureOpera,
            "parallel_n64": LibretroCores.configurePARALLELN64,
            "np2kai" : LibretroCores.configureNPKAI,
        }

        # Get handler and execute
        core = recalbox["core"]
        if core in specificCoreHandlers:
            function = specificCoreHandlers.get(core)
            function(settings)

        # Specific configuration handlers per system
        specificSystemHandlers =\
        {
            "amiga600"     : LibretroCores.configureAmiga600,
            "amiga1200"    : LibretroCores.configureAmiga1200,
            "amigacdtv"    : LibretroCores.configureAmigaCDTV,
            "amigacd32"    : LibretroCores.configureAmigaCD32,
            "atari5200"    : LibretroCores.configureAtari5200,
            "atari800"     : LibretroCores.configureAtari800,
            "amstradcpc"   : LibretroCores.configureAmstradCPC,
            "gx4000"       : LibretroCores.configureAmstradGX4000,
            "spectravideo" : LibretroCores.configureSpectravideo,
            "64dd"         : LibretroCores.configure64DD,
            "msx1"         : LibretroCores.configureMsx1,
            "msx2"         : LibretroCores.configureMsx2,
            "msxturbor"    : LibretroCores.configureMsxTurboR,
            "pc98"         : LibretroCores.configureNPKAI,
            "odyssey2"     : LibretroCores.configureOdyssey2,
            "videopacplus" : LibretroCores.configureVideoPacPlus,
        }

        # Get handler and execute
        if self.system.name in specificSystemHandlers:
            function = specificSystemHandlers.get(self.system.name)
            function(settings)
