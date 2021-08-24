################################################################################
#
# MGBA
#
################################################################################

# Version of 22/08/2021
LIBRETRO_MGBA_VERSION = 5ba012e530b81aa27b237bf646b4e0e37e3dadf9
LIBRETRO_MGBA_SITE = $(call github,libretro,mgba,$(LIBRETRO_MGBA_VERSION))
LIBRETRO_MGBA_LICENSE = MPL-2.0
LIBRETRO_MGBA_LICENSE_FILES = LICENSE

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_MGBA_NEON += "HAVE_NEON=1"
else
LIBRETRO_MGBA_NEON += "HAVE_NEON=0"
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_MGBA_PLATFORM=rpi3
else
LIBRETRO_MGBA_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
endif

define LIBRETRO_MGBA_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(LIBRETRO_MGBA_PLATFORM)" $(LIBRETRO_MGBA_NEON)
endef

define LIBRETRO_MGBA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mgba_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mgba_libretro.so
endef

$(eval $(generic-package))
