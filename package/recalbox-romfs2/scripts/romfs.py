#!/usr/bin/env python
import argparse
import os

from installroms import InstallRoms
from createsystem import CreateSystem
from systembuilder import SystemBuilder


class romfs:

    def __init__(self, args: argparse.Namespace):
        self.__root:str = os.path.realpath(args.systems + "/../../..")
        self.__args = args

    def execute(self):
        # Execute
        if self.__args.buildsystems is not None:
            SystemBuilder(self.__args.systems, self.__args.buildsystems, self.__root, self.__args.mergefrom).execute()
        if self.__args.installroms is not None:
            InstallRoms(self.__args.systems, self.__args.installroms, self.__root).execute()
        if self.__args.create is not None:
            CreateSystem(self.__args.systems, self.__args.create, self.__args.force).execute()


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='emulator-launcher script')
    parser.add_argument("-systems", help="System root folder", type=str, required=True)
    parser.add_argument("-force", help="force file overwriting when creating a new system", action="store_true", required=False)
    parser.add_argument("-mergefrom", help="systemlist.xml location to read before updating entries", type=str, required=False)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("-create", help="Create a new system", type=str)
    group.add_argument("-buildsystems", help="Build systemlist.xml", type=str)
    group.add_argument("-installroms", help="Install roms and readme files", type=str)
    arguments = parser.parse_args()

    fs = romfs(arguments)
    fs.execute()