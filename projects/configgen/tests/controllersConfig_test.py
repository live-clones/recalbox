#!/usr/bin/env python
import os
import shutil
import unittest
import configgen.controllers.controller as controllersConfig
from configgen.controllers.inputItem import InputItem

shutil.copyfile(os.path.abspath(os.path.join(os.path.dirname(__file__), "resources/es_input.cfg.origin")),
                os.path.abspath(os.path.join(os.path.dirname(__file__), "resources/es_input.cfg")))

# Injecting test es_input
controllersConfig.esInputs = os.path.abspath(os.path.join(os.path.dirname(__file__), "resources/es_input.cfg"))


class TestControllersConfig(unittest.TestCase):
    def test_load_all_controllers(self):
        controllers = controllersConfig.Controller.LoadControllerConfigurationFromFile()
        self.assertEqual(34 * 2, len(controllers))

    def test_find_input_args(self):
        controllers = controllersConfig.Controller.LoadControllerConfigurationFromFile()
        for x in controllers:
            print('x:{}\n'.format(x))
        controller = controllers['PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)-060000004c0500006802000000010000-6:1:20']
        self.assertEqual(InputItem.TypeButton, controller.A.Type)
        self.assertEqual(InputItem.TypeAxis, controller.Joy1Up.Type)
        self.assertEqual(1, controller.A.Value)
        self.assertEqual(13, controller.A.Id)

    def test_associate_controllers_with_players_with_sameuuid(self):
        uuid = "060000004c0500006802000000010000"
        players: controllersConfig.ControllerPerPlayer = controllersConfig.Controller.LoadUserControllerConfigurations(
            p1index =-1, p1guid =uuid, p1name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p1devicepath ="", p1nbaxes = -1, p1nbhats = -1, p1nbbuttons = -1,
            p2index =-1, p2guid =uuid, p2name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p2devicepath ="", p2nbaxes = -1, p2nbhats = -1, p2nbbuttons = -1,
            p3index =-1, p3guid =uuid, p3name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p3devicepath ="", p3nbaxes = -1, p3nbhats = -1, p3nbbuttons = -1,
            p4index =-1, p4guid =uuid, p4name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p4devicepath ="", p4nbaxes = -1, p4nbhats = -1, p4nbbuttons = -1,
            p5index =-1, p5guid =uuid, p5name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p5devicepath ="", p5nbaxes = -1, p5nbhats = -1, p5nbbuttons = -1,
            p6index =-1, p6guid =uuid, p6name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p6devicepath ="", p6nbaxes = -1, p6nbhats = -1, p6nbbuttons = -1,
            p7index =-1, p7guid =uuid, p7name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p7devicepath ="", p7nbaxes = -1, p7nbhats = -1, p7nbbuttons = -1,
            p8index =-1, p8guid =uuid, p8name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p8devicepath ="", p8nbaxes = -1, p8nbhats = -1, p8nbbuttons = -1,
            p9index =-1, p9guid =uuid, p9name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p9devicepath ="", p9nbaxes = -1, p9nbhats = -1, p9nbbuttons = -1,
            p10index=-1, p10guid=uuid, p10name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p10devicepath="", p10nbaxes= -1, p10nbhats= -1, p80nbbuttons= -1
        )
        self.assertEqual(10, len(players))
        self.assertEqual(uuid, players[1].GUID)
        self.assertEqual(players[1].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[2].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[3].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[4].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[5].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[6].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[7].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[8].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[9].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[10].DeviceName, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)")
        self.assertEqual(players[3].DeviceName, players[1].DeviceName)

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
        players: controllersConfig.ControllerPerPlayer =\
            controllersConfig.Controller.LoadUserControllerConfigurations(
                p1index ="-1", p1guid =uuid1 , p1name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p1devicepath ="", nb1axes ="*-1", nb1hats ="*-1", nb1buttons ="-1",
                p2index ="-1", p2guid =uuid2 , p2name ="Xbox 360 Wireless Receiver (XBOX)"             , p2devicepath ="", nb2axes ="*-1", nb2hats ="*-1", nb2buttons ="-1",
                p3index ="-1", p3guid =uuid3 , p3name ="Microsoft X-Box 360 pad"                       , p3devicepath ="", nb3axes ="*-1", nb3hats ="*-1", nb3buttons ="-1",
                p4index ="-1", p4guid =uuid4 , p4name ="Mega World"                                    , p4devicepath ="", nb4axes ="*-1", nb4hats ="*-1", nb4buttons ="-1",
                p5index ="-1", p5guid =uuid5 , p5name ="Xbox Gamepad (userspace driver)"               , p5devicepath ="", nb5axes ="*-1", nb5hats ="*-1", nb5buttons ="-1",
                p6index ="-1", p6guid =uuid6 , p6name ="Virtual gamepad"                               , p6devicepath ="", nb6axes ="*-1", nb6hats ="*-1", nb6buttons ="-1",
                p7index ="-1", p7guid =uuid7 , p7name ="XiaoMi Bluetooth Wireless GameController"      , p7devicepath ="", nb7axes ="*-1", nb7hats ="*-1", nb7buttons ="-1",
                p8index ="-1", p8guid =uuid8 , p8name ="Saitek P990 Dual Analog Pad"                   , p8devicepath ="", nb8axes ="*-1", nb8hats ="*-1", nb8buttons ="-1",
                p9index ="-1", p9guid =uuid9 , p9name ="Saitek PLC Cyborg Force Rumble Pad"            , p9devicepath ="", nb9axes ="*-1", nb9hats ="*-1", nb9buttons ="-1",
                p10index="-1", p10guid=uuid10, p10name="FC30               FC30  Joystick"             , p10devicepath="", nb10axes="*-1", nb10hats="*-1", nb10buttons="-1"
            )
        self.assertEqual(10, len(players))
        self.assertEqual(uuid1, players[1].GUID)
        self.assertEqual(uuid2, players[2].GUID)
        self.assertEqual(uuid3, players[3].GUID)
        self.assertEqual(uuid4, players[4].GUID)
        self.assertEqual(uuid5, players[5].GUID)
        self.assertEqual(uuid6, players[6].GUID)
        self.assertEqual(uuid7, players[7].GUID)
        self.assertEqual(uuid8, players[8].GUID)
        self.assertEqual(uuid9, players[9].GUID)
        self.assertEqual(uuid10, players[10].GUID)
        self.assertEqual(13, players[1].A.Id)

    def test_controllers_defaults(self):
        uuid1 = "060000004c0500006802000000010000"
        players: controllersConfig.ControllerPerPlayer = \
            controllersConfig.Controller.LoadUserControllerConfigurations(
                p1index =  0, p1guid =uuid1     , p1name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p1devicepath ="", p1nbaxes =6, p1nbhats =1, p1nbbuttons =20,
                p2index = -1, p2guid ="FAKEUUID", p2name ="DEFAULTNAME"                                   , p2devicepath ="", p2nbaxes =0, p2nbhats =0, p2nbbuttons =0,
                p3index = -1, p3guid ="FAKEUUID", p3name ="DEFAULTNAME"                                   , p3devicepath ="", p3nbaxes =0, p3nbhats =0, p3nbbuttons =0,
                p4index = -1, p4guid ="FAKEUUID", p4name ="DEFAULTNAME"                                   , p4devicepath ="", p4nbaxes =0, p4nbhats =0, p4nbbuttons =0,
                p5index = -1, p5guid ="FAKEUUID", p5name ="DEFAULTNAME"                                   , p5devicepath ="", p5nbaxes =0, p5nbhats =0, p5nbbuttons =0,
                p6index = -1, p6guid ="FAKEUUID", p6name ="DEFAULTNAME"                                   , p6devicepath ="", p6nbaxes =0, p6nbhats =0, p6nbbuttons =0,
                p7index = -1, p7guid ="FAKEUUID", p7name ="DEFAULTNAME"                                   , p7devicepath ="", p7nbaxes =0, p7nbhats =0, p7nbbuttons =0,
                p8index = -1, p8guid ="FAKEUUID", p8name ="DEFAULTNAME"                                   , p8devicepath ="", p8nbaxes =0, p8nbhats =0, p8nbbuttons =0,
                p9index = -1, p9guid ="FAKEUUID", p9name ="DEFAULTNAME"                                   , p9devicepath ="", p9nbaxes =0, p9nbhats =0, p9nbbuttons =0,
                p10index= -1, p10guid="FAKEUUID", p10name="DEFAULTNAME"                                   , p10devicepath="", p10nbaxes=0, p10nbhats=0, p10nbbuttons=0,
            )

        self.assertEqual(1, len(players))
        self.assertEqual(0, players[1].SdlIndex)


if __name__ == '__main__':
    unittest.main()
