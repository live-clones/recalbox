from configgen.controllers.controller import ControllerPerPlayer


class SolarusControllers:

    def __init__(self, controllers: ControllerPerPlayer):
        self.controllers = controllers

    def Start(self):
        for index, controller in self.controllers.items():
            if controller.HasStart:
                return controller.Start.Id
            return -1

    def HotKey(self):
        for index, controller in self.controllers.items():
            if controller.HasHotkey:
                return controller.Hotkey.Id
            return -1
