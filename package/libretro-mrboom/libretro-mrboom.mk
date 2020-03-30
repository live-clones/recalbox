################################################################################
#
# MRBOOM
#
################################################################################

LIBRETRO_MRBOOM_VERSION = a47e81ce1c4126a977780f6b0d797990161ddd20
LIBRETRO_MRBOOM_SITE = $(call github,libretro,mrboom-libretro,$(LIBRETRO_MRBOOM_VERSION))

# Flag to fix build on arm platforms
ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_MRBOOM_NEON += "HAVE_NEON=1"
else
LIBRETRO_MRBOOM_NEON += "HAVE_NEON=0"
endif


define LIBRETRO_MRBOOM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)" $(LIBRETRO_MRBOOM_OPTIONS)
endef

define LIBRETRO_MRBOOM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mrboom_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mrboom_libretro.so
endef

$(eval $(generic-package))
