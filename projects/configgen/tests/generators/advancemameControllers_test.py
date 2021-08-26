#!/usr/bin/env python

import shutil
import unittest
import configgen.generators.advancemame.advMameControllers as advMameControllers
import configgen.controllersConfig as controllersConfig


controllersConfig.esInputs = 'tests/resources/es_input.cfg'


class TestAdvancemameController(unittest.TestCase):
    def setUp(self):
        shutil.copyfile("tests/resources/es_input.cfg.origin", "tests/resources/es_input.cfg")
        self.controllers = controllersConfig.Controller.loadAllControllersConfig()

    def test_generateCombo(self):
        # fbaConf = fba2xConfig.createFBAConfig(fba2xNormal)
        controller = self.controllers['PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)-060000004c0500006802000000010000-6:1:20']
        controllerConfig = advMameControllers.getControllerConfig(controller)
        print("controllerConfig: {}".format(controllerConfig))
        self.assertEqual(controllerConfig, {'input_map[p0_button2]': 'joystick_button[-1,13]', 'input_map[p0_button1]': 'joystick_button[-1,14]', 'input_map[ui_select]': 'joystick_button[-1,14]', 'input_map[p0_down]': 'joystick_button[-1,6]', 'input_map[ui_down]': 'joystick_button[-1,6]', 'input_map[p0_left]': 'joystick_digital[-1,0,0,1] or joystick_button[-1,7]', 'input_map[p0_doubleleft_left]': 'joystick_digital[-1,0,0,1]', 'input_map[ui_left]': 'joystick_digital[-1,0,0,1] or joystick_button[-1,7]', 'input_map[p0_up]': 'joystick_digital[-1,0,1,1] or joystick_button[-1,4]', 'input_map[p0_doubleleft_up]': 'joystick_digital[-1,0,1,1]', 'input_map[ui_up]': 'joystick_digital[-1,0,1,1] or joystick_button[-1,4]', 'input_map[p0_doubleright_left]': 'joystick_digital[-1,0,2,1]', 'input_map[p0_doubleright_up]': 'joystick_digital[-1,0,3,1]', 'input_map[p0_button7]': 'joystick_button[-1,8]', 'input_map[p0_button9]': 'joystick_button[-1,1]', 'input_map[p0_button6]': 'joystick_button[-1,11]', 'input_map[p0_button5]': 'joystick_button[-1,10]', 'input_map[ui_configure]': 'joystick_button[-1,9]', 'input_map[p0_button10]': 'joystick_button[-1,2]', 'input_map[p0_right]': 'joystick_button[-1,5]', 'input_map[ui_right]': 'joystick_button[-1,5]', 'input_map[coin0]': 'joystick_button[-1,0]', 'input_map[start0]': 'joystick_button[-1,3]', 'input_map[p0_button4]': 'joystick_button[-1,12]', 'input_map[p0_button3]': 'joystick_button[-1,15]'})


if __name__ == '__main__':
    unittest.main()
