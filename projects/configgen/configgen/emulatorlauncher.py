#!/usr/bin/env python
import argparse
from Emulator import Emulator
import utils.runner as runner
import recalboxFiles

from controllersConfig import Controller
from settings.keyValueSettings import keyValueSettings

lineAppleGeneratorOverride = None

def getGenerator(emulator):

    # We use a if...elif...else structure in order to instantiate the strict minimum required for an emulator
    # and minimize static imports that are very time consuming, specialy on low-end boards
    if emulator == "libretro":
        module = __import__("generators.libretro.libretroGenerator", fromlist=["LibretroGenerator"])
        generatorClass = getattr(module, "LibretroGenerator")
        return generatorClass()
    elif emulator == "mupen64plus":
        module = __import__("generators.mupen.mupenGenerator", fromlist=["MupenGenerator"])
        generatorClass = getattr(module, "MupenGenerator")
        return generatorClass()
    elif emulator == "reicast":
        module = __import__("generators.reicast.reicastGenerator", fromlist=["ReicastGenerator"])
        generatorClass = getattr(module, "ReicastGenerator")
        return generatorClass()
    elif emulator == "dolphin":
        module = __import__("generators.dolphin.dolphinGenerator", fromlist=["DolphinGenerator"])
        generatorClass = getattr(module, "DolphinGenerator")
        return generatorClass()
    elif emulator == "ppsspp":
        module = __import__("generators.ppsspp.ppssppGenerator", fromlist=["PPSSPPGenerator"])
        generatorClass = getattr(module, "PPSSPPGenerator")
        return generatorClass()
    elif emulator == "amiberry":
        module = __import__("generators.amiberry.amiberryGenerator", fromlist=["AmiberryGenerator"])
        generatorClass = getattr(module, "AmiberryGenerator")
        return generatorClass()
    elif emulator == "daphne":
        module = __import__("generators.daphne.daphneGenerator", fromlist=["DaphneGenerator"])
        generatorClass = getattr(module, "DaphneGenerator")
        return generatorClass()
    elif emulator == "scummvm":
        module = __import__("generators.scummvm.scummvmGenerator", fromlist=["ScummVMGenerator"])
        generatorClass = getattr(module, "ScummVMGenerator")
        return generatorClass()
    elif emulator == "dosbox":
        module = __import__("generators.dosbox.dosboxGenerator", fromlist=["DosBoxGenerator"])
        generatorClass = getattr(module, "DosBoxGenerator")
        return generatorClass()
    elif emulator == "residualvm":
        module = __import__("generators.residualvm.residualvmGenerator", fromlist=["ResidualVMGenerator"])
        generatorClass = getattr(module, "ResidualVMGenerator")
        return generatorClass()
    elif emulator == "advancemame":
        module = __import__("generators.advancemame.advMameGenerator", fromlist=["AdvMameGenerator"])
        generatorClass = getattr(module, "AdvMameGenerator")
        return generatorClass()
    elif emulator == "simcoupe":
        module = __import__("generators.simcoupe.simcoupeGenerator", fromlist=["SimCoupeGenerator"])
        generatorClass = getattr(module, "SimCoupeGenerator")
        return generatorClass()
    elif emulator == "gsplus":
        module = __import__("generators.gsplus.gsplusGenerator", fromlist=["GSplusGenerator"])
        generatorClass = getattr(module, "GSplusGenerator")
        return generatorClass()
    elif emulator == "oricutron":
        module = __import__("generators.oricutron.oricutronGenerator", fromlist=["OricutronGenerator"])
        generatorClass = getattr(module, "OricutronGenerator")
        return generatorClass()
    elif emulator == "linapple":
        if lineAppleGeneratorOverride is not None:
            return lineAppleGeneratorOverride
        module = __import__("generators.linapple.linappleGenerator", fromlist=["LinappleGenerator"])
        generatorClass = getattr(module, "LinappleGenerator")
        os = __import__("os")
        return generatorClass(os.path.join(recalboxFiles.HOME_INIT, '.linapple'),
                              os.path.join(recalboxFiles.HOME     , '.linapple'))
    elif emulator == "kodi":
        module = __import__("generators.kodi.kodiGenerator", fromlist=["KodiGenerator"])
        generatorClass = getattr(module, "KodiGenerator")
        return generatorClass()
    elif emulator == "fba2x":
        module = __import__("generators.fba2x.fba2xGenerator", fromlist=["Fba2xGenerator"])
        generatorClass = getattr(module, "Fba2xGenerator")
        return generatorClass()
    elif emulator == "moonlight":
        module = __import__("generators.moonlight.moonlightGenerator", fromlist=["MoonlightGenerator"])
        generatorClass = getattr(module, "MoonlightGenerator")
        return generatorClass()
    elif emulator == "vice":
        module = __import__("generators.vice.viceGenerator", fromlist=["ViceGenerator"])
        generatorClass = getattr(module, "ViceGenerator")
        return generatorClass()
    elif emulator == "pcsx_rearmed":
        module = __import__("generators.pcsx.pcsxGenerator", fromlist=["PcsxGenerator"])
        generatorClass = getattr(module, "PcsxGenerator")
        return generatorClass()
    else:
        print("Missing generator for {}".format(emulator))
        raise ValueError
        pass


# List emulators with their cores rest mupen64, scummvm
def getDefaultEmulator(targetSystem):
    emulators = \
    {
        # Nintendo
        "snes"          : Emulator(name='snes', emulator='libretro', core='snes9x2002'),
        "nes"           : Emulator(name='nes', emulator='libretro', core='fceunext'),
        "n64"           : Emulator(name='n64', emulator='mupen64plus', core='gliden64'),
        "gba"           : Emulator(name='gba', emulator='libretro', core='mgba'),
        "gb"            : Emulator(name='gb', emulator='libretro', core='gambatte'),
        "gbc"           : Emulator(name='gbc', emulator='libretro', core='gambatte'),
        "fds"           : Emulator(name='fds', emulator='libretro', core='nestopia'),
        "virtualboy"    : Emulator(name='virtualboy', emulator='libretro', core='mednafen_vb'),
        "gamecube"      : Emulator(name='gamecube', emulator='dolphin'),
        "wii"           : Emulator(name='wii', emulator='dolphin'),
        "nds"           : Emulator(name='nds', emulator='libretro', core='desmume'),
        "pokemini"      : Emulator(name='pokemini', emulator='libretro', core='pokemini'),
        "satellaview"   : Emulator(name='satellaview', emulator='libretro', core='snes9x'),
        "sufami"        : Emulator(name='sufami', emulator='libretro', core='snes9x'),
        "gw"            : Emulator(name='gw', emulator='libretro', core='gw'),
        "3ds"           : Emulator(name='3ds', emulator='libretro', core='citra_canary'),

        # Sega
        "sg1000"        : Emulator(name='sg1000', emulator='libretro', core='genesisplusgx'),
        "mastersystem"  : Emulator(name='mastersystem', emulator='libretro', core='picodrive'),
        "megadrive"     : Emulator(name='megadrive', emulator='libretro', core='picodrive'),
        "gamegear"      : Emulator(name='gamegear', emulator='libretro', core='genesisplusgx'),
        "sega32x"       : Emulator(name='sega32x', emulator='libretro', core='picodrive'),
        "segacd"        : Emulator(name='segacd', emulator='libretro', core='picodrive'),
        "saturn"        : Emulator(name='segacd', emulator='libretro', core='beetle_saturn'),
        "dreamcast"     : Emulator(name='dreamcast', emulator='reicast'),

        # Arcade
        "neogeo"        : Emulator(name='neogeo', emulator='fba2x'),
        "mame"          : Emulator(name='mame', emulator='libretro', core='mame2003_plus'),
        "fba"           : Emulator(name='fba', emulator='fba2x'),
        "fba_libretro"  : Emulator(name='fba_libretro', emulator='libretro', core='fbalpha'),
        "advancemame"   : Emulator(name='advancemame', emulator='advmame'),
        "daphne"        : Emulator(name='daphne', emulator='daphne'),
        "neogeocd"      : Emulator(name='neogeocd', emulator='libretro', core='fbalpha'),
        "atomiswave"    : Emulator(name='atomiswave', emulator='libretro', core='flycast'),
        "naomi"         : Emulator(name='naomi', emulator='libretro', core='flycast'),

        # Atari
        "atari2600"     : Emulator(name='atari2600', emulator='libretro', core='stella'),
        "atari5200"     : Emulator(name='atari5200', emulator='libretro', core='atari800'),
        "atari7800"     : Emulator(name='atari7800', emulator='libretro', core='prosystem'),
        "lynx"          : Emulator(name='lynx', emulator='libretro', core='handy'),
        "jaguar"        : Emulator(name='jaguar', emulator='libretro', core='virtualjaguar'),

        # Computers
        "amiga600"      : Emulator(name='amiga600', emulator='amiberry'),
        "amiga1200"     : Emulator(name='amiga1200', emulator='amiberry'),
        "msx"           : Emulator(name='msx', emulator='libretro', core='bluemsx'),
        "msx1"          : Emulator(name='msx1', emulator='libretro', core='bluemsx'),
        "msx2"          : Emulator(name='msx2', emulator='libretro', core='bluemsx'),
        "spectravideo"  : Emulator(name='spectravideo', emulator='libretro', core='bluemsx'),
        "amiga"         : Emulator(name='amiga', emulator='libretro', core='puae'),
        "amstradcpc"    : Emulator(name='amstradcpc', emulator='libretro', core='cap32'),
        "apple2"        : Emulator(name='apple2', emulator='linapple', videomode='default'),
        "apple2gs"      : Emulator(name='apple2gs', emulator='gsplus', videomode='default'),
        "atarist"       : Emulator(name='atarist', emulator='libretro', core='hatari'),
        "zxspectrum"    : Emulator(name='zxspectrum', emulator='libretro', core='fuse'),
        "o2em"          : Emulator(name='odyssey2', emulator='libretro', core='o2em'),
        "zx81"          : Emulator(name='zx81', emulator='libretro', core='81'),
        "dos"           : Emulator(name='dos', emulator='dosbox', videomode='default'),
        "c64"           : Emulator(name='c64', emulator='libretro', core='vice_x64'),
        "x1"            : Emulator(name='x1', emulator='libretro', core='xmil'),
        "x68000"        : Emulator(name='x68000', emulator='libretro', core='px68k'),
        "thomson"       : Emulator(name='thomson', emulator='libretro', core='theodore'),
        "atari800"      : Emulator(name='atari800', emulator='libretro', core='atari800'),
        "oricatmos"     : Emulator(name='oricatmos', emulator='oricutron', videomode='default'),
        "samcoupe"      : Emulator(name='samcoupe', emulator='simcoupe', videomode='default'),
        "pc88"          : Emulator(name='pc88', emulator='libretro', core='quasi88'),
        "pc98"          : Emulator(name='pc98', emulator='libretro', core='np2kai'),
        "macintosh"     : Emulator(name='macintosh', emulator='libretro', core='minivmac'),

        # Game engines
        "scummvm"       : Emulator(name='scummvm', emulator='scummvm', videomode='default'),
        "prboom"        : Emulator(name='prboom', emulator='libretro', core='prboom'),
        "lutro"         : Emulator(name='lutro', emulator='libretro', core='lutro'),
        "cavestory"     : Emulator(name='cavestory', emulator='libretro', core='nxengine'),

        # Consoles
        "psp"           : Emulator(name='psp', emulator='ppsspp'),
        "ngp"           : Emulator(name='ngp', emulator='libretro', core='mednafen_ngp'),
        "ngpc"          : Emulator(name='ngpc', emulator='libretro', core='mednafen_ngp'),
        "vectrex"       : Emulator(name='vectrex', emulator='libretro', core='vecx'),
        "wswan"         : Emulator(name='wswan', emulator='libretro', core='mednafen_wswan', ratio='16/10'),
        "wswanc"        : Emulator(name='wswanc', emulator='libretro', core='mednafen_wswan', ratio='16/10'),
        "pcengine"      : Emulator(name='pcengine', emulator='libretro', core='mednafen_supergrafx'),
        "pcenginecd"    : Emulator(name='pcenginecd', emulator='libretro', core='mednafen_supergrafx'),
        "pcfx"          : Emulator(name='pcfx', emulator='libretro', core='mednafen_pcfx'),
        "supergrafx"    : Emulator(name='supergrafx', emulator='libretro', core='mednafen_supergrafx'),
        "psx"           : Emulator(name='psx', emulator='libretro', core='pcsx_rearmed'),
        "intellivision" : Emulator(name='intellivision', emulator='libretro', core='freeintv'),
        "channelf"      : Emulator(name='channelf', emulator='libretro', core='freechaf'),
        "colecovision"  : Emulator(name='colecovision', emulator='libretro', core='bluemsx'),
        "3do"           : Emulator(name='3do', emulator='libretro', core='4do'),
        "amigacd32"     : Emulator(name='amigacd32', emulator='amiberry'),
        "amigacdtv"     : Emulator(name='amigacdtv', emulator='amiberry'),
        "gx4000"        : Emulator(name='gx4000', emulator='libretro', core='cap32'),
        "uzebox"        : Emulator(name='uzebox', emulator='libretro', core='uzem'),
        "palm"          : Emulator(name='palm', emulator='libretro', core='mu'),

        # Miscellaneous
        "imageviewer"   : Emulator(name='imageviewer', emulator='libretro', core='imageviewer'),
        "kodi"          : Emulator(name='kodi', emulator='kodi', videomode='default'),
        "moonlight"     : Emulator(name='moonlight', emulator='moonlight'),
    }

    if targetSystem in emulators:
        return emulators[targetSystem]

    return None


def loadRecalboxSettings(rom, systemname):

    os = __import__("os")

    # Save dir
    dirname = os.path.join(recalboxFiles.savesDir, systemname)
    if not os.path.exists(dirname):
        os.makedirs(dirname)

    # Load global settings
    recalboot = keyValueSettings("/boot/recalbox-boot.conf", False)
    recalboot.loadFile(True)
    fixedScreenSize = recalboot.getOption("case", "") in ("GPiV1", "GPiV2", "GPiV3")
    del recalboot

    # Load global settings
    settings = keyValueSettings(recalboxFiles.recalboxConf, False)
    settings.loadFile(True)
    
    if rom is not None:
        # build file names
        romPath = os.path.dirname(rom)
        romFile, romWithoutExt = os.path.splitext(rom)
        romSettings = "{}{}".format(romFile, ".recalbox.conf")
        pathSettings = os.path.join(romPath, ".recalbox.conf")
        # Override with path settings
        settings.changeSettingsFile(pathSettings)
        settings.loadFile(False)
        # Override with rom settings
        settings.changeSettingsFile(romSettings)
        settings.loadFile(False)

    return settings, fixedScreenSize


def main(arguments):
    if not arguments.demo:
        demoStartButtons = dict()
        # Read the controller configuration
        playersControllers = Controller.loadControllerConfig(arguments.p1index, arguments.p1guid, arguments.p1name, arguments.p1devicepath, arguments.p1nbaxes,
                                                             arguments.p2index, arguments.p2guid, arguments.p2name, arguments.p2devicepath, arguments.p2nbaxes,
                                                             arguments.p3index, arguments.p3guid, arguments.p3name, arguments.p3devicepath, arguments.p3nbaxes,
                                                             arguments.p4index, arguments.p4guid, arguments.p4name, arguments.p4devicepath, arguments.p4nbaxes,
                                                             arguments.p5index, arguments.p5guid, arguments.p5name, arguments.p5devicepath, arguments.p5nbaxes)
    else:
        playersControllers = dict()
        demoStartButtons = Controller.loadDemoConfig(arguments.p1index, arguments.p1guid, arguments.p1name, arguments.p1devicepath, arguments.p1nbaxes,
                                                     arguments.p2index, arguments.p2guid, arguments.p2name, arguments.p2devicepath, arguments.p2nbaxes,
                                                     arguments.p3index, arguments.p3guid, arguments.p3name, arguments.p3devicepath, arguments.p3nbaxes,
                                                     arguments.p4index, arguments.p4guid, arguments.p4name, arguments.p4devicepath, arguments.p4nbaxes,
                                                     arguments.p5index, arguments.p5guid, arguments.p5name, arguments.p5devicepath, arguments.p5nbaxes)

    systemName = arguments.system

    # Main Program
    # A generator will configure its emulator, and return a command
    system = getDefaultEmulator(systemName)
    if system is not None:
        # Load recalbox.conf
        recalboxSettings, fixedScreenSize = loadRecalboxSettings(arguments.rom, system.name)

        # Configure attributes
        system.configure(recalboxSettings, arguments.emulator, arguments.core, arguments.ratio, arguments.netplay, arguments.netplay_ip, arguments.netplay_port, arguments.hash, arguments.extra)

        # Wrong way?
        if system.config['emulator'] not in recalboxFiles.recalboxBins:
            strErr = "ERROR : {} is not a known emulator".format(system.config['emulator'])
            sys = __import__("sys")
            print >> sys.stderr, strErr
            exit(2)

        # Generate all the config files required by the selected emulator
        command = getGenerator(system.config['emulator']).generate(system, arguments.rom, playersControllers, arguments.demo, recalboxSettings)

        # The next line is commented and will eventually be used instead of the previous one
        # if we even want the binary to be set from here rather than from the generator
        # command.array.insert(0, recalboxFiles.recalboxBins[system.config['emulator']])
        print(command)
        returnCode = runner.runCommand(command, arguments, demoStartButtons, recalboxSettings, fixedScreenSize)

        # Rerun emulator in play mode
        if returnCode == runner.USERWANNAPLAY:
            print("User wanna play!")
            arguments.demo = None
            arguments.demoduration = 0
            arguments.demoinfoduration = 0
            main(arguments)
            return runner.USERQUIT

        return returnCode, not fixedScreenSize
    
    else:
        sys = __import__("sys")
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
     "libretro",
     "mupen64plus",
     "reicast",
     "dolphin",
     "ppsspp",
     "amiberry",
     "daphne",
     "scummvm",
     "dosbox",
     "residualvm",
     "advancemame",
     "simcoupe",
     "gsplus",
     "oricutron",
     "linapple",
     "kodi",
     "fba2x",
     "moonlight",
     "vice",
     "pcsx_rearmed"
    )

    res = True
    for emulator in emulatorList:
        res &= getGenerator(emulator).config_upgrade(version)
    return res


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='emulator-launcher script')
    parser.add_argument("-p1index", help="player1 controller index", type=int, required=False)
    parser.add_argument("-p1guid", help="player1 controller SDL2 guid", type=str, required=False)
    parser.add_argument("-p1name", help="player1 controller name", type=str, required=False)
    parser.add_argument("-p1devicepath", help="player1 controller device", type=str, required=False)
    parser.add_argument("-p1nbaxes", help="player1 controller number of axes", type=str, required=False)
    parser.add_argument("-p2index", help="player2 controller index", type=int, required=False)
    parser.add_argument("-p2guid", help="player2 controller SDL2 guid", type=str, required=False)
    parser.add_argument("-p2name", help="player2 controller name", type=str, required=False)
    parser.add_argument("-p2devicepath", help="player2 controller device", type=str, required=False)
    parser.add_argument("-p2nbaxes", help="player2 controller number of axes", type=str, required=False)
    parser.add_argument("-p3index", help="player3 controller index", type=int, required=False)
    parser.add_argument("-p3guid", help="player3 controller SDL2 guid", type=str, required=False)
    parser.add_argument("-p3name", help="player3 controller name", type=str, required=False)
    parser.add_argument("-p3devicepath", help="player3 controller device", type=str, required=False)
    parser.add_argument("-p3nbaxes", help="player3 controller number of axes", type=str, required=False)
    parser.add_argument("-p4index", help="player4 controller index", type=int, required=False)
    parser.add_argument("-p4guid", help="player4 controller SDL2 guid", type=str, required=False)
    parser.add_argument("-p4name", help="player4 controller name", type=str, required=False)
    parser.add_argument("-p4devicepath", help="player4 controller device", type=str, required=False)
    parser.add_argument("-p4nbaxes", help="player4 controller number of axes", type=str, required=False)
    parser.add_argument("-p5index", help="player5 controller index", type=int, required=False)
    parser.add_argument("-p5guid", help="player5 controller SDL2 guid", type=str, required=False)
    parser.add_argument("-p5name", help="player5 controller name", type=str, required=False)
    parser.add_argument("-p5devicepath", help="player5 controller device", type=str, required=False)
    parser.add_argument("-p5nbaxes", help="player5 controller number of axes", type=str, required=False)
    parser.add_argument("-system", help="select the system to launch", type=str, required=True)
    parser.add_argument("-rom", help="rom absolute path", type=str, required=False)
    parser.add_argument("-emulator", help="force emulator", type=str, required=False)
    parser.add_argument("-core", help="force emulator core", type=str, required=False)
    parser.add_argument("-ratio", help="force game ratio", type=str, required=False)
    parser.add_argument("-demo", help="mode demo", type=bool, required=False)
    parser.add_argument("-demoduration", help="mode demo duration in second", type=int, required=False)
    parser.add_argument("-demoinfoduration", help="mode demo outscreen duration in second", type=int, required=False)
    parser.add_argument("-netplay", help="host/client", type=str, required=False)
    parser.add_argument("-netplay_ip", help="host IP", type=str, required=False)
    parser.add_argument("-netplay_port", help="host port (not used in client mode)", type=str, required=False)
    parser.add_argument("-hash", help="force rom crc", type=str, required=False)
    parser.add_argument("-extra", help="pass extra argument", type=str, required=False)

    args = parser.parse_args()
    exitcode, waitNeeded = main(args)
    # Investigate : is this delay still required?
    if waitNeeded:
        __import__("time").sleep(0.5)
    exit(exitcode)

