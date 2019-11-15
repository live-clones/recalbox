################################################################################
#
# OPENLARA
#
################################################################################

LIBRETRO_OPENLARA_VERSION = 2c9e0a8ff49afcc6c1cd8c9e0ad8604312e79207
LIBRETRO_OPENLARA_SITE = $(call github,libretro,OpenLara,$(LIBRETRO_OPENLARA_VERSION))

define LIBRETRO_OPENLARA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
	$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)/src/platform/libretro/ platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_OPENLARA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/platform/libretro/openlara_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/openlara_libretro.so
endef

$(eval $(generic-package))
