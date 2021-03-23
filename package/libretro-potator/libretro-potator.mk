################################################################################
#
# Watara Supervision
#
################################################################################

LIBRETRO_POTATOR_VERSION = 356b6951a8dc59408e15f67cd449a6fe0e539f5a
LIBRETRO_POTATOR_SITE = $(call github,libretro,potator,$(LIBRETRO_POTATOR_VERSION))
LIBRETRO_POTATOR_LICENSE = Unlicense

define LIBRETRO_POTATOR_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CPPFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/platform/libretro -f Makefile platform="unix"
endef

define LIBRETRO_POTATOR_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/platform/libretro/potator_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/potator_libretro.so
endef

$(eval $(generic-package))
