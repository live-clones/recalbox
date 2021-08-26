#!/usr/bin/env python
import os
import shutil
import unittest
import configgen.controllersConfig as controllersConfig

shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), "resources/es_input.cfg.origin")),
                os.path.abspath(os.path.join(os.path.dirname(__file__), "resources/es_input.cfg")))

# Injecting test es_input
controllersConfig.esInputs = os.path.abspath(os.path.join(os.path.dirname(__file__), "resources/es_input.cfg"))


class TestControllersConfig(unittest.TestCase):
    def test_load_all_controllers(self):
        controllers = controllersConfig.Controller.loadAllControllersConfig()
        self.assertEqual(34 * 2, len(controllers))

    def test_find_input_args(self):
        controllers = controllersConfig.Controller.loadAllControllersConfig()
        for x in controllers:
            print('x:{}\n'.format(x))
        controller = controllers['PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)-060000004c0500006802000000010000-6:1:20']
        self.assertEqual("button", controller.inputs["a"].type)
        self.assertEqual("axis", controller.inputs["joystick1up"].type)
        self.assertEqual("1", controller.inputs["a"].value)
        self.assertEqual("13", controller.inputs["a"].id)

    def test_associate_controllers_with_players_with_sameuuid(self):
        uuid = "060000004c0500006802000000010000"
        players = controllersConfig.Controller.loadControllerConfig("-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*")
        self.assertEqual(10, len(players))
        self.assertEqual(uuid, players["1"].guid)
        self.assertEqual(players["1"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["2"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["3"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["4"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["5"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["6"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["7"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["8"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["9"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["10"].realName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players["3"].configName, players["1"].configName)

    def test_associate_controllers_with_players_with_differentuuid(self):
        uuid1 = "060000004c0500006802000000010000"
        uuid2 = "030000005e0400009102000007010000"
        uuid3 = "030000005e0400008e02000014010000"
        uuid4 = "03000000b50700000399000000010000"
        uuid5 = "0000000058626f782047616d65706101"
        uuid6 = "03000000030000000300000002000000"
        uuid7 = "05000000172700004431000029010000"
        uuid8 = "03000000a30600000b04000000010000"
        uuid9 = "03000000a30600000cff000010010000"
        uuid10 = "030000003512000011ab000010010000"
        players = controllersConfig.Controller.loadControllerConfig("-1", uuid1, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                                    "-1", uuid2, "Xbox 360 Wireless Receiver (XBOX)", "", "*", "*", "*",
                                                                    "-1", uuid3, "Microsoft X-Box 360 pad", "", "*", "*", "*",
                                                                    "-1", uuid4, "Mega World", "", "*", "*", "*",
                                                                    "-1", uuid5, "Xbox Gamepad (userspace driver)", "", "*", "*", "*",
                                                                    "-1", uuid6, "Virtual gamepad", "", "*", "*", "*",
                                                                    "-1", uuid7, "XiaoMi Bluetooth Wireless GameController", "", "*", "*", "*",
                                                                    "-1", uuid8, "Saitek P990 Dual Analog Pad", "", "*", "*", "*",
                                                                    "-1", uuid9, "Saitek PLC Cyborg Force Rumble Pad", "", "*", "*", "*",
                                                                    "-1", uuid10, "FC30               FC30  Joystick", "", "*", "*", "*")
        self.assertEqual(10, len(players))
        self.assertEqual(uuid1, players["1"].guid)
        self.assertEqual(uuid2, players["2"].guid)
        self.assertEqual(uuid3, players["3"].guid)
        self.assertEqual(uuid4, players["4"].guid)
        self.assertEqual(uuid5, players["5"].guid)
        self.assertEqual(uuid6, players["6"].guid)
        self.assertEqual(uuid7, players["7"].guid)
        self.assertEqual(uuid8, players["8"].guid)
        self.assertEqual(uuid9, players["9"].guid)
        self.assertEqual(uuid10, players["10"].guid)
        self.assertEqual("13", players["1"].inputs["a"].id)

    def test_controllers_defaults(self):
        uuid1 = "060000004c0500006802000000010000"
        players = controllersConfig.Controller.loadControllerConfig("0", uuid1, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "6", "1", "20",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0",
                                                                    "-1", "FAKEUUID", "DEFAULTNAME", "", "0", "0", "0")

        self.assertEqual(1, len(players))
        self.assertEqual("0", players["1"].index)


if __name__ == '__main__':
    unittest.main()
