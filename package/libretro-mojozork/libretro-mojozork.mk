################################################################################
#
# libretro-mojozork
#
################################################################################

# Version: 2022-12-12
LIBRETRO_MOJOZORK_VERSION = 22b638ccd0948d46b7902d45387caaccef35b38a
LIBRETRO_MOJOZORK_SITE = $(call github,icculus,mojozork,$(LIBRETRO_MOJOZORK_VERSION))
LIBRETRO_MOJOZORK_LICENSE = zlib
LIBRETRO_MOJOZORK_LICENSE_FILE = LICENSE.txt
LIBRETRO_MOJOZORK_DEPENDENCIES = retroarch

LIBRETRO_MOJOZORK_CONF_OPTS += \
	-DCMAKE_BUILD_TYPE=Release \
	-DLIBRETRO=ON

define LIBRETRO_MOJOZORK_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mojozork_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mojozork_libretro.so
endef

$(eval $(cmake-package))
