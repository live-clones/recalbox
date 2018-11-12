import recalboxFiles
from generators.Generator import Generator
import os.path
import glob
import sys
import shutil
import amiberryController
import sys
import amiberryConfig
import binascii
from settings.unixSettings import UnixSettings

mountPointWHDL = recalboxFiles.amiberryMountPoint + "/WHDL"
whdFilespath = recalboxFiles.BIOS + "/amiga/whdl"


def generateWHDL(fullName, romFolder, gameName, amigaHardware, controller):
    print("execute WHDLoad : <%s>" % os.path.join(romFolder, gameName))

    # ----- check Bios -----
    if not amiberryConfig.hasKickstarts(amigaHardware, whdl=True):
        raise IOError("No %s kickstarts found" % amigaHardware)

    amiberryConfig.initMountpoint(recalboxFiles.amiberryMountPoint)
    os.makedirs(mountPointWHDL)

    # ------------ copy WHDL structure Files ------------
    print("Copy WHDL bootstrap files from %s to %s" % (whdFilespath, mountPointWHDL))
    # TODO REDO IN PYTHON (not easily done)
    os.popen('cp -R ' + whdFilespath + '/* ' + mountPointWHDL)

    # ---- copy BIOS as equivalent into WHDL structure ----
    whdlKickstarts = os.path.join(mountPointWHDL, "Devs", "Kickstarts")
    shutil.copy2(os.path.join(recalboxFiles.BIOS, "kick13.rom"), os.path.join(whdlKickstarts, "kick33180.A500"))
    shutil.copy2(os.path.join(recalboxFiles.BIOS, "kick13.rom"), os.path.join(whdlKickstarts, "kick33192.A500"))
    shutil.copy2(os.path.join(recalboxFiles.BIOS, "kick13.rom"), os.path.join(whdlKickstarts, "kick34005.A500"))
    # shutil.copy2(os.path.join(recalboxFiles.BIOS,"kick20.rom"),os.path.join(whdlKickstart,))
    shutil.copy2(os.path.join(recalboxFiles.BIOS, "kick31.rom"), os.path.join(whdlKickstarts, "kick40068.A1200"))

    # ------------ copy game folder & uae ------------
    whdlDir = os.path.join(romFolder, gameName)
    whdlZip = whdlDir + ".zip"
    if os.path.exists(whdlZip):
        print("Unzip game folder %s" % whdlZip)
        os.popen('unzip "' + whdlZip + '" -d ' + mountPointWHDL)
    else:
        print("Copy game folder and uae %s" % whdlDir)
        # TODO REDO IN PYTHON (not easily done)
        os.popen('cp -R "' + whdlDir + '/"* ' + mountPointWHDL)
    shutil.copy2(whdlDir + ".uae", os.path.join(recalboxFiles.amiberryMountPoint, "amiberry", "conf", "uaeconfig.uae"))

    # ------------ Build reference ------------
    referenceFiles = {}
    buildReferenceFile(mountPointWHDL, referenceFiles)
    # for key, val in referenceFiles.items():
    #     print key, "=>", val

    # ------------ copy game saved files from previous sessions ------------
    handleBackupToGame(gameName, amigaHardware)

    # ------------ Complete UAE ----------------
    uaeConfig = os.path.join(recalboxFiles.amiberryMountPoint, "amiberry", "conf", "uaeconfig.uae")

    fUaeConfig = UnixSettings(uaeConfig, separator='', defaultComment=';')
    uaeConfigIsEmpty = os.path.getsize(uaeConfig) == 0

    # Needed or too speedy
    amiberryConfig.generateConfType(fUaeConfig)
    # Allow custom controllers conf in file
    if uaeConfigIsEmpty or not ';controls' in open(uaeConfig).read():
        amiberryController.generateControllerConf(fUaeConfig)

    amiberryController.generateSpecialKeys(fUaeConfig, controller)
    amiberryConfig.generateGUIConf(fUaeConfig, 'false')
    amiberryConfig.generateKickstartPathWHDL(fUaeConfig, amigaHardware)
    # Allow custom hardware conf in file
    if uaeConfigIsEmpty or not ';hardware' in open(uaeConfig).read():
        amiberryConfig.generateHardwareConf(fUaeConfig, amigaHardware)
    # Add Z3 Mem to load whole game in memory
    amiberryConfig.generateZ3Mem(fUaeConfig)
    if uaeConfigIsEmpty or not ';graphics' in open(uaeConfig).read():
        amiberryConfig.generateGraphicConf(fUaeConfig)
    amiberryConfig.generateSoundConf(fUaeConfig)
    generateHardDriveConf(fUaeConfig)

    # ------------ Create StartupSequence with right slave files ------------
    customLaunch = os.path.join(romFolder, gameName + ".whdl")
    gotAddedParams = os.path.exists(customLaunch) and not os.path.getsize(customLaunch) == 0

    fStartupSeq = open(os.path.join(mountPointWHDL, "S", "Startup-Sequence"), "a+")
    try:
        slaveFiles = [filename for filename in os.listdir(mountPointWHDL) if
                      filename.endswith(".Slave") or filename.endswith(".slave")]
        print(slaveFiles)
        if len(slaveFiles) == 0:
            raise IOError("This is not a valid WHD game")

        for slaveFile in slaveFiles:
            if gotAddedParams:
                addedParams = open(customLaunch, "r").readlines()[0].rstrip('\n\r ')
                print("Using slave file %s with custom params %s" % (slaveFile, addedParams))
                fStartupSeq.write("WHDload " + slaveFile + " Preload " + addedParams + "\n")
            else:
                print("Using slave file %s" % slaveFile)
                fStartupSeq.write("WHDload " + slaveFile + " Preload\n")

        # comment for now
        # fStartupSeq.write("exitemu\n")
    finally:
        fStartupSeq.close()

    return referenceFiles


def generateHardDriveConf(fUaeConfig):
    fUaeConfig.save("rtg_nocustom", "true")
    fUaeConfig.save("filesystem2", "rw,DH0:DH0:" + mountPointWHDL + "/,0")
    fUaeConfig.save("uaehf0", "dir,rw,DH0:DH0:" + mountPointWHDL + "/,0")


def handleBackupToGame(gameName, amigaHardware):
    saveDir = os.path.join(recalboxFiles.amiberrySaves, amigaHardware, gameName)
    if os.path.exists(saveDir):
        print("Copy saved files to game folder. From %s to %s" % (saveDir, mountPointWHDL))
        restoreModifiedFiles(saveDir, mountPointWHDL)
    else:
        print("No saved data.")


def handleBackupFromGame(fullName, romFolder, gameName, amigaHardware, reference):
    # ------------ clean WHDL structure Files before backup of backups ------------
    shutil.rmtree(os.path.join(mountPointWHDL, 'S'))
    shutil.rmtree(os.path.join(mountPointWHDL, 'C'))
    shutil.rmtree(os.path.join(mountPointWHDL, 'Devs'))

    # ------------ detect changes in remaining games files for backuping saves ------------
    saveDir = os.path.join(recalboxFiles.amiberrySaves, amigaHardware, gameName)
    romDir = os.path.join(romFolder, gameName)
    print("Backup changed files from %s to %s" % (mountPointWHDL, saveDir))
    backupModifiedFiles(mountPointWHDL, saveDir, reference)


def makedirectories(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def backupModifiedFiles(source, target, reference):
    print("backup dir <%s> to <%s>" %(source, target))
    for f in os.listdir(source):
        sourcePath = os.path.join(source, f)
        destinationPath = os.path.join(target, f)
        print ("backup file : %s/%s" %(source, f))
        if os.path.isdir(sourcePath):
            backupModifiedFiles(sourcePath, destinationPath, reference)
        else:
            if not sourcePath in reference:
                # if this is a new file, just copy
                print("new file : %s backup %s -> %s" % (f, source, target))
                # create target path if necessary
                if not os.path.isdir(target):
                    makedirectories(target)
                shutil.copy2(sourcePath, target)
            else:
                # file exists, compate datetime to detect any change
                datetime = int(os.path.getmtime(sourcePath))
                if datetime != reference[sourcePath]:
                    # create target path if necessary
                    if not os.path.isdir(target):
                        makedirectories(target)
                    print("changed file : %s backup %s -> %s" % (f, source, target))
                    shutil.copy2(sourcePath, target)


def restoreModifiedFiles(source, target):
    print("restore dir <%s> to <%s>" %(source, target))
    for f in os.listdir(source):
        sourcePath = os.path.join(source, f)
        destinationPath = os.path.join(target, f)
        print ("restore file : %s/%s" %(source, f))
        if os.path.isdir(sourcePath):
            restoreModifiedFiles(sourcePath, destinationPath)
        else:
            if not os.path.isdir(target):
                makedirectories(target)
            shutil.copy2(sourcePath, target)


def buildReferenceFile(source, referenceFiles):
    for f in os.listdir(source):
        sourcePath = os.path.join(source, f)
        if os.path.isdir(sourcePath):
            buildReferenceFile(sourcePath, referenceFiles)
        else:
            referenceFiles[sourcePath] = int(os.path.getmtime(sourcePath))


def CRC32_from_file(filename):
    buf = open(filename, 'rb').read()
    buf = (binascii.crc32(buf) & 0xFFFFFFFF)
    return "%08X" % buf
