################################################################################
#
# VITAQUAKE2
#
################################################################################

LIBRETRO_VITAQUAKE2_VERSION = 77316663d977a5921b5416341cf18f45c6b6dba0
LIBRETRO_VITAQUAKE2_SITE = $(call github,libretro,vitaquake2,$(LIBRETRO_VITAQUAKE2_VERSION))

define LIBRETRO_VITAQUAKE2_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_VITAQUAKE2_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vitaquake2_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vitaquake2_libretro.so
endef

$(eval $(generic-package))
