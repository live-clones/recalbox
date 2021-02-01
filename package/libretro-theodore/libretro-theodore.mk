################################################################################
#
# THEODORE
#
################################################################################

LIBRETRO_THEODORE_VERSION = a965b9890ae4c7f56b8154176561624881d041eb
LIBRETRO_THEODORE_SITE = $(call github,Zlika,theodore,$(LIBRETRO_THEODORE_VERSION))
LIBRETRO_THEODORE_LICENSE = GPL-3.0
LIBRETRO_THEODORE_LICENSE_FILES = LICENSE

define LIBRETRO_THEODORE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_THEODORE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/theodore_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/theodore_libretro.so
endef

$(eval $(generic-package))
