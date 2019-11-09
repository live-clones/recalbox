################################################################################
#
# DHEWM3
#
################################################################################

LIBRETRO_DHEWM3_VERSION = 078f452426a2eafd998b4d7adf401d364861c34c
LIBRETRO_DHEWM3_SITE = $(call github,libretro,dhewm3-libretro,$(LIBRETRO_DHEWM3_VERSION))

define LIBRETRO_DHEWM3_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/neo/Makefile
	$(SED) "s|LDFLAGS :=|LDFLAGS := $(COMPILER_COMMONS_LDFLAGS_SO)|g" $(@D)/neo/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/neo -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_DHEWM3_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/hdewm3_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/hdewm3_libretro.so
endef

$(eval $(generic-package))
