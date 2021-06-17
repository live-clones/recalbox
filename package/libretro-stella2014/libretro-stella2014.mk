################################################################################
#
# STELLA2014
#
################################################################################

LIBRETRO_STELLA2014_VERSION = ceed5a52549c313a12b4feb94c28411bdffb7612
LIBRETRO_STELLA2014_SITE = $(call github,libretro,stella2014-libretro,$(LIBRETRO_STELLA2014_VERSION))
LIBRETRO_STELLA2014_LICENSE = GPL-2.0

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_STELLA2014_PLATFORM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
LIBRETRO_STELLA2014_PLATFORM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
LIBRETRO_STELLA2014_PLATFORM=rpi1
else
LIBRETRO_STELLA2014_PLATFORM=$(RETROARCH_LIBRETRO_BOARD)
endif

define LIBRETRO_STELLA2014_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_STELLA2014_PLATFORM)"
endef

define LIBRETRO_STELLA2014_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/stella2014_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/stella2014_libretro.so
endef

$(eval $(generic-package))
