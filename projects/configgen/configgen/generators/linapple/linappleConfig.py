"""
Created on Mar 6, 2016

@author: Laurent Marchelli
"""
from typing import Dict, Tuple, List

from configgen.Emulator import Emulator
from configgen.controllers.inputItem import InputItem
from configgen.controllers.controller import ControllerPerPlayer, Controller

joystick_translator: Dict[str, Tuple[int]] = \
{
    # linapple : recalboxOS
    'Joy0Axis0' :   ( InputItem.ItemJoy1Left, InputItem.ItemJoy2Left, InputItem.ItemLeft ),
    'Joy0Axis1' :   ( InputItem.ItemJoy1Up, InputItem.ItemJoy2Up, InputItem.ItemUp ),
    'Joy0Button1' : ( InputItem.ItemA, InputItem.ItemX ),
    'Joy0Button2' : ( InputItem.ItemB, InputItem.ItemY ),
    'Joy1Axis0' :   ( InputItem.ItemJoy1Left, InputItem.ItemJoy2Left, InputItem.ItemLeft ),
    'Joy1Axis1':    ( InputItem.ItemJoy1Up, InputItem.ItemJoy2Up, InputItem.ItemUp ),
    'Joy1Button1':  ( InputItem.ItemL1, ),
    'Joy1Button2':  ( InputItem.ItemR1, ),
}


class LinappleConfig:

    globalResolutionFile = "/sys/class/graphics/fb0/virtual_size"

    @staticmethod
    def setResolutionFile(resolutionFile: str):
        LinappleConfig.globalResolutionFile = resolutionFile

    """
    Class managing linapple emulator configuration file.

    Customize the linappple configuration file (linapple.conf) with devices
    informations provided by configgen classes.

    Args:
        filename (str):
            Configuration path and filename.

    """
    def __init__(self, filename: str):
        self.settings: Dict[str, str] = {}
        self.filename = filename
        self.load(filename)

    def load(self, filename=None):
        """
        Load settings from the requested configuration file.

        Args:
            filename (str, None):
                Configuration path and file name.
                If None provided, self.filename is used.

        Returns (None):

        Notes:
            Commented and blank lines are stripped from the extraction.
        """
        import re
        patten = re.compile(r"^\s*(?P<name>\w+( \w+)*)\s*="
                            r"\s*(?P<value>[^\s]*).*$")
        with open(filename, 'r' ) as lines:
            for l in lines:
                m = patten.match(l)
                if m : self.settings[m.group('name')] = m.group('value')


    def save(self, filename=None):
        """
        Save all settings into a configuration file.

        Settings are written in alphabetical order in the configuration
        file using the linapple parameters convention.
        (see : Registry.cpp funct RegSaveKeyValue)

        Args:
            filename (str, None):
                Configuration path and file name.
                If None provided, self.filename is used.

        Returns (None):
        """
        if filename:
            self.filename = filename
        else:
            filename = self.filename
        
        with open(filename, 'w') as file_out:
            for k, v in sorted(self.settings.items()):
                file_out.write('\t{}=\t{}\n'.format(k,v))

    def joysticks(self, controllers: ControllerPerPlayer):
        """
        Configure linapple joysticks (2) with given controllers.

        Args:
            controllers (dict):
                Dictionary of controllers connected (1 to 5).
        """
        # Disable both joysticks
        self.settings['Joystick 0'] = '0'
        self.settings['Joystick 1'] = '0'
        
        # Configure axis and buttons for first two available joysticks
        sortedKeys = sorted(controllers.keys())
        joysticks: ControllerPerPlayer = {}
        if len(sortedKeys) != 0: joysticks[sortedKeys[0]] = controllers[sortedKeys[0]]
        if len(sortedKeys) > 0 : joysticks[sortedKeys[1]] = controllers[sortedKeys[1]]
        # Strange button behaviour with 2 joysticks enabled :-( TBD
        # joysticks = sorted(controllers.items())[:2] 
        for counter, (playerIndex, controller) in enumerate(joysticks.items()):
            assert(playerIndex == controller.PlayerIndex)   # Just if Controller source code changed
            
            # Use the translator to get recalbox value from linapple keyword 
            joy_i = 'Joy{}'.format(counter)
            transl: List[Tuple[str, Tuple[int]]] = [(linKey, items) for (linKey, items) in joystick_translator.items() if linKey.startswith(joy_i)]
            for linKey, items in transl:
                for item in items:
                    if controller.HasInput(item):
                        inputItem = controller.Input(item)
                        if linKey.startswith(joy_i + 'Axis'):
                            if not inputItem.IsAxis: continue
                        else:
                            if not inputItem.IsButton: continue
                        self.settings[linKey] = str(inputItem.Id)
                        break

            # Enable Joystick
            self.settings['Joy{}Index'.format(counter)] = str(controller.SdlIndex)
            self.settings['Joystick {}'.format(counter)] = '1'
            
        # Configure extended buttons information
        if self.settings['Joystick 0'] == '1':
            controller: Controller = joysticks[sortedKeys[0]]
            if controller.HasHotkey: self.settings['JoyExitButton0'] = str(controller.Hotkey.Id)
            if controller.HasStart : self.settings['JoyExitButton1'] = str(controller.Start.Id)
            self.settings['JoyExitEnable'] = '1' if controller.HasHotkey and controller.HasStart else '0'
    
    def system(self, system: Emulator, filename: str):
        """
        Configure Recalbox system settings

        Args:
            system (Emulator):
                Emulator object containing a config dictionay with all
                parameters set in EmulationStation.
            filename (str):
                Path and filename of the current disk.
        """

        if filename:
            self.settings['Boot at Startup'] = '1'
            self.settings['Disk Image 1'] = filename
            self.settings['Slot 6 Autoload'] = '1'
        else:
            self.settings['Boot at Startup'] = '0'
            self.settings['Disk Image 1'] = ''
            self.settings['Slot 6 Autoload'] = '0'
        
        if filename and system.AutoSave:
            import os
            name = os.path.join(self.settings['Save State Directory'],
                os.path.splitext(os.path.split(filename)[1])[0] + '.sve')
            self.settings['Save State Filename'] = name
            self.settings['Save State On Exit'] = '1'
        else:
            self.settings['Save State Filename'] = ''
            self.settings['Save State On Exit'] = '0'

        # Screen resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        self.settings['Fullscreen'] = '1'
        if resolution.isSet and resolution.selfProcess:
            self.settings['Screen Width'] = str(resolution.width)
            self.settings['Screen Height'] = str(resolution.height)
        else:
            xy = "800,600"
            try:
                with open(LinappleConfig.globalResolutionFile, "r") as f:
                    xy = f.read()
            except Exception as ex:
                print("Can't read resolution: {}".format(ex))

            (x, y) = xy.split(',')
            if x == 320 and y == 480: # Hard-patch for GOA2
                x = 480
                y = 320
            print("Final resolution: {}x{}".format(x, y))
            self.settings['Screen Width'] = x
            self.settings['Screen Height'] = y


# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
