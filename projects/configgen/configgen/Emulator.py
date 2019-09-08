import recalboxFiles
from settings.keyValueSettings import keyValueSettings


class Emulator:

    def __init__(self, name, emulator, core='', videomode='CEA 4 HDMI', shaders='', ratio='auto', smooth='1', rewind='0', configfile=None, showFPS=None):
        self.name = name
        self.config = dict()
        self.config['videomode']  = videomode
        self.config['core']       = core
        self.config['emulator']   = emulator
        self.config['shaders']    = shaders
        self.config['ratio']      = ratio
        self.config['smooth']     = smooth
        self.config['rewind']     = rewind
        self.config['configfile'] = configfile
        self.config['netplay']    = None
        self.config['showFPS']    = showFPS
        self.config['args']       = None
        self.config['hash']       = None

    def configure(self, recalboxSettings, emulator='default', core='default', ratio='auto', netplay=None, netplay_ip=None, netplay_port=None, hash_=None, extra=None):
        self.config['specials'] = recalboxSettings.getOption('system.emulators.specialkeys', 'default')
        self.config['netplay_nickname'] = recalboxSettings.getOption('global.netplay.nickname', 'default')
        self.config['netplaymode'] = netplay
        self.config['netplay_ip'] = netplay_ip
        self.config['netplay_port'] = netplay_port
        self.config['hash'] = hash_
        self.config['extra'] = extra
        self.updateConfiguration(recalboxSettings.getOptionSubset("global."), recalboxSettings)
        self.updateConfiguration(recalboxSettings.getOptionSubset("{}.".format(self.name)), recalboxSettings)
        self.updateForcedConfig(emulator, core, ratio)

    def updateConfiguration(self, settings, recalboxSettings):
        systemSettings = self.config

        # Special case of auto ratio
        if settings.get('ratio') == 'auto':
            del settings['ratio']
        if settings.get('emulator') == 'default':
            del settings['emulator']
        if settings.get('core') == 'default':
            del settings['core']
        systemSettings.update(settings)

        # ShaderSets
        if settings.get('shaderset', '') != '':
            self.updateShaders(settings['shaderset'])

        # Draw FPS
        if self.config.get('showFPS') not in ['false', 'true']:
            self.config['showFPS'] = False
            self.updateDrawFPS()

        # Optionnal emulator args ONLY if security is disabled
        security = recalboxSettings.getOption("system.security.enabled", '0')
        if security != '1' and settings.get('args', '') != '':
            shlex = __import__("shlex")
            self.config['args'] = shlex.split(settings['args'])
        else:
            self.config['args'] = None

    def updateDrawFPS(self):
        try:
            import xml.etree.ElementTree as ET
            esConfig = ET.parse(recalboxFiles.esSettings)
            value = esConfig.find("./bool[@name='DrawFramerate']").attrib["value"]
        except:
            value = 'false'
        if value not in ['false', 'true']:
            value = 'false'
        self.config['showFPS'] = value

    def updateShaders(self, shaderSet):
        if shaderSet is not None and shaderSet != 'none':
            shaderFile = recalboxFiles.shaderPresetRoot + '/' + shaderSet + '.cfg'
            shaderContent = keyValueSettings(shaderFile, False)
            shaderContent.loadFile(True)
            systemShader = shaderContent.getOption(self.name, None)
            if systemShader is not None:
                self.config['shaders'] = systemShader

    def updateForcedConfig(self, emulator, core, ratio):
        if emulator is not None and emulator != 'default':
            self.config['emulator'] = emulator
        if core is not None and core != 'default':
            self.config['core'] = core
        if ratio is not None and ratio != 'auto':
            self.config['ratio'] = ratio

