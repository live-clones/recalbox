#!/usr/bin/env python
import argparse

from configgen.Emulator import Emulator
import configgen.utils.runner as runner
import configgen.recalboxFiles as recalboxFiles

from configgen.controllersConfig import Controller
from configgen.settings.configOverriding import buildOverrideChain
from configgen.settings.keyValueSettings import keyValueSettings

lineAppleGeneratorOverride = None

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
    else:
        print("Missing generator for {}".format(emulator))
        raise ValueError
        pass


# List emulators with their cores rest mupen64, scummvm
def getDefaultEmulator(targetSystem):
    emulators = \
    {
        # Nintendo
        "3ds"            : Emulator(name='3ds', emulator='libretro', core='citra_canary'),
        "64dd"           : Emulator(name='64dd', emulator='libretro', core='parallel_n64'),
        "fds"            : Emulator(name='fds', emulator='libretro', core='nestopia'),
        "gamecube"       : Emulator(name='gamecube', emulator='dolphin'),
        "gb"             : Emulator(name='gb', emulator='libretro', core='gambatte'),
        "gba"            : Emulator(name='gba', emulator='libretro', core='mgba'),
        "gbc"            : Emulator(name='gbc', emulator='libretro', core='gambatte'),
        "gw"             : Emulator(name='gw', emulator='libretro', core='gw'),
        "n64"            : Emulator(name='n64', emulator='mupen64plus', core='gliden64'),
        "nds"            : Emulator(name='nds', emulator='libretro', core='desmume'),
        "nes"            : Emulator(name='nes', emulator='libretro', core='fceunext'),
        "pokemini"       : Emulator(name='pokemini', emulator='libretro', core='pokemini'),
        "satellaview"    : Emulator(name='satellaview', emulator='libretro', core='snes9x'),
        "snes"           : Emulator(name='snes', emulator='libretro', core='snes9x2002'),
        "sufami"         : Emulator(name='sufami', emulator='libretro', core='snes9x'),
        "virtualboy"     : Emulator(name='virtualboy', emulator='libretro', core='mednafen_vb'),
        "wii"            : Emulator(name='wii', emulator='dolphin'),

        # Sega
        "dreamcast"      : Emulator(name='dreamcast', emulator='reicast', core='reicast'),
        "gamegear"       : Emulator(name='gamegear', emulator='libretro', core='genesisplusgx'),
        "mastersystem"   : Emulator(name='mastersystem', emulator='libretro', core='picodrive'),
        "megadrive"      : Emulator(name='megadrive', emulator='libretro', core='picodrive'),
        "saturn"         : Emulator(name='saturn', emulator='libretro', core='mednafen_saturn'),
        "sega32x"        : Emulator(name='sega32x', emulator='libretro', core='picodrive'),
        "segacd"         : Emulator(name='segacd', emulator='libretro', core='picodrive'),
        "sg1000"         : Emulator(name='sg1000', emulator='libretro', core='genesisplusgx'),

        # Arcade
        "advancemame"    : Emulator(name='advancemame', emulator='advmame'),
        "atomiswave"     : Emulator(name='atomiswave', emulator='libretro', core='flycast'),
        "daphne"         : Emulator(name='daphne', emulator='daphne'),
        "fba"            : Emulator(name='fba', emulator='fba2x'),
        "fbneo"          : Emulator(name='fbneo', emulator='libretro', core='fbneo'),
        "mame"           : Emulator(name='mame', emulator='libretro', core='mame2003_plus'),
        "model3"         : Emulator(name='model3', emulator='supermodel'),
        "naomi"          : Emulator(name='naomi', emulator='libretro', core='flycast'),
        "naomigd"        : Emulator(name='naomigd', emulator='libretro', core='flycast'),
        "neogeo"         : Emulator(name='neogeo', emulator='fba2x'),
        "neogeocd"       : Emulator(name='neogeocd', emulator='libretro', core='fbneo'),
        "stv"            : Emulator(name='stv', emulator='libretro', core='kronos'),

        # Atari
        "atari2600"      : Emulator(name='atari2600', emulator='libretro', core='stella'),
        "atari5200"      : Emulator(name='atari5200', emulator='libretro', core='atari800'),
        "atari7800"      : Emulator(name='atari7800', emulator='libretro', core='prosystem'),
        "jaguar"         : Emulator(name='jaguar', emulator='libretro', core='virtualjaguar'),
        "lynx"           : Emulator(name='lynx', emulator='libretro', core='handy'),

        # Computers
        "amiga1200"      : Emulator(name='amiga1200', emulator='amiberry', core='amiberry'),
        "amiga600"       : Emulator(name='amiga600', emulator='amiberry', core='amiberry'),
        "amstradcpc"     : Emulator(name='amstradcpc', emulator='libretro', core='cap32'),
        "apple2"         : Emulator(name='apple2', emulator='linapple', videomode='default'),
        "apple2gs"       : Emulator(name='apple2gs', emulator='gsplus', videomode='default'),
        "atari800"       : Emulator(name='atari800', emulator='libretro', core='atari800'),
        "atarist"        : Emulator(name='atarist', emulator='libretro', core='hatari'),
        "bk"             : Emulator(name='bk', emulator='libretro', core='bk'),
        "c128"           : Emulator(name='c128', emulator='libretro', core='vice_x128'),
        "c64"            : Emulator(name='c64', emulator='libretro', core='vice_x64'),
        "cbm2"           : Emulator(name='cbm2', emulator='libretro', core='vice_xcbm2'),
        "dos"            : Emulator(name='dos', emulator='dosbox', videomode='default'),
        "dragon"         : Emulator(name='dragon', emulator='xroar'),
        "lowresnx"       : Emulator(name='lowresnx', emulator='libretro', core='lowresnx'),
        "macintosh"      : Emulator(name='macintosh', emulator='libretro', core='minivmac'),
        "msx1"           : Emulator(name='msx1', emulator='libretro', core='bluemsx'),
        "msx2"           : Emulator(name='msx2', emulator='libretro', core='bluemsx'),
        "msxturbor"      : Emulator(name='msxturbor', emulator='libretro', core='bluemsx'),
        "o2em"           : Emulator(name='odyssey2', emulator='libretro', core='o2em'),
        "oricatmos"      : Emulator(name='oricatmos', emulator='oricutron', videomode='default'),
        "pc88"           : Emulator(name='pc88', emulator='libretro', core='quasi88'),
        "pc98"           : Emulator(name='pc98', emulator='libretro', core='np2kai'),
        "pet"            : Emulator(name='pet', emulator='libretro', core='vice_xpet'),
        "pico8"          : Emulator(name='pico8', emulator='libretro', core='retro8'),
        "plus4"          : Emulator(name='plus4', emulator='libretro', core='vice_xplus4'),
        "samcoupe"       : Emulator(name='samcoupe', emulator='simcoupe', videomode='default'),
        "spectravideo"   : Emulator(name='spectravideo', emulator='libretro', core='bluemsx'),
        "thomson"        : Emulator(name='thomson', emulator='libretro', core='theodore'),
        "tic80"          : Emulator(name='tic80', emulator='libretro', core='tic80'),
        "trs80coco"      : Emulator(name='trs80coco', emulator='xroar'),
        "vic20"          : Emulator(name='vic20', emulator='libretro', core='vice_xvic'),
        "videopacplus"   : Emulator(name='videopacplus', emulator='libretro', core='o2em'),
        "x1"             : Emulator(name='x1', emulator='libretro', core='xmil'),
        "x68000"         : Emulator(name='x68000', emulator='libretro', core='px68k'),
        "zx81"           : Emulator(name='zx81', emulator='libretro', core='81'),
        "zxspectrum"     : Emulator(name='zxspectrum', emulator='libretro', core='fuse'),

        # Game engines
        "cavestory"      : Emulator(name='cavestory', emulator='libretro', core='nxengine'),
        "dinothawr"      : Emulator(name='dinothawr', emulator='libretro', core='dinothawr'),
        "doom"           : Emulator(name='doom', emulator='libretro', core='prboom'),
        "doom3"          : Emulator(name='doom3', emulator='libretro', core='boom3'),
        "easyrpg"        : Emulator(name='easyrpg', emulator='libretro', core='easyrpg'),
        "flashback"      : Emulator(name='flashback', emulator='libretro', core='reminiscence'),
        "lutro"          : Emulator(name='lutro', emulator='libretro', core='lutro'),
        "openbor"        : Emulator(name='openbor', emulator='openbor'),
        "quake"          : Emulator(name='quake', emulator='libretro', core='tyrquake'),
        "quake2"         : Emulator(name='quake2', emulator='libretro', core='vitaquake2'),
        "quake3"         : Emulator(name='quake3', emulator='libretro', core='vitaquake3'),
        "rickdangerous"  : Emulator(name='rickdangerous', emulator='libretro', core='xrick'),
        "scummvm"        : Emulator(name='scummvm', emulator='scummvm', videomode='default'),
        "solarus"        : Emulator(name='solarus', emulator='solarus'),
        "startrekvoyager": Emulator(name='startrekvoyager', emulator='libretro', core='vitavoyager'),
        "tombraider"     : Emulator(name='tombraider', emulator='libretro', core='openlara'),
        "wolfenstein3d"  : Emulator(name='wolfenstein3d', emulator='libretro', core='ecwolf'),

        # Consoles
        "3do"            : Emulator(name='3do', emulator='libretro', core='opera'),
        "amigacd32"      : Emulator(name='amigacd32', emulator='libretro', core='puae'),
        "amigacdtv"      : Emulator(name='amigacdtv', emulator='libretro', core='puae'),
        "channelf"       : Emulator(name='channelf', emulator='libretro', core='freechaf'),
        "colecovision"   : Emulator(name='colecovision', emulator='libretro', core='bluemsx'),
        "gx4000"         : Emulator(name='gx4000', emulator='libretro', core='cap32'),
        "intellivision"  : Emulator(name='intellivision', emulator='libretro', core='freeintv'),
        "multivision"    : Emulator(name='multivision', emulator='libretro', core='gearsystem'),
        "ngp"            : Emulator(name='ngp', emulator='libretro', core='mednafen_ngp'),
        "ngpc"           : Emulator(name='ngpc', emulator='libretro', core='mednafen_ngp'),
        "palm"           : Emulator(name='palm', emulator='libretro', core='mu'),
        "pcengine"       : Emulator(name='pcengine', emulator='libretro', core='mednafen_supergrafx'),
        "pcenginecd"     : Emulator(name='pcenginecd', emulator='libretro', core='mednafen_supergrafx'),
        "pcfx"           : Emulator(name='pcfx', emulator='libretro', core='mednafen_pcfx'),
        "pcv2"           : Emulator(name='pcv2', emulator='libretro', core='mednafen_wswan', ratio='16/10'),
        "psp"            : Emulator(name='psp', emulator='ppsspp'),
        "psx"            : Emulator(name='psx', emulator='libretro', core='pcsx_rearmed'),
        "scv"            : Emulator(name='scv', emulator='libretro', core='emuscv'),
        "supergrafx"     : Emulator(name='supergrafx', emulator='libretro', core='mednafen_supergrafx'),
        "supervision"    : Emulator(name='supervision', emulator='libretro', core='potator'),
        "uzebox"         : Emulator(name='uzebox', emulator='libretro', core='uzem'),
        "vectrex"        : Emulator(name='vectrex', emulator='libretro', core='vecx'),
        "wswan"          : Emulator(name='wswan', emulator='libretro', core='mednafen_wswan', ratio='16/10'),
        "wswanc"         : Emulator(name='wswanc', emulator='libretro', core='mednafen_wswan', ratio='16/10'),

        # Miscellaneous
        "ports"          : Emulator(name='ports', emulator=''),
        "2048"           : Emulator(name='2048', emulator='libretro', core='2048'),
        "dungeoncrawlstonesoup" : Emulator(name='dungeoncrawlstonesoup', emulator='libretro', core='stonesoup'),
        "imageviewer"    : Emulator(name='imageviewer', emulator='libretro', core='imageviewer'),
        "kodi"           : Emulator(name='kodi', emulator='kodi', videomode='default'),
        "minecraft"      : Emulator(name='minecraft', emulator='libretro', core='craft'),
        "moonlight"      : Emulator(name='moonlight', emulator='moonlight'),
        "mrboom"         : Emulator(name='mrboom', emulator='libretro', core='mrboom'),
        "outrun"         : Emulator(name='outrun', emulator='libretro', core='cannonball'),
        "thepowdertoy"   : Emulator(name='thepowdertoy', emulator='libretro', core='thepowdertoy'),
    }

    if targetSystem in emulators:
        return emulators[targetSystem]

    return None


def loadRecalboxSettings(rom, systemname):

    # Save dir
    import os
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
        pathSettings = buildOverrideChain(rom, ".recalbox.conf")
        # Override with path settings
        for pathSetting in pathSettings:
            settings.changeSettingsFile(pathSetting)
            settings.loadFile(False)

    return settings, fixedScreenSize


def main(arguments):
    if not arguments.demo:
        demoStartButtons = dict()
        # Read the controller configuration
        playersControllers = Controller.loadControllerConfig(arguments.p1index, arguments.p1guid, arguments.p1name, arguments.p1devicepath, arguments.p1nbaxes, arguments.p1nbhats, arguments.p1nbbuttons,
                                                             arguments.p2index, arguments.p2guid, arguments.p2name, arguments.p2devicepath, arguments.p2nbaxes, arguments.p2nbhats, arguments.p2nbbuttons,
                                                             arguments.p3index, arguments.p3guid, arguments.p3name, arguments.p3devicepath, arguments.p3nbaxes, arguments.p3nbhats, arguments.p3nbbuttons,
                                                             arguments.p4index, arguments.p4guid, arguments.p4name, arguments.p4devicepath, arguments.p4nbaxes, arguments.p4nbhats, arguments.p4nbbuttons,
                                                             arguments.p5index, arguments.p5guid, arguments.p5name, arguments.p5devicepath, arguments.p5nbaxes, arguments.p5nbhats, arguments.p5nbbuttons,
                                                             arguments.p6index, arguments.p6guid, arguments.p6name, arguments.p6devicepath, arguments.p6nbaxes, arguments.p6nbhats, arguments.p6nbbuttons,
                                                             arguments.p7index, arguments.p7guid, arguments.p7name, arguments.p7devicepath, arguments.p7nbaxes, arguments.p7nbhats, arguments.p7nbbuttons,
                                                             arguments.p8index, arguments.p8guid, arguments.p8name, arguments.p8devicepath, arguments.p8nbaxes, arguments.p8nbhats, arguments.p8nbbuttons,
                                                             arguments.p9index, arguments.p9guid, arguments.p9name, arguments.p9devicepath, arguments.p9nbaxes, arguments.p9nbhats, arguments.p9nbbuttons,
                                                             arguments.p10index, arguments.p10guid, arguments.p10name, arguments.p10devicepath, arguments.p10nbaxes, arguments.p10nbhats, arguments.p10nbbuttons)
    else:
        playersControllers = dict()
        demoStartButtons = Controller.loadDemoConfig(arguments.p1index, arguments.p1guid, arguments.p1name, arguments.p1devicepath, arguments.p1nbaxes, arguments.p1nbhats, arguments.p1nbbuttons,
                                                     arguments.p2index, arguments.p2guid, arguments.p2name, arguments.p2devicepath, arguments.p2nbaxes, arguments.p2nbhats, arguments.p2nbbuttons,
                                                     arguments.p3index, arguments.p3guid, arguments.p3name, arguments.p3devicepath, arguments.p3nbaxes, arguments.p3nbhats, arguments.p3nbbuttons,
                                                     arguments.p4index, arguments.p4guid, arguments.p4name, arguments.p4devicepath, arguments.p4nbaxes, arguments.p4nbhats, arguments.p4nbbuttons,
                                                     arguments.p5index, arguments.p5guid, arguments.p5name, arguments.p5devicepath, arguments.p5nbaxes, arguments.p5nbhats, arguments.p5nbbuttons,
                                                     arguments.p6index, arguments.p6guid, arguments.p6name, arguments.p6devicepath, arguments.p6nbaxes, arguments.p6nbhats, arguments.p6nbbuttons,
                                                     arguments.p7index, arguments.p7guid, arguments.p7name, arguments.p7devicepath, arguments.p7nbaxes, arguments.p7nbhats, arguments.p7nbbuttons,
                                                     arguments.p8index, arguments.p8guid, arguments.p8name, arguments.p8devicepath, arguments.p8nbaxes, arguments.p8nbhats, arguments.p8nbbuttons,
                                                     arguments.p9index, arguments.p9guid, arguments.p9name, arguments.p9devicepath, arguments.p9nbaxes, arguments.p9nbhats, arguments.p9nbbuttons,
                                                     arguments.p10index, arguments.p10guid, arguments.p10name, arguments.p10devicepath, arguments.p10nbaxes, arguments.p10nbhats, arguments.p10nbbuttons)

    systemName = arguments.system

    # Cleanup extraction folder
    if arguments.demo:
        import glob
        import os
        for extractedFile in glob.glob("/recalbox/share/extractions/*"):
            os.remove(extractedFile)

    # Main Program
    # A generator will configure its emulator, and return a command
    system = getDefaultEmulator(systemName)
    if system is not None:
        # Load recalbox.conf
        recalboxSettings, fixedScreenSize = loadRecalboxSettings(arguments.rom, system.name)

        # Configure attributes
        system.configure(recalboxSettings, arguments)

        # Wrong way?
        if system.config['emulator'] not in recalboxFiles.recalboxBins:
            import sys
            sys.stderr.write("ERROR : {} is not a known emulator".format(system.config['emulator']))
            return 2, True

        # Generate all the config files required by the selected emulator
        command = getGenerator(system.config['emulator']).generate(system, playersControllers, recalboxSettings, arguments)

        # The next line is commented and will eventually be used instead of the previous one
        # if we even want the binary to be set from here rather than from the generator
        # command.array.insert(0, recalboxFiles.recalboxBins[system.config['emulator']])
        returnCode = runner.runCommand(command, arguments, demoStartButtons, recalboxSettings, fixedScreenSize)

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
    parser = argparse.ArgumentParser(description='emulator-launcher script')
    for i in range(1, 11):
        option = "-p{}".format(i)
        parser.add_argument("{}index".format(option), help="player{} controller index".format(i), type=int, required=False)
        parser.add_argument("{}guid".format(option), help="player{} controller SDL2 guid".format(i), type=str, required=False)
        parser.add_argument("{}name".format(option), help="player{} controller name".format(i), type=str, required=False)
        parser.add_argument("{}devicepath".format(option), help="player{} controller device".format(i), type=str, required=False)
        parser.add_argument("{}nbaxes".format(option), help="player{} controller number of axes".format(i), type=str, required=False)
        parser.add_argument("{}nbhats".format(option), help="player{} controller number of hats".format(i), type=str, required=False)
        parser.add_argument("{}nbbuttons".format(option), help="player{} controller number of buttons".format(i), type=str, required=False)
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
    parser.add_argument("-netplay_playerpassword", help="player password", type=str, required=False, default='')
    parser.add_argument("-netplay_viewerpassword", help="viewer password", type=str, required=False, default='')
    parser.add_argument("-netplay_vieweronly", help="start as viewer only", action="store_true", required=False, default=False)
    parser.add_argument("-hash", help="force rom crc", type=str, required=False)
    parser.add_argument("-extra", help="pass extra argument", type=str, required=False)
    parser.add_argument("-nodefaultkeymap", help="disable libretro default keybindings", action="store_true", required=False)
    parser.add_argument("-verbose", help="verbose logging", action="store_true", required=False)

    args = parser.parse_args()
    exitcode, waitNeeded = main(args)
    # Investigate : is this delay still required?
    if waitNeeded:
        import time
        time.sleep(0.5)
    exit(exitcode)

