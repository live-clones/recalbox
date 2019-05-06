################################################################################
#
# FUSE
#
################################################################################

LIBRETRO_FUSE_VERSION = 43d01dec6ad26e6014fabf79dbcf6a04ac2c7eea
LIBRETRO_FUSE_SITE = $(call github,libretro,fuse-libretro,$(LIBRETRO_FUSE_VERSION))

ifeq ($(BR2_aarch64),y)
LIBRETRO_FUSE_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_FUSE_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_FUSE_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)
else
LIBRETRO_FUSE_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_FUSE_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_FUSE_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)
endif


define LIBRETRO_FUSE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_FUSE_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_FUSE_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_FUSE_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_FUSE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/fuse_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/fuse_libretro.so
endef

$(eval $(generic-package))
