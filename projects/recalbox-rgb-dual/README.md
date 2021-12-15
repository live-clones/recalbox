# recalbox-rgb-dual

## EEPROM

Program the eeprom of Recalbox RGB Dual

Once programmed, the hat creates files in `/sys/firmware/devicetree/base/hat/` and the dtb is automatically loaded from the eeprom.

## Kernel module - based on cpasjuste rpi-dpidac

https://github.com/Cpasjuste/rpi-dpidac/

Building (cross compilation, for raspbian os)
- make -f Makefile.cross ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- KERNELDIR=/path/to/linux
