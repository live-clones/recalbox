#!/usr/bin/env python
import glob
import logging
import os
import re
import sys

from Nvidia.NvidiaInstaller import GpuError, NvidiaInstaller
import Utils.PciUtils

BLACKLIST_FILE = "/etc/modprobe.d/blacklist.conf"
NVIDIA_DRM_FILE = "/etc/modprobe.d/nvidia-drm.conf"

log = logging.getLogger(__name__)


def blacklist_nouveau():
    """
    add nouveau driver to blacklist
    this will disallow automatic loading of the driver
    """
    with open(BLACKLIST_FILE, "w", encoding="utf8") as file:
        file.write("blacklist nouveau")


def enable_nvidia_drm_modeset():
    """
    add nvidia-drm modeset parameter to 1
    this will enable DRM modesetting
    """
    with open(NVIDIA_DRM_FILE, "w", encoding="utf8") as file:
        file.write("options nvidia-drm modeset=1")


def unload_nouveau():
    """
    remove nouveau module if loaded
    """
    with open("/proc/modules", encoding="utf8") as file:
        for line in file.readlines():
            if re.search(r"^nouveau", line):
                unbind_nouveau()
                os.system("rmmod nouveau")


def unbind_nouveau():
    """make nouveau release the console
    so we can remove the module"""
    try:
        with open("/sys/class/vtconsole/vtcon1/bind", "w", encoding="utf8") as file:
            file.write("0")
    except FileNotFoundError as exc:
        log.warning("Can't unbind vtcon1: %s", exc)


def is_nouveau_blacklisted() -> bool:
    """
    test if nouveau module is blacklisted
    """
    try:
        with open(BLACKLIST_FILE, encoding="utf8") as file:
            for line in file.readlines():
                if line == "blacklist nouveau":
                    return True
        raise Exception("not blacklisted")
    except Exception as e:
        return False


def run_depmod():
    """
    execute depmod -a
    """
    os.system("depmod -a")


def restart_udev():
    """
    find udev service file and then
    stop and start udev
    """
    for root, dirs, files in os.walk("/etc/init.d"):
        for file in files:
            if re.search("udev", file):
                os.system(f"{root}/{file} stop")
                os.system(f"{root}/{file} start")


def unprotect_root():
    """
    remount root file system read/write
    """
    os.system("mount -o remount,rw /")


def protect_root():
    """
    remount root file system read only
    """
    os.system("mount -o remount,ro /")


# return the card id that was used to display boot sequence
def get_bootvga_card():
    bootvga = None
    cards = glob.glob("/sys/class/drm/card[0-9]")
    for card in cards:
        bootvga_path = os.path.join(card, "device/boot_vga")
        try:
            with open(bootvga_path, "r") as file:
                t = file.read()
                file.close()
                if t:
                    if int(t.strip()) > 0:
                        bootvga = card
                        break
        except:
            pass
    return bootvga


# return card vendor id from card id
def get_card_vendor(card):
    vendor_path = os.path.join(card, "device/vendor")
    with open(vendor_path, "r") as file:
        t = file.read()
        file.close()
        return t.strip()
    return ""


# if booted from Intel or Amd iGPU, return True
# else False
def has_booted_on_igpu():
    card = get_bootvga_card()
    if card is not None:
        vendor = get_card_vendor(card)
        log.info("Boot vga card is %s from vendor %s", card, vendor)
        if vendor in ["0x8086", "0x1002"]:
            return True
    return False


# effectively configure nvidia prime offloading
def configure_prime():
    os.system("/usr/bin/nvidia-xconfig --prime")


# test if prime is needed and enable it
def eventualy_configure_prime():
    if has_booted_on_igpu():
        log.info("Enable prime offloading")
        configure_prime()


def main(args):

    if not args.force and is_nouveau_blacklisted():
        # already blacklisted
        log.info("Already installed")
        sys.exit(0)

    installer = NvidiaInstaller(args.version)
    if args.search_path:
        installer.set_search_path(args.search_path)

    if len(Utils.PciUtils.find_nvidia_controller()) == 0:
        log.info("No Nvidia controller found, exiting...")
        sys.exit(0)

    for pci_card in Utils.PciUtils.find_nvidia_controller():
        pci_ids = iter(pci_card)
        vendor = next(pci_ids)
        product = next(pci_ids)
        log.info("Trying to install Nvidia controller %s:%s", vendor, product)

        try:
            unprotect_root()
            installer.install(f"0x{product}")
            # this part is reach if installer.install() found a supported nvidia controller
            eventualy_configure_prime()
            blacklist_nouveau()
            unload_nouveau()
            run_depmod()
            enable_nvidia_drm_modeset()
            restart_udev()
        except GpuError as exc:
            log.error("GPU error(%s): %s", exc.product_id, exc.message)
        except RuntimeError as exc:
            log.error("Can't install Nvidia drivers: %s", exc)
        except FileNotFoundError as exc:
            log.error("General error: %s", exc)
        finally:
            protect_root()
