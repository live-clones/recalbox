################################################################################
#
# EMUSCV
#
################################################################################

#Commit version of 2022-03-28
LIBRETRO_EMUSCV_VERSION = 67c8f616a4e2bbe21d0af0d219ee37dbb62c1a15
LIBRETRO_EMUSCV_SITE = $(call gitlab,recalbox,packages/libretro/libretro-emuscv,$(LIBRETRO_EMUSCV_VERSION))
LIBRETRO_EMUSCV_LICENSE = GPL-3.0
LIBRETRO_EMUSCV_LICENSE_FILES = licence.txt

define LIBRETRO_EMUSCV_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="unix"
endef

define LIBRETRO_EMUSCV_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/emuscv_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/emuscv_libretro.so
endef

$(eval $(generic-package))
