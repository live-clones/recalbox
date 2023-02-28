################################################################################
#
# VITAQUAKE2
#
################################################################################

#Commit version of 15/12/2021
LIBRETRO_VITAQUAKE2_VERSION = aed906ef107eb684e18d8889c9bf383cc7b77b84
LIBRETRO_VITAQUAKE2_SITE = $(call github,libretro,vitaquake2,$(LIBRETRO_VITAQUAKE2_VERSION))
LIBRETRO_VITAQUAKE2_LICENSE = GPL-2.0
LIBRETRO_VITAQUAKE2_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RECALBOX_HAS_LIBGLES3),y)
LIBRETRO_VITAQUAKE2_PLATFORM=recalbox-embedded rpi4
else ifeq ($(BR2_PACKAGE_RECALBOX_HAS_LIBGLES),y)
LIBRETRO_VITAQUAKE2_PLATFORM=recalbox-embedded
else
LIBRETRO_VITAQUAKE2_PLATFORM=unix
endif

define LIBRETRO_VITAQUAKE2_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="$(LIBRETRO_VITAQUAKE2_PLATFORM)"
endef

define LIBRETRO_VITAQUAKE2_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vitaquake2_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vitaquake2_libretro.so
endef

$(eval $(generic-package))
