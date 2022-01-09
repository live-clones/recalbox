# recalbox-rgb-dual

## EEPROM

Program the eeprom of Recalbox RGB Dual

Once programmed, the hat creates files in `/sys/firmware/devicetree/base/hat/` and the dtb is automatically loaded from the eeprom.

## Kernel module - based on cpasjuste rpi-dpidac

https://github.com/Cpasjuste/rpi-dpidac/

Building (cross compilation, for raspbian os)
- make -f Makefile.cross ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- KERNELDIR=/path/to/linux

## Resources
### DTS
https://www.embedded.com/linux-device-driver-development-the-pin-control-subsystem/
https://gist.github.com/0xff07/d286f45649a7e05c32c4523631bd15e0
https://github.com/raspberrypi/linux/blob/rpi-5.10.y/arch/arm/boot/dts/overlays/gpio-key-overlay.dts
https://www.programmerall.com/article/1048180876/
https://www.kernel.org/doc/html/latest/driver-api/gpio/board.html

### Kernel
https://github.com/raspberrypi/linux/blob/rpi-5.10.y/drivers/power/reset/gpio-poweroff.c