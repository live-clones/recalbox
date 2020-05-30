################################################################################
#
# MRBOOM
#
################################################################################

LIBRETRO_MRBOOM_VERSION = 441ba41151f2a688c6d9a2c1a34ff2106d8c57f8
LIBRETRO_MRBOOM_SITE = git://github.com/libretro/mrboom-libretro.git
LIBRETRO_MRBOOM_SITE_METHOD=git
LIBRETRO_MRBOOM_GIT_SUBMODULES=y

# Flag to fix build on arm platforms
ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_MRBOOM_OPTIONS += HAVE_NEON=1
else
LIBRETRO_MRBOOM_OPTIONS +=
endif

define LIBRETRO_MRBOOM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="unix" $(LIBRETRO_MRBOOM_OPTIONS)
endef

define LIBRETRO_MRBOOM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mrboom_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mrboom_libretro.so
endef

$(eval $(generic-package))
