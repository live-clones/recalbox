################################################################################
#
# DESMUME
#
################################################################################

LIBRETRO_DESMUME_VERSION = 7ea0fc96804fcd9c8d33e8f76cf64b1be50cc5ea
LIBRETRO_DESMUME_SITE = $(call github,libretro,desmume,$(LIBRETRO_DESMUME_VERSION))
LIBRETRO_DESMUME_DEPENDENCIES = libpcap
LIBRETRO_DESMUME_LICENSE = GPL-2.0+
LIBRETRO_DESMUME_LICENSE_FILES = license.txt

ifeq ($(BR2_arm)$(BR2_aarch64),y)
LIBRETRO_DESMUME_PLATFORM=linux-nogl
else
LIBRETRO_DESMUME_PLATFORM=unix
endif

define LIBRETRO_DESMUME_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/desmume/src/frontend/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS)  $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS)  $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_CXX) -lpthread" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" \
		-C $(@D)/desmume/src/frontend/libretro -f Makefile platform="$(LIBRETRO_DESMUME_PLATFORM)"
endef

define LIBRETRO_DESMUME_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/desmume/src/frontend/libretro/desmume_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/desmume_libretro.so
endef

$(eval $(generic-package))
