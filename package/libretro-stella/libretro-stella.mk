################################################################################
#
# STELLA
#
################################################################################
LIBRETRO_STELLA_VERSION = 6a8e20909e0a39a4e01f9102dc935113e38cd62b
LIBRETRO_STELLA_SITE = $(call github,libretro,stella-libretro,$(LIBRETRO_STELLA_VERSION))

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
	LIBRETRO_STELLA_PLATFORM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
	LIBRETRO_STELLA_PLATFORM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
	LIBRETRO_STELLA_PLATFORM=rpi1
else 
	LIBRETRO_STELLA_PLATFORM=$(LIBRETRO_BOARD)
endif

define LIBRETRO_STELLA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
	$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_STELLA_PLATFORM)"
endef

define LIBRETRO_STELLA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/stella_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/stella_libretro.so
endef

$(eval $(generic-package))
