################################################################################
#
# NP2KAI
#
################################################################################
LIBRETRO_NP2KAI_VERSION = a54a4052150cb24f7f7b405b56f936ac4023327a
LIBRETRO_NP2KAI_SITE = $(call github,AZO234,NP2kai,$(LIBRETRO_NP2KAI_VERSION))
LIBRETRO_NP2KAI_LICENSE = MIT


define LIBRETRO_NP2KAI_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/sdl2 -f Makefile.libretro platform="$(LIBRETRO_PLATFORM)"
endef


define LIBRETRO_NP2KAI_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/np2kai_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/np2kai_libretro.so
endef


$(eval $(generic-package))