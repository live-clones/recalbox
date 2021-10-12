################################################################################
#
# GONG
#
################################################################################

#Commit of version 29/09/2021
LIBRETRO_GONG_VERSION = 79c5b691e2b49231fcf19dc37d3a992a9f806444
LIBRETRO_GONG_SITE = $(call github,libretro,gong,$(LIBRETRO_GONG_VERSION))
LIBRETRO_GONG_LICENSE = GPL-3.0

define LIBRETRO_GONG_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="unix"
endef

define LIBRETRO_GONG_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/gong_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/gong_libretro.so
endef

$(eval $(generic-package))
