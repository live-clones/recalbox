from configgen.Emulator import Emulator
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.controllers.controller import ControllerPerPlayer


class LibretroCores:

    def __init__(self, system: Emulator, settings: keyValueSettings, controllers: ControllerPerPlayer, rom: str):
        self.system: Emulator = system
        self.settings: keyValueSettings = settings
        self.controllers: ControllerPerPlayer = controllers
        self.rom = rom

    #
    # Core specific configurations
    #

    @staticmethod
    def configureBlueMSX(coreSettings: keyValueSettings):
        # Default core configuration - Actual value for legacy msx folder
        coreSettings.setString("bluemsx_msxtype", '"Auto"')

    @staticmethod
    def configureMAME2003plus(coreSettings: keyValueSettings):
        coreSettings.setString("mame2003-plus_analog", '"digital"')

    @staticmethod
    def configureOpera(coreSettings: keyValueSettings):
        coreSettings.setString("opera_dsp_threaded", '"enabled"')

    @staticmethod
    def configurePARALLELN64(coreSettings: keyValueSettings):
        coreSettings.setString("parallel-n64-boot-device", '"Default"')
        coreSettings.setString("parallel-n64-64dd-hardware", '"disabled"')
        coreSettings.setString("parallel-n64-gfxplugin", '"auto"')

    @staticmethod
    # (pc98) force the "joymode" option to have automatic joystick support.
    def configureNPKAI(coreSettings: keyValueSettings):
        coreSettings.setString("np2kai_joymode", '"Keypad"')

    #
    # System specific configurations
    #

    @staticmethod
    def configureAmigaCDTV(coreSettings: keyValueSettings):
        coreSettings.setString("puae_model", '"CDTV"')

    @staticmethod
    def configureAmigaCD32(coreSettings: keyValueSettings):
        coreSettings.setString("puae_model", '"CD32"')

    @staticmethod
    def configureAmiga1200(coreSettings: keyValueSettings):
        coreSettings.setString("puae_model", '"A1200"')

    @staticmethod
    def configureAmiga600(coreSettings: keyValueSettings):
        coreSettings.setString("puae_model", '"A600"')

    @staticmethod
    def configureAmstradCPC(coreSettings: keyValueSettings):
        coreSettings.setString("cap32_model", '"6128"')

    @staticmethod
    def configureAmstradGX4000(coreSettings: keyValueSettings):
        coreSettings.setString("cap32_model", '"6128+"')

    @staticmethod
    def configureAtari5200(coreSettings: keyValueSettings):
        coreSettings.setString("atari800_system", '"5200"')

        import configgen.recalboxFiles as recalboxFiles
        a800settings = keyValueSettings(recalboxFiles.atari800CustomConfig)
        a800settings.loadFile(True)
        a800settings.setString("MACHINE_TYPE", "Atari 5200")
        a800settings.setInt("RAM_SIZE", 16)
        a800settings.setInt("STEREO_POKEY", 0)
        a800settings.setInt("BUILTIN_BASIC", 0)
        a800settings.saveFile()

    @staticmethod
    def configureAtari800(coreSettings: keyValueSettings):
        coreSettings.setString("atari800_system", '"130XE (128K)"')

        import configgen.recalboxFiles as recalboxFiles
        a800settings = keyValueSettings(recalboxFiles.atari800CustomConfig)
        a800settings.loadFile(True)
        a800settings.setString("MACHINE_TYPE", "Atari XL/XE")
        a800settings.setInt("RAM_SIZE", 64)
        a800settings.setInt("STEREO_POKEY", 1)
        a800settings.setInt("BUILTIN_BASIC", 1)
        a800settings.saveFile()

    def configureAtariST(self, _):
        #print("ATARI DETECTED!")
        MACHINE_ST = 0
        MACHINE_STE = 1
        MACHINE_TT = 2
        MACHINE_FALCON = 3,
        MACHINE_MEGA_STE = 4

        # Load config
        from configgen.settings.iniSettings import IniSettings
        import configgen.recalboxFiles as recalboxFiles
        atariStSettings = IniSettings(recalboxFiles.hatariCustomConfig)
        atariStSettings.loadFile(True)

        # Auto set machine
        from typing import Dict
        # Try identify machine folder
        machines: Dict[str, int] = \
        {
            "atari-st": MACHINE_ST,
            "/st/": MACHINE_ST,
            "atariste": MACHINE_STE,
            "atari-ste": MACHINE_STE,
            "/ste/": MACHINE_STE,
            "ataritt": MACHINE_TT,
            "atari-tt": MACHINE_TT,
            "/tt/": MACHINE_TT,
            "atarimegaste": MACHINE_MEGA_STE,
            "atari-megaste": MACHINE_MEGA_STE,
            "atari-mega-ste": MACHINE_MEGA_STE,
            "megaste": MACHINE_MEGA_STE,
            "mega-ste": MACHINE_MEGA_STE,
            "falcon": MACHINE_FALCON,
            "atari-falcon": MACHINE_FALCON,
        }
        defaultMachine: int = MACHINE_ST
        romLower: str = self.rom.lower()
        for key in machines:
            if key in romLower:
                defaultMachine: int = machines[key]
                break
        print("MACHINE: {}".format(defaultMachine))
        atariStSettings.setString("System", "nMachineType", str(defaultMachine))

        # Select bios
        import os
        subdir: str = "atarist"
        biosPath = os.path.join(recalboxFiles.BIOS, subdir, "st.img")
        if   defaultMachine == MACHINE_STE     : biosPath = os.path.join(recalboxFiles.BIOS, subdir, "ste.img")
        elif defaultMachine == MACHINE_TT      : biosPath = os.path.join(recalboxFiles.BIOS, subdir, "tt.img")
        elif defaultMachine == MACHINE_MEGA_STE: biosPath = os.path.join(recalboxFiles.BIOS, subdir, "megaste.img")
        elif defaultMachine == MACHINE_FALCON  : biosPath = os.path.join(recalboxFiles.BIOS, subdir, "falcon.img")
        if not os.path.exists(biosPath):
            biosPath = os.path.join(recalboxFiles.BIOS, subdir, "tos.img")
            if not os.path.exists(biosPath):
                biosPath = os.path.join(recalboxFiles.BIOS, "tos.img")
                if not os.path.exists(biosPath):
                    biosPath = os.path.join(recalboxFiles.BIOS, subdir, "emutos.img")
        if "emutos" in self.rom:
            biosPath = os.path.join(recalboxFiles.BIOS, subdir, "emutos.img")
        atariStSettings.setString("ROM", "szTosImageFileName", str(biosPath))
        print("BIOS   : {}".format(biosPath))

        # Save config
        atariStSettings.saveFile()

    @staticmethod
    def configure64DD(coreSettings: keyValueSettings):
        coreSettings.setString("parallel-n64-boot-device", '"64DD IPL"')
        coreSettings.setString("parallel-n64-64dd-hardware", '"enabled"')
        coreSettings.setString("parallel-n64-gfxplugin", '"gln64"')

    @staticmethod
    def configureSpectravideo(coreSettings: keyValueSettings):
        coreSettings.setString("bluemsx_msxtype", '"SVI - Spectravideo SVI-328 MK2"')

    @staticmethod
    def configureMsx1(coreSettings: keyValueSettings):
        coreSettings.setString("bluemsx_msxtype", '"MSX"')

    @staticmethod
    def configureMsx2(coreSettings: keyValueSettings):
        coreSettings.setString("bluemsx_msxtype", '"MSX2+"')

    @staticmethod
    def configureMsxTurboR(coreSettings: keyValueSettings):
        coreSettings.setString("bluemsx_msxtype", '"MSXturboR"')

    @staticmethod
    def configureOdyssey2(coreSettings: keyValueSettings):
        coreSettings.setString("o2em_bios", '"o2rom.bin"')

    @staticmethod
    def configureVideoPacPlus(coreSettings: keyValueSettings):
        coreSettings.setString("o2em_bios", '"g7400.bin"')

    @staticmethod
    def configureSwanstation(coreSettings: keyValueSettings):
        coreSettings.setString("duckstation_Controller2.Type", '"DigitalController"')

    @staticmethod
    def configureDosBoxPure(coreSettings: keyValueSettings):
        coreSettings.setString("dosbox_pure_savestate", '"rewind"')

    @staticmethod
    def configureGenesisPlusGxWide(coreSettings: keyValueSettings):
        from configgen.utils.videoMode import getCurrentFramebufferRatio
        ratio = getCurrentFramebufferRatio()
        currentColumns = 10
        # Ratio to columns (16/9 and more => 10 cols, 1.6 = 16/10 => 6 cols, 1.5 = GOA => 2 cols, 1.34 = 4/3 => 0 cols)
        if ratio <= 1.6:
            currentColumns = 6
        if ratio <= 1.5:
            currentColumns = 2
        if ratio <= 1.34:
            currentColumns = 0
        coreSettings.setString("genesis_plus_gx_wide_h40_extra_columns", '"{}"'.format(currentColumns))

    # Fill cores configuration
    def fillCoresConfiguration(self):
        settings = self.settings

        # Specific configuration handlers per core
        specificCoreHandlers =\
        {
            "bluemsx" : LibretroCores.configureBlueMSX,
            "mame2003_plus": LibretroCores.configureMAME2003plus,
            "opera": LibretroCores.configureOpera,
            "parallel_n64": LibretroCores.configurePARALLELN64,
            "np2kai" : LibretroCores.configureNPKAI,
            "swanstation" : LibretroCores.configureSwanstation,
            "genesisplusgxwide": LibretroCores.configureGenesisPlusGxWide,
            "dosbox_pure" : LibretroCores.configureDosBoxPure,
        }

        # Get handler and execute
        core = self.system.Core
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
            "atarist"      : self.configureAtariST,
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
        if self.system.Name in specificSystemHandlers:
            function = specificSystemHandlers.get(self.system.Name)
            function(settings)
