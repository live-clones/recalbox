#!/usr/bin/env python
from pathlib import Path
from typing import Dict, List
import configgen.recalboxFiles as recalboxFiles

lineAppleGeneratorOverride = None
recalboxFiles = recalboxFiles

def getGenerator(emulator):

    # We use a if...elif...else structure in order to instantiate the strict minimum required for an emulator
    # and minimize static imports that are very time consuming, specialy on low-end boards
    if emulator == "libretro":
        module = __import__("configgen.generators.libretro.libretroGenerator", fromlist=["LibretroGenerator"])
        generatorClass = getattr(module, "LibretroGenerator")
        return generatorClass()
    elif emulator == "mupen64plus":
        module = __import__("configgen.generators.mupen.mupenGenerator", fromlist=["MupenGenerator"])
        generatorClass = getattr(module, "MupenGenerator")
        return generatorClass()
    elif emulator == "reicast":
        module = __import__("configgen.generators.reicast.reicastGenerator", fromlist=["ReicastGenerator"])
        generatorClass = getattr(module, "ReicastGenerator")
        return generatorClass()
    elif emulator == "dolphin":
        module = __import__("configgen.generators.dolphin.dolphinGenerator", fromlist=["DolphinGenerator"])
        generatorClass = getattr(module, "DolphinGenerator")
        return generatorClass()
    elif emulator == "ppsspp":
        module = __import__("configgen.generators.ppsspp.ppssppGenerator", fromlist=["PPSSPPGenerator"])
        generatorClass = getattr(module, "PPSSPPGenerator")
        return generatorClass()
    elif emulator == "amiberry":
        module = __import__("configgen.generators.amiberry.amiberryGenerator", fromlist=["AmiberryGenerator"])
        generatorClass = getattr(module, "AmiberryGenerator")
        return generatorClass()
    elif emulator == "daphne":
        module = __import__("configgen.generators.daphne.daphneGenerator", fromlist=["DaphneGenerator"])
        generatorClass = getattr(module, "DaphneGenerator")
        return generatorClass()
    elif emulator == "scummvm":
        module = __import__("configgen.generators.scummvm.scummvmGenerator", fromlist=["ScummVMGenerator"])
        generatorClass = getattr(module, "ScummVMGenerator")
        return generatorClass()
    elif emulator == "dosbox":
        module = __import__("configgen.generators.dosbox.dosboxGenerator", fromlist=["DosBoxGenerator"])
        generatorClass = getattr(module, "DosBoxGenerator")
        return generatorClass()
    elif emulator == "advancemame":
        module = __import__("configgen.generators.advancemame.advMameGenerator", fromlist=["AdvMameGenerator"])
        generatorClass = getattr(module, "AdvMameGenerator")
        return generatorClass()
    elif emulator == "simcoupe":
        module = __import__("configgen.generators.simcoupe.simcoupeGenerator", fromlist=["SimCoupeGenerator"])
        generatorClass = getattr(module, "SimCoupeGenerator")
        return generatorClass()
    elif emulator == "gsplus":
        module = __import__("configgen.generators.gsplus.gsplusGenerator", fromlist=["GSplusGenerator"])
        generatorClass = getattr(module, "GSplusGenerator")
        return generatorClass()
    elif emulator == "oricutron":
        module = __import__("configgen.generators.oricutron.oricutronGenerator", fromlist=["OricutronGenerator"])
        generatorClass = getattr(module, "OricutronGenerator")
        return generatorClass()
    elif emulator == "linapple":
        if lineAppleGeneratorOverride is not None:
            return lineAppleGeneratorOverride
        module = __import__("configgen.generators.linapple.linappleGenerator", fromlist=["LinappleGenerator"])
        generatorClass = getattr(module, "LinappleGenerator")
        import os
        return generatorClass(os.path.join(recalboxFiles.HOME_INIT, '.linapple'),
                              os.path.join(recalboxFiles.HOME     , '.linapple'))
    elif emulator == "kodi":
        module = __import__("configgen.generators.kodi.kodiGenerator", fromlist=["KodiGenerator"])
        generatorClass = getattr(module, "KodiGenerator")
        return generatorClass()
    elif emulator == "fba2x":
        module = __import__("configgen.generators.fba2x.fba2xGenerator", fromlist=["Fba2xGenerator"])
        generatorClass = getattr(module, "Fba2xGenerator")
        return generatorClass()
    elif emulator == "moonlight":
        module = __import__("configgen.generators.moonlight.moonlightGenerator", fromlist=["MoonlightGenerator"])
        generatorClass = getattr(module, "MoonlightGenerator")
        return generatorClass()
    elif emulator == "vice":
        module = __import__("configgen.generators.vice.viceGenerator", fromlist=["ViceGenerator"])
        generatorClass = getattr(module, "ViceGenerator")
        return generatorClass()
    elif emulator == "pcsx_rearmed":
        module = __import__("configgen.generators.pcsx.pcsxGenerator", fromlist=["PcsxGenerator"])
        generatorClass = getattr(module, "PcsxGenerator")
        return generatorClass()
    elif emulator == "openbor":
        module = __import__("configgen.generators.openbor.openborGenerator", fromlist=["OpenborGenerator"])
        generatorClass = getattr(module, "OpenborGenerator")
        return generatorClass()
    elif emulator == "solarus":
        module = __import__("configgen.generators.solarus.solarusGenerator", fromlist=["SolarusGenerator"])
        generatorClass = getattr(module, "SolarusGenerator")
        return generatorClass()
    elif emulator == "pisnes":
        module = __import__("configgen.generators.pisnes.pisnesGenerator", fromlist=["PisnesGenerator"])
        generatorClass = getattr(module, "PisnesGenerator")
        return generatorClass()
    elif emulator == "supermodel":
        module = __import__("configgen.generators.supermodel.supermodelGenerator", fromlist=["SupermodelGenerator"])
        generatorClass = getattr(module, "SupermodelGenerator")
        return generatorClass()
    elif emulator == "xroar":
        module = __import__("configgen.generators.xroar.xroarGenerator", fromlist=["XroarGenerator"])
        generatorClass = getattr(module, "XroarGenerator")
        return generatorClass()
    elif emulator == "ti99sim":
        module = __import__("configgen.generators.ti99sim.ti99Generator", fromlist=["TI99Generator"])
        generatorClass = getattr(module, "TI99Generator")
        return generatorClass()
    elif emulator == "hatari":
        module = __import__("configgen.generators.hatari.hatariGenerator", fromlist=["HatariGenerator"])
        generatorClass = getattr(module, "HatariGenerator")
        return generatorClass()
    elif emulator == "duckstation":
        module = __import__("configgen.generators.duckstation.duckstationGenerator", fromlist=["DuckstationGenerator"])
        generatorClass = getattr(module, "DuckstationGenerator")
        return generatorClass()
    elif emulator == "beebem":
        module = __import__("configgen.generators.beebem.beebemGenerator", fromlist=["BeebemGenerator"])
        generatorClass = getattr(module, "BeebemGenerator")
        return generatorClass()
    elif emulator == "pcsx2":
        module = __import__("configgen.generators.pcsx2.pcsx2Generator", fromlist=["Pcsx2Generator"])
        generatorClass = getattr(module, "Pcsx2Generator")
        return generatorClass()
    else:
        print("Missing generator for {}".format(emulator))
        raise ValueError
        pass


# List emulators with their cores rest mupen64, scummvm
def getDefaultEmulator(systemName: str, emulatorName: str, coreName: str):
    # Keep here **ONLY** spécial configurations
    # Since everything is passed by the frontend,
    # we can build a common Emulator class when it has no particular parameter
    from configgen.Emulator import Emulator
    emulators = \
    {
        # Nintendo

        # Sega

        # Arcade

        # Atari

        # Computers
        "oricatmos"      : Emulator(name='oricatmos', emulator='oricutron', core='oricutron', videoMode='default'),
        "samcoupe"       : Emulator(name='samcoupe', emulator='simcoupe', core='simcoupe', videoMode='default'),

        # Game engines
        "scummvm"        : Emulator(name='scummvm', emulator='scummvm', core='scummvm', videoMode='default'),

        # Consoles
        "pcv2"           : Emulator(name='pcv2', emulator='libretro', core='mednafen_wswan', ratio='16/10'),
        "wswan"          : Emulator(name='wswan', emulator='libretro', core='mednafen_wswan', ratio='16/10'),
        "wswanc"         : Emulator(name='wswanc', emulator='libretro', core='mednafen_wswan', ratio='16/10'),

        # Miscellaneous
        "imageviewer"    : Emulator(name='imageviewer', emulator='libretro', core='imageviewer'),
        "kodi"           : Emulator(name='kodi', emulator='kodi', core='kodi', videoMode='default'),
    }

    # Seek for spécific configuration
    if systemName in emulators:
        return emulators[systemName]

    # Otherwise, a common config is built
    emulator = Emulator(name=systemName, emulator=emulatorName, core=coreName)
    return emulator


def loadRecalboxSettings(rom, systemname):

    # Save dir
    import os
    dirname = os.path.join(recalboxFiles.savesDir, systemname)
    if not os.path.exists(dirname):
        os.makedirs(dirname)

    # Load boot settings
    from configgen.settings.keyValueSettings import keyValueSettings
    recalboot = keyValueSettings("/boot/recalbox-boot.conf", False)
    recalboot.loadFile(True)
    fixedScreenSize = recalboot.getString("case", "") in ("GPiV1", "GPiV2", "GPiV3")
    del recalboot
    from configgen.utils.architecture import Architecture
    arch = Architecture()
    if not arch.isSupportingTvService: fixedScreenSize = True

    # Load global settings
    settings = keyValueSettings(recalboxFiles.recalboxConf, False)
    settings.loadFile(True)
    
    if rom is not None:
        # build file names
        from configgen.settings.configOverriding import buildOverrideChain
        pathSettings: List[str] = buildOverrideChain(rom, ".recalbox.conf")
        # Override with path settings
        for pathSetting in pathSettings:
            settings.changeSettingsFile(pathSetting)
            settings.loadFile(False)

    return settings, fixedScreenSize

def main(arguments) -> (int, bool):

    from configgen.controllers.controller import Controller, ControllerPerPlayer
    demoStartButtons: Dict[int, int] = Controller.LoadDemoControllerConfigurations(**vars(arguments)) if arguments.demo else {}
    playersControllers: ControllerPerPlayer = Controller.LoadUserControllerConfigurations(**vars(arguments)) if not arguments.demo else {}

    systemName = arguments.system

    # Cleanup extraction folder
    if arguments.demo:
        import glob
        import os
        for extractedFile in glob.glob("/recalbox/share/extractions/*"):
            os.remove(extractedFile)

    # Main Program
    # A generator will configure its emulator, and return a command
    system = getDefaultEmulator(systemName, arguments.emulator, arguments.core)
    recalboxOptions, fixedScreenSize = loadRecalboxSettings(arguments.rom, system.Name)

    if arguments.crtadaptor is not None and len(arguments.crtadaptor) > 0:
        from configgen.crt.CRTEmulatorSelector import CRTEmulatorSelector
        system = CRTEmulatorSelector().chooseEmulatorAndCoreFor(system, arguments.rom)

    if system is not None:
        # Load recalbox.conf

        # Configure attributes
        system.configure(recalboxOptions, arguments)

        # Wrong way?
        if system.Emulator not in recalboxFiles.recalboxBins:
            import sys
            sys.stderr.write("ERROR : {} is not a known emulator".format(system.Emulator))
            return 2, True

        # Generate all the config files required by the selected emulator
        command = getGenerator(system.Emulator).generate(system, playersControllers, recalboxOptions, arguments)

        import configgen.utils.runner as runner
        returnCode = runner.runCommand(command, arguments, demoStartButtons, recalboxOptions, fixedScreenSize)

        # Rerun emulator in play mode
        if returnCode == runner.USERWANNAPLAY:
            print("User wanna play!")
            arguments.demo = False
            arguments.demoduration = 0
            arguments.demoinfoduration = 0
            main(arguments)
            returnCode = runner.USERQUIT

        return returnCode, not fixedScreenSize

    else:
        import sys
        sys.stderr.write("Unknown system: {}".format(systemName))
        return 1, True


'''
Upgrade all generators user's configuration files with new values added
to their system configuration file upgraded by S11Share:do_upgrade()

Args:
    version (str): New Recalbox version

Returns (bool):
    Returns True if all generators sucessfully handled the upgraded.
'''
def config_upgrade(version):
    emulatorList = \
    (
     "advancemame",
     "amiberry",
     "daphne",
     "dolphin",
     "dosbox",
     "fba2x",
     "gsplus",
     "kodi",
     "libretro",
     "linapple",
     "moonlight",
     "mupen64plus",
     "oricutron",
     "pcsx2",
     "pcsx_rearmed",
     "pisnes",
     "ppsspp",
     "reicast",
     "scummvm",
     "simcoupe",
     "supermodel",
     "vice"
    )

    res = True
    for emulator in emulatorList:
        res &= getGenerator(emulator).config_upgrade(version)
    return res


if __name__ == '__main__':
    from configgen.commandline.parser import Parser
    parser: Parser = Parser()

    for i in range(1, 11):
        option = "p{}".format(i)
        mandatory: bool = i == 0
        parser.AddInt(command="{}index".format(option), chelp="player{} controller index".format(i), required=mandatory)
        parser.AddString(command="{}guid".format(option), chelp="player{} controller SDL2 guid".format(i), required=mandatory)
        parser.AddString(command="{}name".format(option), chelp="player{} controller name".format(i), required=mandatory)
        parser.AddString(command="{}devicepath".format(option), chelp="player{} controller device".format(i), required=mandatory)
        parser.AddInt(command="{}nbaxes".format(option), chelp="player{} controller number of axes".format(i), required=mandatory)
        parser.AddInt(command="{}nbhats".format(option), chelp="player{} controller number of hats".format(i), required=mandatory)
        parser.AddInt(command="{}nbbuttons".format(option), chelp="player{} controller number of buttons".format(i), required=mandatory)
    parser.AddString(command="system", chelp="select the system to launch", required=True)
    parser.AddString(command="rom", chelp="rom absolute path", required=True)
    parser.AddString(command="emulator", chelp="force emulator", required=True)
    parser.AddString(command="core", chelp="force emulator core", required=True)
    parser.AddString(command="ratio", chelp="force game ratio", required=False)
    parser.AddBool(command="demo", chelp="mode demo", required=False)
    parser.AddInt(command="demoduration", chelp="mode demo duration in second", required=False)
    parser.AddInt(command="demoinfoduration", chelp="mode demo outscreen duration in second", required=False)
    parser.AddString(command="netplay", chelp="host/client", required=False)
    parser.AddString(command="netplay_ip", chelp="host IP", required=False)
    parser.AddInt(command="netplay_port", chelp="host port (not used in client mode)", required=False)
    parser.AddString(command="netplay_playerpassword", chelp="player password", required=False)
    parser.AddString(command="netplay_viewerpassword", chelp="viewer password", required=False)
    parser.AddSimple(command="netplay_vieweronly", chelp="start as viewer only", required=False)
    parser.AddString(command="hash", chelp="force rom crc", required=False)
    parser.AddString(command="extra", chelp="pass extra argument", required=False)
    parser.AddSimple(command="nodefaultkeymap", chelp="disable libretro default keybindings", required=False)
    parser.AddSimple(command="verbose", chelp="verbose logging", required=False)
    parser.AddString(command="crtadaptor", chelp="Plugged crt adaptor", required=False)
    parser.AddString(command="crtvideostandard", chelp="Selected crt video standard, auto, pal, ntsc", required=False)
    parser.AddString(command="crtregion", chelp="Selected crt region: us,jp,eu", required=False)
    parser.AddString(command="crtresolutiontype", chelp="progressive, interlaced, or auto", required=False)
    parser.AddString(command="crtscreentype", chelp="15kHz, 31kHz", required=False)
    parser.AddInt(command="crtverticaloffset", chelp="vertical offset on CRT", required=False)
    parser.AddInt(command="crthorizontaloffset", chelp="horizontal offset on CRT", required=False)
    parser.AddInt(command="crtviewportwidth", chelp="screen width on crt", required=False)

    args = parser.Parse()

    exitcode, waitNeeded = main(args)
    # Investigate : is this delay still required?
    if waitNeeded:
        import time
        time.sleep(0.5)
    exit(exitcode)

