#! /usr/bin/env python2
from __future__ import print_function

import argparse
import os
import sys
from distutils.dir_util import copy_tree


class EmPack:
    def __init__(self, system, extensions, emcoredef, fullname=None, platform=None, theme=None, force=False, port=False, readonly=False, rompath=None):
        # Set member variable
        """

        :type emcoredef: list
        """
        self._System = system.lower()
        self._Extensions = extensions
        self._EmulatoreCoreDefinitions = emcoredef
        self._FullName = fullname if fullname is not None else self._System.title()
        self._Platform = platform if platform is not None else self._System
        self._Theme = theme if theme is not None else self._System
        self._Force = force
        self._Port = port
        self._ReadOnly = readonly
        self._RomPath = rompath if rompath is not None else ""

        self._SingleMode = False
        self._SingleEmulatorBRPackage = ""
        self._MultiEmulatorBRPackage = dict()
        self._BRVarsList = list()

        if not isinstance(emcoredef, list):
            raise

        # Don't use single emulator!
        if len(emcoredef) == 1 and emcoredef[0].count(':') == 0:
            self._SingleMode = True
            self._SingleEmulatorBRPackage = emcoredef[0]
            self._BRVarsList = [emcoredef[0]]
        else:
            self._SingleMode = False
            self._BRVarsList = self.initmultiem(emcoredef)

        self._CommandLine = self.setcmdline(system, extensions, emcoredef, fullname, platform, theme, rompath)
        self._SystemUpper = self._System.upper()
        self._PackageName = "recalbox-romfs-{}".format(self._System)
        self._PackageDir = "package/recalbox-romfs/{}".format(self._PackageName)
        if self._Port:
            self._RomsDir = "{}/{}/ports/{}".format(self._PackageDir, 'roms', self._FullName)
        else:
            self._RomsDir = "{}/{}/{}".format(self._PackageDir, 'roms', self._System)
        self._Makefile = '{}/{}.mk'.format(self._PackageDir, self._PackageName)

    def __str__(self):
        return """
                system: {}
                extensions: {}
                fullname: {}
                platform: {}
                theme: {}
                rompath: {}
                singleMode: {}
                single emulator package: {}
                multi emulator packages: {}
                ------
                list of BR package variables: {}
                """.format(self._System, self._Extensions, self._FullName, self._Platform, self._Theme, self._RomPath, self._SingleMode, self._SingleEmulatorBRPackage, self._MultiEmulatorBRPackage, self._BRVarsList)

    def setcmdline(self, system, extensions, emcoredef, fullname=None, platform=None, theme=None, rompath=None):
        cmdline = "{} ".format(sys.argv[0])
        if self._Force: cmdline += "--force "
        if self._Port: cmdline += "--port "
        if self._ReadOnly: cmdline += "--readonly "
        cmdline += "--system {} ".format(system)
        cmdline += "--extension '{}' ".format(extensions)
        cmdline += "--fullname '{}' ".format(fullname) if fullname is not None else ""
        cmdline += "--platform {} ".format(platform) if platform is not None else ""
        cmdline += "--theme {} ".format(theme) if theme is not None else ""
        cmdline += "--rompath {} ".format(rompath) if rompath is not None else ""
        cmdline += " ".join(emcoredef)
        return cmdline

    def initmultiem(self, emcorelist):
        """
        Returns the list of BR2_PACKAGE from the command line into a single list
        :type emcorelist: list
        :rtype: list
        """
        superpackage = list()
        for item in emcorelist:
            if item.count(':') != 3:
                print(sys.stderr, "{} must be follow the pattern priority:emulator:core:BR2_PACKAGE_NAME".format(item))
                exit(1)
            try:
                # split priority:emulator:core:BR_VAR into dicts
                priority, emulator, core, brvar = item.split(':')
                # print "{} splitted to {} {} {} {}".format(arg, priority, emulator, core, brVar)
                if emulator not in self._MultiEmulatorBRPackage:
                    self._MultiEmulatorBRPackage[emulator] = dict()

                self._MultiEmulatorBRPackage[emulator][core] = (brvar, priority)
                if brvar not in superpackage:
                    superpackage.append(brvar)
            except:
                raise
        return superpackage

    def listpackages(self):
        """ Appends buildroot package variables to be processed by a ifeq/ifneq command
        :returns: a concatenation of $(VAR1)$(VAR2) etc ... ex: '$(BR2_PACKAGE_LIBRETRO_MAME2003)$(BR2_PACKAGE_LIBRETRO_MAME2000)'
        :rtype: string
        """
        cond = '$('
        cond += ')$('.join(self._BRVarsList)
        cond += ')'
        return cond

    def generatedefinename(self, typeCmd, emulator='', core=''):
        """ Generate a define name for .mk files
        The return string looks like CONFIGURE_system_emulator_core_typeCmd , depending on provided arguments
        :param typeCmd: they type of command
        :type typeCmd: string must be START, END or DEF
        :param emulator: optionnal, the emulator name (ex: libretro)
        :type emulator: string
        :param core: optional, the core name (ex: mame2003)
        :type core: string
        :returns: the define name
        :rtype: string
        """
        if typeCmd not in ('START', 'END', 'DEF'):
            sys.stderr.write("generateDefineName : typeCmd must be START END or DEF\n")
            sys.exit(1)

        if core and not emulator:
            sys.stderr.write("generateDefineName : you must define emulator if you set core\n")
            sys.exit(1)

        defStr = "CONFIGURE_{}".format(self._SystemUpper)

        if emulator:
            emulatorUp = emulator.upper()
            defStr += "_{}".format(emulatorUp)
            if core:
                coreUp = core.upper()
                defStr += "_{}".format(coreUp)

        defStr += "_{}".format(typeCmd)
        return defStr

    def addemulator(self, emulator):
        """ Returns the starting makefile block of a new emulator section in the es_settings.cfg
        the returned string looks like :
        
        ifneq(brVars,)
        define YYY
            echo -e '\t\t<emulator name="{emulator}">\n' > $(SYSTEM_XML_{})
            
        endef

        :returns: a multiline and indented string
        :rtype: string
        """
        #emulatorUp = emulator.upper()
        emulatorLo = emulator.lower()
        defineName = self.generatedefinename('START', emulator)
        #returnStr = "define {}\n".format(defineName)
        cond = self.listpackages()
        
        returnStr = "ifneq ({},)\n".format(cond)
        returnStr += "define {}\n".format(defineName)
        returnStr += "\t$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_{}),{})\n".format(self._SystemUpper, emulatorLo)
        returnStr += "endef\n"
        
        return returnStr, defineName

    def addcore(self, emulator, core, brVar, priority):
        """ Returns the starting makefile block of a new emulator section in the es_settings.cfg
        The returned string looks like:

        ifeq(brVar,y)
        define XXX
            echo -e '\t\t\t<core priority="number">core</core>' >> $(SYSTEM_XML_system)

        endef
        RECALBOX_ROMFS_system_CONFIGURE_CMDS += $(XXX)

        :param emulator: optionnal, the emulator name (ex: libretro)
        :type emulator: string
        :param core: the core name
        :type core: string
        :param brVar: a BR2_PACKAGE_ like name
        :type brVar: string
        :param priority: core priority (lower priority choosen first as default)
        :type priority: int
        :returns: a multiline and indented string
        :rtype: string
        """
        #emulatorUp = emulator.upper()
        #emulatorLo = emulator.lower()
        #coreUp = core.upper()
        coreLo = core.lower()
        defineName = self.generatedefinename('DEF', emulator, core)

        returnStr = "ifeq ($({}),y)\n".format(brVar)
        returnStr += "define {}\n".format(defineName)
        returnStr += "\t$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_{}),{},{})\n".format( self._SystemUpper,coreLo,priority)
        returnStr += "endef\n"
        returnStr += "endif\n\n"

        return returnStr, defineName

    def endemulator(self, emulator):
        """ Returns the ending makefile block of a new emulator section in the es_settings.cfg
        the returned string looks like :

        define XXX
            echo -e '\t\t</emulator>\n' >> $(SYSTEM_XML_MAME)

        endef
        RECALBOX_ROMFS_system_CONFIGURE_CMDS += $(XXX)

        :param emulator: optionnal, the emulator name (ex: libretro)
        :type emulator: string
        :returns: a multiline and indented string
        :rtype: string
        """
        #emulatorUp = emulator.upper()
        defineName = self.generatedefinename('END', emulator)

        returnStr = "define {}\n".format(defineName)
        returnStr += "\t$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_{}))\n".format(self._SystemUpper)
        returnStr += "endef\n"
        returnStr += "endif\n\n"

        return returnStr, defineName

    def writemakefile(self):
        print("== Creating new package dir structure: {}".format(self._RomsDir))
        if not os.path.exists(self._RomsDir):
            try:
                os.makedirs(self._RomsDir)
                print("{} created !".format(self._RomsDir))
            except:
                print("Failed ... Could not make dir {}".format(self._RomsDir))
                raise
        else:
            if not self._Force:
                print("{} already exists ... Are you sure of what you're doing ? Exiting ...".format(self._RomsDir))
                sys.exit(1)

        if self._SingleMode:
            skeletonFile = 'package/recalbox-romfs/recalbox-romfs_single_emulator.skeleton'
        else:
            if len(self._RomPath) > 0:
                skeletonFile = 'package/recalbox-romfs/recalbox-romfs_multicores.rompath.skeleton'
            else:
                if self._Port:
                    skeletonFile = 'package/recalbox-romfs/recalbox-romfs_multicores.ports.skeleton'
                else:
                    skeletonFile = 'package/recalbox-romfs/recalbox-romfs_multicores.skeleton'

        #prtCmdLine = " ".join(sys.argv[:]) # Ugly, sadly ... Should be improved to reflect quotes

        #
        # Replace known patterns
        #
        mkFile = open(skeletonFile, 'r').read()
        #mkFile = mkFile.replace('%COMMAND_LINE%', prtCmdLine)
        mkFile = mkFile.replace('%COMMAND_LINE%', self._CommandLine)
        mkFile = mkFile.replace('%SYSTEM_EXTENSIONS%', self._Extensions)
        mkFile = mkFile.replace('%SYSTEM_UPPER%', self._SystemUpper)
        mkFile = mkFile.replace('%SYSTEM_LOWER%', self._System)
        mkFile = mkFile.replace('%FULLNAME%', self._FullName)
        mkFile = mkFile.replace('%PLATFORM%', self._Platform)
        mkFile = mkFile.replace('%THEME%', self._Theme)
        mkFile = mkFile.replace('%RO%', '1' if self._ReadOnly else '0')
        mkFile = mkFile.replace('%ROMPATH%', self._RomPath)

        #
        # patterns : additionnal actions for multicore systems
        #
        listDefines = list()
        if not self._SingleMode:
            superString = ""
            for emulator, cores in self._MultiEmulatorBRPackage.iteritems():
                print("Adding emulator {} {}".format(self._System, emulator))
                definePart, defineName = self.addemulator(emulator)
                superString += definePart
                listDefines.append(defineName)
                for core, (brVar, priority) in cores.iteritems():
                    definePart, defineName =  self.addcore(emulator, core, brVar, priority)
                    superString += definePart
                    listDefines.append(defineName)

                definePart, defineName = self.endemulator(emulator)
                superString += definePart
                listDefines.append(defineName)
            finalDefines = "$(" + ")\n\t$(".join(listDefines) + ")"
            print(listDefines)
            print(finalDefines)
            mkFile = mkFile.replace('%SUPER_PACKAGE%', self.listpackages())
            mkFile = mkFile.replace('%EMULATORS_AND_CORES%', superString)
            mkFile = mkFile.replace('%EMULATORS_DEFINES%', finalDefines)
        else:
            mkFile = mkFile.replace('%BR2_PACKAGE_NAME%', self._SingleEmulatorBRPackage)

        print("== Writing {} :".format(self._Makefile), end=' ')
        with open(self._Makefile, "w") as f:
            try:
                f.write(mkFile)
                print("OK !")
            except:
                print("Failed ... Couldn't write to {}. Aborting ...".format(mkFile))
                raise

    def writeconfigin(self):
        # Time to write the Config.in
        dependsOn = " \\\n\t  || ".join(self._BRVarsList)
        print("== Writing the Config.in: ", end=' ')
        configIn = "config BR2_PACKAGE_RECALBOX_ROMFS_{}\n".format(self._SystemUpper)
        configIn += "\tbool \"recalbox-romfs-{}\"\n".format(self._System)
        #~ configIn += "\tdefault y\n"
        configIn += "\tselect BR2_PACKAGE_RECALBOX_ROMS\n"
        configIn += "\tdepends on {}\n".format(dependsOn)
        configIn += "\thelp\n"
        configIn += "\t  share_init/roms and xml for {}\n".format(self._System)

        fileConfigIn = "{}/Config.in".format(self._PackageDir)
        try:
            with open(fileConfigIn, "w") as f:
                f.write(configIn)
            print("OK!")
        except:
            print("KO!")
            print("Couldn't write the {}".format(fileConfigIn))
            raise

    def copyoverlaydir(self):
        # Copy the previous fsoverlay if it exists
        overlaydir = "board/recalbox/fsoverlay/recalbox/share_init/roms/{}".format(self._System)
        print("== Copy the previous fsoverlay of this system if it exists:", end=' ')
        if os.path.exists(overlaydir):
            try:
                copy_tree(overlaydir, self._RomsDir)
                # Need to remove the .keep
                print("OK !")
            except:
                print("Failed ... Couldn't copy {} to {}. Aborting ...".format(overlaydir, self._PackageDir))
                raise
        else:
            print("No overlay, creating default files instead:", end=' ')
            LISEZ_MOI = "{}/_lisezmoi.txt".format(self._RomsDir)
            READ_ME = "{}/_readme.txt".format(self._RomsDir)
            try:
                if not os.path.exists(LISEZ_MOI):
                    with open(LISEZ_MOI, "w") as f:
                        f.write("Remplir ce fichier")
                if not os.path.exists(READ_ME):
                    with open(READ_ME, "w") as f:
                        f.write("Please fill the file")
                print("OK !")
            except:
                print("Failed ... couldn't create {} or {}".format(LISEZ_MOI, READ_ME))
                raise

    def finalword(self):
        # Ask the user to add himself to recalbox-rom.mk the following lines:
        print("\nNow you will have to edit :\n")
        # print "  * package/recalbox-romfs/recalbox-romfs/Config.in and add :"
        # print "\tdepends on BR2_PACKAGE_RECALBOX_ROMFS_{}\n\n".format(SYSTEM_UPPER)
        print("  * package/recalbox-romfs/recalbox-romfs/recalbox-romfs.mk :")
        print("    Mind the tabulation. The shell may have added spaces instead\n")
        defLine = "# System: {}\n".format(self._System)
        if not self._SingleMode:
            defLine += "ifneq ({},)\n".format(self.listpackages())
        else:
            defLine += "ifeq ($({}),y)\n".format(self._SingleEmulatorBRPackage)
        defLine += "\tRECALBOX_ROMFS_DEPENDENCIES += {}\n".format(self._PackageName)
        defLine += "endif"
        print(defLine)
        print("\n  * Add a source to package/recalbox-romfs/Config.in :")
        print(" source {}/Config.in".format(self._PackageDir))
        print("\n  * Add dependencies to package/recalbox-romfs/recalbox-romfs/Config.in :")
        print("\n\t  || BR2_PACKAGE_RECALBOX_ROMFS_{}".format(self._SystemUpper))
        print("\n  * Add to the emulators/cores Config.in:\n")
        print("\tselect BR2_PACKAGE_RECALBOX_ROMFS_{}".format(self._SystemUpper))


if __name__ == '__main__':
    # Parse command line
    parser = argparse.ArgumentParser(description='Emulator Packager helper')
    parser.add_argument("-s", "--system", help="Sets the system name ex: snes", type=str, required=True)
    parser.add_argument("-e", "--extensions", help="File extensions ES should display. Must be a single string between (double) quotes ex: '.bin .BIN .zip.ZIP'", type=str, required=True)
    parser.add_argument("-f", "--fullname", help="Sets the nice full name of the system. Defaults to the system name with a first upper case. ex: 'SEGA Master System'", type=str, required=False)
    parser.add_argument("-p", "--platform", help="Sets the system platform. Defaults to the system name. ex: pc", type=str, required=False)
    parser.add_argument("-t", "--theme", help="Sets the theme name. Defaults to the system name. ex: nes", type=str, required=False)
    parser.add_argument("-r", "--rompath", help="Sets the full rompath instead of /recalbox/share/roms/<system>. ex: /recalbox/share/screenshots", type=str, required=False)
    parser.add_argument("--force", help="force overwriting any existing files", action="store_true", required=False)
    parser.add_argument("--port", help="This system is a port, not a regular system", action="store_true", required=False)
    parser.add_argument("--readonly", help="This system is a port, and this port is ready-only (can stay in share_init)", action="store_true", required=False)
    parser.add_argument("packageDetails", nargs='+', help="Either specify a BR2_PACKAGE_XXXXX for a standalone emulator (like reicast, ppsspp etc ...)\nOr write it like libretro:mame2003:BR2_PACKAGE_LIBRETRO_MAME2003 libretro:mame2000:BR2_PACKAGE_LIBRETRO_MAME2000 advancemame:advancemame:BR2_PACKAGE_ADVANCEMAME for a multiple emulators/cores system. The syntax in that case is emulator:core:BUILDROOT_CORE_PACKAGE", type=str)

    args = parser.parse_args()

    ConfigEm = EmPack(args.system, args.extensions, args.packageDetails, fullname = args.fullname, platform = args.platform, theme = args.theme, force = args.force, port = args.port, readonly = args.readonly, rompath = args.rompath)
    print(ConfigEm)
    ConfigEm.writemakefile()
    ConfigEm.writeconfigin()
    ConfigEm.copyoverlaydir()
    ConfigEm.finalword()
