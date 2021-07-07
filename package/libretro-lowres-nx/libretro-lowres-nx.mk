################################################################################
#
# LOWRES-NX
#
################################################################################

LIBRETRO_LOWRES_NX_VERSION = f2b5c1d4ec95290081eb95fd8262bd25bdea31cc
LIBRETRO_LOWRES_NX_SITE = $(call github,timoinutilis,lowres-nx,$(LIBRETRO_LOWRES_NX_VERSION))
LIBRETRO_LOWRES_NX_LICENSE = ZLIB
LIBRETRO_LOWRES_NX_LICENSE_FILES = LICENSE

define LIBRETRO_LOWRES_NX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/platform/LibRetro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/platform/LibRetro -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_LOWRES_NX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/platform/LibRetro/lowresnx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/lowresnx_libretro.so
endef

$(eval $(generic-package))
