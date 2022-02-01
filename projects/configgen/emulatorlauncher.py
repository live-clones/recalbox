#!/bin/python
import site

exec(open(f"{site.getsitepackages()[0]}/configgen/emulatorlauncher.py").read())
