#!/usr/bin/env python

def get_all_pci_vendors_and_products():
    with open('/proc/bus/pci/devices', encoding="utf8") as file:
        devices_file = file.readlines()
        raw_id = [x.split('\t')[1] for x in devices_file]
        vend_and_dev = [(x[:4], x[4:]) for x in raw_id]
        return vend_and_dev
    return []

def find_nvidia_controller():
    pci = list(filter(lambda x: x[0] == "10de", get_all_pci_vendors_and_products()))
    if pci:
        return pci
    return ""
