################################################################################
#
# SAME_CDI
#
#################################################################################

# Commit version of 2022/12/11
LIBRETRO_SAME_CDI_VERSION = 2ee1200d3c9bbceb64e1f0ec9e5224a16cdc0061
LIBRETRO_SAME_CDI_SITE = $(call github,libretro,same_cdi,$(LIBRETRO_SAME_CDI_VERSION))
LIBRETRO_SAME_CDI_LICENSE = GPL-2.0+
LIBRETRO_SAME_CDI_LICENSE = MAME
LIBRETRO_SAME_CDI_NON_COMMERCIAL = y

define LIBRETRO_SAME_CDI_BUILD_CMDS
        CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
                CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
                LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
                $(MAKE) -C $(@D)/ -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_SAME_CDI_INSTALL_TARGET_CMDS
        $(INSTALL) -D $(@D)/same_cdi_libretro.so \
                $(TARGET_DIR)/usr/lib/libretro/same_cdi_libretro.so
endef

$(eval $(generic-package))

