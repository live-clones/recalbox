################################################################################
#
# ECWOLF
#
################################################################################

LIBRETRO_ECWOLF_VERSION = 5aeb2557633a74bf588cbbfe322a8911629a57e1
LIBRETRO_ECWOLF_SITE = $(call github,libretro,ecwolf,$(LIBRETRO_ECWOLF_VERSION))

define LIBRETRO_ECWOLF_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/src/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/src/libretro/ -f Makefile platform="unix"
endef

define LIBRETRO_ECWOLF_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/libretro/ecwolf_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/ecwolf_libretro.so
endef

$(eval $(generic-package))
