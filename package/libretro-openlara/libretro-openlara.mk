################################################################################
#
# OPENLARA
#
################################################################################

LIBRETRO_OPENLARA_VERSION = fe88d74673344afc71891717b2474d33f90c165f
LIBRETRO_OPENLARA_SITE = $(call github,libretro,OpenLara,$(LIBRETRO_OPENLARA_VERSION))
LIBRETRO_OPENLARA_LICENSE = BSD-2-Clause
LIBRETRO_OPENLARA_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1)$(BR2_PACKAGE_RECALBOX_TARGET_RPI2)$(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
LIBRETRO_OPENLARA_PLATFORM=pi
else
LIBRETRO_OPENLARA_PLATFORM=unix
endif

define LIBRETRO_OPENLARA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO) -lpthread" \
	$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)/src/platform/libretro/ -f Makefile platform="$(LIBRETRO_OPENLARA_PLATFORM)"
endef

define LIBRETRO_OPENLARA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/platform/libretro/openlara_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/openlara_libretro.so
endef

$(eval $(generic-package))