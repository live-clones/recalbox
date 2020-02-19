################################################################################
#
# BEETLE_SATURN
#
################################################################################

LIBRETRO_BEETLE_SATURN_VERSION = 8a65943bb7bbc3183eeb0d57c4ac3e663f1bcc11
LIBRETRO_BEETLE_SATURN_SITE = $(call github,libretro,beetle-saturn-libretro,$(LIBRETRO_BEETLE_SATURN_VERSION))

define LIBRETRO_BEETLE_SATURN_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_BEETLE_SATURN_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_saturn_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_saturn_libretro.so
endef

$(eval $(generic-package))
