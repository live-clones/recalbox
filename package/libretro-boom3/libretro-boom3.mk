################################################################################
#
# BOOM3
#
################################################################################

LIBRETRO_BOOM3_VERSION = c55a7533ed4aac8db529c16a2e87f9f00b77d0ed
LIBRETRO_BOOM3_SITE = $(call github,libretro,boom3,$(LIBRETRO_BOOM3_VERSION))

define LIBRETRO_BOOM3_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/neo -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_BOOM3_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/neo/boom3_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/boom3_libretro.so
endef

$(eval $(generic-package))
