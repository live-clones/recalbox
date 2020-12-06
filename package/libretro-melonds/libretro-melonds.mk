################################################################################
#
# MELONDS
#
################################################################################

LIBRETRO_MELONDS_VERSION = 7bbf8e1606697f7591072d05b61fae84e8b49455
LIBRETRO_MELONDS_SITE = $(call github,libretro,melonds,$(LIBRETRO_MELONDS_VERSION))
LIBRETRO_MELONDS_LICENSE = GPLv3

ifeq ($(BR2_arm)$(BR2_aarch64),y)
LIBRETRO_MELONDS_PIC = -fPIC
LIBRETRO_MELONDS_PLATFORM=linux-nogl
else
LIBRETRO_MELONDS_PLATFORM=unix
endif

define LIBRETRO_MELONDS_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO) $(LIBRETRO_MELONDS_PIC)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) $(LIBRETRO_MELONDS_PIC)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO) $(LIBRETRO_MELONDS_PIC)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MELONDS_PLATFORM)"
endef

define LIBRETRO_MELONDS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/melonds_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/melonds_libretro.so
endef

$(eval $(generic-package))
