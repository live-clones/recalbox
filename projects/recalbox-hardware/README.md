# recalbox-hardware

## Hardware management

The purpose of the lib is to automatically detect and or configure hardwares connected to the different boards.
Main python scripts *must* run fast. Please write short and fast code. Use module dynamic loading wherever it is possible/efficient

### Cases

Here is the part that auto-detect and/or configure cases with hardware parts, starting with the GPi.

compile:
```
python -m compilall .
```

How to start:
```
python case/manage.pyc "<casename>" <install> <machine> <phase>
with:
- casename : Case to install/uninstall. Empty to autodetect.
- install  : 1 = install, 0 = uninstall
- machine  : 1 = machine request, 0 = user request
- phase    : 0 = install hardware, 1 = install software
```
