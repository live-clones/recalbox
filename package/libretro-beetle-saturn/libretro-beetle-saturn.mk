################################################################################
#
# BEETLE_SATURN
#
################################################################################

LIBRETRO_BEETLE_SATURN_VERSION = 139528344281d8bd650ccbadb90be08142d976ce
LIBRETRO_BEETLE_SATURN_SITE = $(call github,libretro,beetle-saturn-libretro,$(LIBRETRO_BEETLE_SATURN_VERSION))
LIBRETRO_BEETLE_SATURN_LICENSE = GPL-2.0
LIBRETRO_BEETLE_SATURN_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
  	LIBRETRO_BEETLE_SATURN_BUILD_MAKEFILE_OPTIONS = platform="unix-gles" IS_X86=0
else
  	LIBRETRO_BEETLE_SATURN_BUILD_MAKEFILE_OPTIONS = platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endif

define LIBRETRO_BEETLE_SATURN_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile $(LIBRETRO_BEETLE_SATURN_BUILD_MAKEFILE_OPTIONS)
endef

define LIBRETRO_BEETLE_SATURN_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_saturn_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_saturn_libretro.so
endef

$(eval $(generic-package))
