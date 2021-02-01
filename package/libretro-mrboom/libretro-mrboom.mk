################################################################################
#
# MRBOOM
#
################################################################################

LIBRETRO_MRBOOM_VERSION = c59553c8ea025515b2e77cea6056b5108acf867d
LIBRETRO_MRBOOM_SITE = git://github.com/libretro/mrboom-libretro.git
LIBRETRO_MRBOOM_LICENSE = MIT
LIBRETRO_MRBOOM_LICENSE_FILES = LICENSE

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
