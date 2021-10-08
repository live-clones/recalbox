#!/usr/bin/env python

from pprint import pprint
import shutil
import configgen.controllers.controller as controllersConfig
from configgen.generators.openbor.openborControllers import OpenborControllers
from configgen.settings.keyValueSettings import keyValueSettings
import pytest


controllersConfig.esInputs = "tests/resources/es_input.cfg"


@pytest.fixture
def openbor_controller_init(self):
    shutil.copyfile(
        "tests/resources/es_input.cfg.origin", "tests/resources/es_input.cfg"
    )
    self.controllers = (
        controllersConfig.Controller.LoadControllerConfigurationFromFile()
    )


@pytest.fixture
def controller_configuration():
    return controllersConfig.Controller.LoadUserControllerConfigurations(
        p1index=0,
        p1guid="060000004c0500006802000000010000",
        p1name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)",
        p1devicepath="",
        p1nbaxes=-1,
        p1nbhats=-1,
        p1nbbuttons=-1,
    )


@pytest.fixture
def settings():
    return keyValueSettings("/tmp/dumb")


def test_openborControllers(controller_configuration, settings):
    controllers = OpenborControllers(controller_configuration)
    controllers.addControllers(settings)
    assert (
        settings.getInt("JoystickButton-P0-MOVEDOWN", "undefined-not-good") == 607
        and settings.getInt("JoystickButton-P0-MOVEUP", "undefined-not-good") == 605
        and settings.getInt("JoystickButton-P0-MOVELEFT", "undefined-not-good") == 608
        and settings.getInt("JoystickButton-P0-MOVERIGHT", "undefined-not-good") == 606
        and settings.getInt("JoystickButton-P0-ATTACK", "undefined-not-good") == 614
        and settings.getInt("JoystickButton-P0-ATTACK2", "undefined-not-good") == 615
        and settings.getInt("JoystickButton-P0-ATTACK3", "undefined-not-good") == 613
        and settings.getInt("JoystickButton-P0-ATTACK4", "undefined-not-good") == 616
        and settings.getInt("JoystickButton-P0-HOTKEY", "undefined-not-good") == 617
        and settings.getInt("JoystickButton-P0-JUMP", "undefined-not-good") == 611
    )
