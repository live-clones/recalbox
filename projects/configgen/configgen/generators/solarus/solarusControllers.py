class SolarusControllers:

    def __init__(self, controllers):
        self.controllers = controllers

    def Start(self):
        for index, controller in self.controllers.items():
            if "start" in controller.inputs:
                return controller.inputs["start"].id
            return "255"

    def HotKey(self):
        for index, controller in self.controllers.items():
            if "hotkey" in controller.inputs:
                return controller.inputs["hotkey"].id
            return "255"
