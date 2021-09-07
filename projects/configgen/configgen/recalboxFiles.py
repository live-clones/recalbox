#!/usr/bin/env python
HOME_INIT = '/recalbox/share_init/system/'
HOME = '/recalbox/share/system'
CONF = HOME + '/configs'
SAVES = '/recalbox/share/saves'
SAVES_INIT = '/recalbox/share_init/saves'
SCREENSHOTS = '/recalbox/share/screenshots'
BIOS = '/recalbox/share/bios'
BIOS_INIT = '/recalbox/share_init/bios'
OVERLAYS = '/recalbox/share/overlays'
RECALBOX_OVERLAYS = '/recalbox/share_init/overlays'
ROMS = '/recalbox/share/roms'

esInputs = HOME + '/.emulationstation/es_input.cfg'
esSettings = HOME + '/.emulationstation/es_settings.cfg'
recalboxConf = HOME + '/recalbox.conf'
logdir = HOME + '/logs/'

esLightGun = HOME_INIT + '/.emulationstation/lightgun.cfg'
GameListFileName = "gamelist.xml"

# This dict is indexed on the emulator name, not on the system
recalboxBins =\
{
    'advancemame' : '/usr/bin/advmame',
    'amiberry'    : '/usr/bin/amiberry',
    'daphne'      : '/usr/bin/hypseus',
    'dolphin'     : '/usr/bin/dolphin-emu',
    'dosbox'      : '/usr/bin/dosbox',
    'fba2x'       : '/usr/bin/fba2x',
    'gsplus'      : '/usr/bin/GSplus',
    'hatari'      : '/usr/bin/hatari',
    'kodi'        : '/recalbox/scripts/kodilauncher.sh',
    'libretro'    : '/usr/bin/retroarch',
    'linapple'    : '/usr/bin/linapple',
    'moonlight'   : '/usr/bin/moonlight',
    'mupen64plus' : '/usr/bin/mupen64plus',
    'openbor'     : '/usr/bin/OpenBOR',
    'oricutron'   : '/usr/bin/oricutron/oricutron',
    'pcsx_rearmed': '/usr/bin/pcsx_rearmed',
    'pisnes'      : '/usr/bin/pisnes',
    'ppsspp'      : '/usr/bin/PPSSPPSDL',
    'reicast'     : '/usr/bin/reicast.elf',
    'scummvm'     : '/usr/bin/scummvm',
    'simcoupe'    : '/usr/bin/simcoupe',
    'solarus'     : '/usr/bin/solarus-run',
    'supermodel'  : '/usr/bin/supermodel',
    'ti99sim'     : '/usr/bin/ti99sim/ti99sim-sdl',
    'vice'        : '/usr/bin/x64',
    'xroar'       : '/usr/bin/xroar',
}


retroarchRoot = CONF + '/retroarch'
retroarchCustom = retroarchRoot + '/retroarchcustom.cfg'
retroarchCustomOrigin = retroarchRoot + "/retroarchcustom.cfg.origin"
retroarchCoreCustom = retroarchRoot + "/cores/retroarch-core-options.cfg"
retroarchInitCustomOrigin = HOME_INIT + "configs/retroarch/retroarchcustom.cfg.origin"

retroarchCores = "/usr/lib/libretro/"
shadersRoot = "/recalbox/share/shaders/"
shadersExt = '.gplsp'
libretroExt = '_libretro.so'
screenshotsDir = "/recalbox/share/screenshots/"
savesDir = "/recalbox/share/saves/"

fbaRoot = CONF + '/fba/'
fbaCustom = fbaRoot + 'fba2x.cfg'
fbaCustomOrigin = fbaRoot + 'fba2x.cfg.origin'


mupenConf = CONF + '/mupen64/'
mupenCustom = mupenConf + "mupen64plus.cfg"
mupenInput = mupenConf + "InputAutoCfg.ini"
mupenSaves = SAVES + "/n64"
mupenMappingUser    = mupenConf + 'input.xml'
mupenMappingSystem  = '/recalbox/share_init/system/configs/mupen64/input.xml'

shaderPresetRoot = "/recalbox/share_init/system/configs/shadersets/"

kodiJoystick = HOME + '/.kodi/userdata/addon_data/peripheral.joystick/resources/buttonmaps/xml/linux/'
kodiMappingUser    = CONF + '/kodi/input.xml'
kodiMappingSystem  = '/recalbox/share_init/system/configs/kodi/input.xml'


moonlightCustom = CONF+'/moonlight'
moonlightConfig = moonlightCustom + '/moonlight.conf'
moonlightGamelist = moonlightCustom + '/gamelist.txt'
moonlightMapping = dict()
moonlightMapping[1] = moonlightCustom + '/mappingP1.conf'
moonlightMapping[2] = moonlightCustom + '/mappingP2.conf'
moonlightMapping[3] = moonlightCustom + '/mappingP3.conf'
moonlightMapping[4] = moonlightCustom + '/mappingP4.conf'
moonlightMapping[5] = moonlightCustom + '/mappingP5.conf'
moonlightMapping[6] = moonlightCustom + '/mappingP6.conf'
moonlightMapping[7] = moonlightCustom + '/mappingP7.conf'
moonlightMapping[8] = moonlightCustom + '/mappingP8.conf'
moonlightMapping[9] = moonlightCustom + '/mappingP9.conf'
moonlightMapping[10] = moonlightCustom + '/mappingP10.conf'

reicastCustom = CONF + '/reicast'
reicastConfig = reicastCustom + '/emu.cfg'
reicastConfigInit = HOME_INIT + 'configs/reicast/emu.cfg'
reicastSaves = SAVES + '/dreamcast'
reicastBios = BIOS

dolphinConfig  = CONF + "/dolphin-emu"
dolphinData    = SAVES + "/dolphin-emu"
dolphinIni     = dolphinConfig + '/Dolphin.ini'
dolphinHKeys   = dolphinConfig + '/Hotkeys.ini'
dolphinGFX     = dolphinConfig + '/GFX.ini'
dolphinSYSCONF = dolphinData + '/Wii/shared2/sys/SYSCONF'

ppssppConf = CONF + '/ppsspp/PSP/SYSTEM'
ppssppControlsIni = ppssppConf + '/controls.ini'
ppssppControls = CONF + '/ppsspp/gamecontrollerdb.txt'
ppssppControlsInit = HOME_INIT + 'configs/ppsspp/PSP/SYSTEM/controls.ini'
ppssppConfig = ppssppConf + '/ppsspp.ini'
ppssppSaves = SAVES + '/psp'

dosboxCustom = CONF + '/dosbox'
dosboxConfig = dosboxCustom + '/dosbox.conf'

scummvmSaves = SAVES + '/scummvm'

simcoupeConfig = HOME + '/.simcoupe/SimCoupe.cfg'

viceConfig = CONF + "/vice/vice.conf"

advancemameConfig = CONF + '/advancemame/advmame.rc'
advancemameConfigOrigin = CONF + '/advancemame/advmame.rc.origin'

amiberryMountPoint = '/tmp/amiga'
amiberrySaves      = SAVES

daphneInputIni = CONF + '/daphne/dapinput.ini'
daphneHomedir = ROMS + '/daphne'
daphneDatadir = '/usr/share/daphne'

oricutronConfig = HOME + '/.config/oricutron.cfg'

openborConfig = HOME + '/configs/openbor/default.cfg'
openborConfigOrigin = HOME + '/configs/openbor/default.cfg.origin'

gsplusConfig = HOME + '/.config/gsplus.cfg'

atari800CustomConfig = HOME + '/.atari800.cfg'

hatariCustomConfig = HOME + '/.hatari/hatari.cfg'

pcsxRootFolder = '/recalbox/share/system/configs/pcsx'
pcsxConfigFile = pcsxRootFolder + '/pcsx.cfg'

pisnesRootFolder = '/recalbox/share/system/configs/pisnes'
pisnesConfigFile = pisnesRootFolder + '/snes9x.cfg'

supermodelRooFolder = CONF + '/model3'
supermodelConfigFile = supermodelRooFolder + '/ConfigModel3.ini'
supermodelControlsIni = supermodelRooFolder + '/Supermodel.ini'
