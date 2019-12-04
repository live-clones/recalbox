################################################################################
#
# DHEWM3
#
################################################################################

LIBRETRO_DHEWM3_VERSION = f6abe8a5ecddfbeac918e452f6634ad5a5b32451
LIBRETRO_DHEWM3_SITE = $(call github,libretro,dhewm3-libretro,$(LIBRETRO_DHEWM3_VERSION))

define LIBRETRO_DHEWM3_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/neo -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_DHEWM3_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/neo/dhewm3_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/dhewm3_libretro.so
endef

$(eval $(generic-package))
