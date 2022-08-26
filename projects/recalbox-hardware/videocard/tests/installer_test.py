#!/usr/bin/env python
import pytest
import Nvidia.installer

from mock_open import MockOpen
from pprint import pprint

def test_blacklist_nouveau(mocker):
    mocker.patch("builtins.open", mocker.mock_open())

    Nvidia.installer.blacklist_nouveau()

    open.assert_called_once_with("/etc/modprobe.d/blacklist.conf", "w", encoding="utf8")
    open().write.assert_called_once_with("blacklist nouveau")


def test_unload_nouveau(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data="nouveau blah blah\nblah nouveau blah"))
    mocker.patch("Nvidia.installer.unbind_nouveau")
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.unload_nouveau()
# example to debug mocker
#    pprint(open.mock_calls)
#    pprint(instaler.os.system.mock_calls)
    Nvidia.installer.os.system.assert_called_once_with("rmmod nouveau")


def test_unload_nouveau_not_loaded(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data="blah x blah"))
    mocker.patch("Nvidia.installer.unbind_nouveau")
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.unload_nouveau()

    Nvidia.installer.os.system.assert_not_called()


def test_unbind_nouveau(mocker):
    mocker.patch("builtins.open", mocker.mock_open())

    Nvidia.installer.unbind_nouveau()

    open.assert_called_once_with("/sys/class/vtconsole/vtcon1/bind", "w", encoding="utf8")
    open().write.assert_called_once_with("0")


def test_is_nouveau_blacklisted_false(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data="not blacklisted"))

    assert Nvidia.installer.is_nouveau_blacklisted() == False

    
def test_is_nouveau_blacklisted_true(mocker):
    mocker.patch("builtins.open", mocker.mock_open(read_data="blacklist nouveau"))

    assert Nvidia.installer.is_nouveau_blacklisted() == True

    
def test_is_nouveau_blacklisted_error(mocker):
    mo = MockOpen()
    mo[Nvidia.installer.BLACKLIST_FILE].side_effect = IOError("no such file or directory")
    mocker.patch("builtins.open", mo)

    assert Nvidia.installer.is_nouveau_blacklisted() == False


def test_run_depmod(mocker):
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.run_depmod()

    Nvidia.installer.os.system.assert_called_once_with("depmod -a")


def test_restart_udev(mocker):
    mocker.patch("Nvidia.installer.os.walk", return_value=[("/etc/init.d", "", ["S01udev"])])
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.restart_udev()

    Nvidia.installer.os.system.assert_any_call("/etc/init.d/S01udev stop")
    Nvidia.installer.os.system.assert_any_call("/etc/init.d/S01udev start")


def test_restart_udev_no_service_file(mocker):
    mocker.patch("Nvidia.installer.os.walk", return_value=[("/etc/init.d", "", ["S01something-else"])])
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.restart_udev()

    Nvidia.installer.os.system.assert_not_called()


def test_unprotect_root(mocker):
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.unprotect_root()

    Nvidia.installer.os.system.assert_called_once_with("mount -o remount,rw /")


def test_protect_root(mocker):
    mocker.patch("Nvidia.installer.os.system")

    Nvidia.installer.protect_root()

    Nvidia.installer.os.system.assert_called_once_with("mount -o remount,ro /")
