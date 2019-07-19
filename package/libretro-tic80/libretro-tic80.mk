################################################################################
#
# TIC-80
#
################################################################################

LIBRETRO_TIC80_VERSION = 35d8e98e959f847481c47ad8e50adc70ef15bb66
LIBRETRO_TIC80_SITE = $(call github,nesbox,TIC-80,$(LIBRETRO_TIC80_VERSION))
LIBRETRO_TIC80_LICENSE = MIT

LIBRETRO_TIC80_CONF_OPTS=-DBUILD_SDL=0 \
						-DBUILD_SOKOL=0 \
						-DBUILD_DEMO_CARTS=0 \
                        -DBUILD_LIBRETRO=1 \
                        -DBUILD_PRO=1

define LIBRETRO_TIC80_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/libretro
	$(INSTALL) -D $(@D)/bin/tic80_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/tic80_libretro.so
endef

$(eval $(cmake-package))