################################################################################
#
# SAMEBOY
#
################################################################################

LIBRETRO_SAMEBOY_VERSION = 50f6f92b6f6ab8b778d0e1f41b4ed437e7e43bea
LIBRETRO_SAMEBOY_SITE = $(call github,libretro,SameBoy,$(LIBRETRO_SAMEBOY_VERSION))

define LIBRETRO_SAMEBOY_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro -f Makefile platform="unix"
endef

define LIBRETRO_SAMEBOY_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/sameboy_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/sameboy_libretro.so
	$(INSTALL) -D $(@D)/BootROMs/prebuilt/*.bin \
		$(TARGET_DIR)/recalbox/share_init/bios/
endef

$(eval $(generic-package))
