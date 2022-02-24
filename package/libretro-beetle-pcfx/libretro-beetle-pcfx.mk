################################################################################
#
# BEETLE_PCFX
#
################################################################################

LIBRETRO_BEETLE_PCFX_VERSION = f0d284854a1973196c36590505f70c3404cc37b4
LIBRETRO_BEETLE_PCFX_SITE = $(call github,libretro,beetle-pcfx-libretro,$(LIBRETRO_BEETLE_PCFX_VERSION))
LIBRETRO_BEETLE_PCFX_LICENSE = GPL-2.0
LIBRETRO_BEETLE_PCFX_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
LIBRETRO_BEETLE_PCFX_MAKEFILE_EXTRA_OPTS = IS_X86=0
endif

define LIBRETRO_BEETLE_PCFX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" LD="$(TARGET_CXX)" RANLIB="$(TARGET_RANLIB)" -C $(@D) platform="$(RETROARCH_LIBRETRO_PLATFORM)" $(LIBRETRO_BEETLE_PCFX_MAKEFILE_EXTRA_OPTS)
endef

define LIBRETRO_BEETLE_PCFX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_pcfx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_pcfx_libretro.so
endef

$(eval $(generic-package))
