################################################################################
#
# OPENLARA
#
################################################################################

LIBRETRO_OPENLARA_VERSION = c7fd002610c551cfbbfa4947ea9c3a2574fe51c5
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
