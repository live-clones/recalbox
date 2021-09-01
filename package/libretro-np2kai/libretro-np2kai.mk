################################################################################
#
# NP2KAI
#
################################################################################

LIBRETRO_NP2KAI_VERSION = 3e8fedc7c1c6f68faa26589187512474a766ee9e
LIBRETRO_NP2KAI_SITE = $(call github,AZO234,NP2kai,$(LIBRETRO_NP2KAI_VERSION))
LIBRETRO_NP2KAI_LICENSE = MIT

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
LIBRETRO_NP2KAI_PLATFORM=rpi4-embedded
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_NP2KAI_PLATFORM=rpi3-embedded
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
LIBRETRO_NP2KAI_PLATFORM=rpi2-embedded
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
LIBRETRO_NP2KAI_PLATFORM=xu4-embedded
else
LIBRETRO_NP2KAI_PLATFORM=unix
endif

define LIBRETRO_NP2KAI_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/sdl/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/sdl -f Makefile.libretro platform="$(LIBRETRO_NP2KAI_PLATFORM)"
endef

define LIBRETRO_NP2KAI_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/sdl/np2kai_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/np2kai_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/np2kai
endef

$(eval $(generic-package))
