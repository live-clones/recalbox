################################################################################
#
# BEETLE_SATURN
#
################################################################################

LIBRETRO_BEETLE_SATURN_VERSION = 3313cc6760c14cffa9226e0cfd41debc11df8bdd
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
