import recalboxFiles
from generators.Generator import Generator
import os.path
import glob
import sys
import shutil
import amiberryController
import amiberryConfig
from settings.unixSettings import UnixSettings

def generateCD(fullName,romPath,uaeName,amigaHardware,controller) :
    # TODO Also allow to use Amiga CD ?
    
    # ----- check Bios -----
    if not amiberryConfig.hasCD32Kickstarts() :
        raise IOError("No CD32 kickstarts found")
    
    print("execute CD32 : <%s> on <%s>" %("/amiberry",romPath + "/" + uaeName))
    
    amiberryConfig.initMountpoint(recalboxFiles.amiberryMountPoint)
    
    # ----- Create uae configuration file -----
    uaeconfig = os.path.join(recalboxFiles.amiberryMountPoint,"amiberry","conf","uaeconfig.uae")
    
    if os.path.exists(uaeconfig) :
        os.remove(uaeconfig)
    
    fUaeConfig = UnixSettings(uaeconfig, separator='', defaultComment=';')
    amiberryController.generateCD32ControllerConf(fUaeConfig,controller)
    amiberryConfig.generateGUIConf(fUaeConfig,"false")
    amiberryConfig.generateKickstartPathCD32(fUaeConfig,amigaHardware)
    amiberryConfig.generateHardwareConf(fUaeConfig,amigaHardware)
    generateCD32Conf(fUaeConfig,romPath,uaeName)
    amiberryConfig.generateCD32GraphicConf(fUaeConfig)
    
def generateCD32Conf(fUaeConfig,romPath,uaeName) :
    fUaeConfig.save("cdimage0",os.path.join(romPath,uaeName)+",image")
