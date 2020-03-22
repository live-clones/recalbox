################################################################################
#
# BEETLE_PCE_FAST
#
################################################################################

LIBRETRO_BEETLE_PCE_FAST_VERSION = 019179ce335b68a22dcba94ae6c51cad8135ee16
LIBRETRO_BEETLE_PCE_FAST_SITE = $(call github,libretro,beetle-pce-fast-libretro,$(LIBRETRO_BEETLE_PCE_FAST_VERSION))

define LIBRETRO_BEETLE_PCE_FAST_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_BEETLE_PCE_FAST_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_pce_fast_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_pce_fast_libretro.so
endef

$(eval $(generic-package))
