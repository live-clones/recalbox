class LibretroCores:

    def __init__(self, system, settings, controllers):
        self.system = system
        self.settings = settings
        self.controllers = controllers

    # Fill cores configuration
    def fillCoresConfiguration(self):
        recalbox = self.system.config
        settings = self.settings

        # fMSX machine
        if recalbox["core"] == "bluemsx":
            settings.setOption("bluemsx_msxtype", '"Auto"')

        # Atari 5200/800XL switch
        if self.system.name == "atari5200":
            settings.setOption("atari800_system", '"5200"')
        elif self.system.name == "atari800":
            settings.setOption("atari800_system", '"800XL (64K)"')
