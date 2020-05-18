################################################################################
#
# DOSBOX_CORE
#
################################################################################

LIBRETRO_DOSBOX_CORE_VERSION = 7cb3896e0988ce365cc96ffd27f4a15cd98e6a76
LIBRETRO_DOSBOX_CORE_SITE = git://github.com/libretro/dosbox-core.git
LIBRETRO_DOSBOX_CORE_DEPENDENCIES = sdl sdl_net
LIBRETRO_DOSBOX_CORE_SITE_METHOD = git
LIBRETRO_DOSBOX_CORE_GIT_SUBMODULES = YES

LIBRETRO_DOSBOX_CORE_OPTIONS += BUNDLED_SDL=1

ifeq ($(BR2_arm),y)
LIBRETRO_DOSBOX_CORE_DYNAREC=arm
endif

ifeq ($(BR2_i386),y)
LIBRETRO_DOSBOX_CORE_DYNAREC=x86
endif

ifeq ($(BR2_x86_64),y)
LIBRETRO_DOSBOX_CORE_DYNAREC=x86_64
endif

define LIBRETRO_DOSBOX_CORE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_BOARD)"  $(LIBRETRO_DOSBOX_CORE_OPTIONS) \
		WITH_DYNAREC=$(LIBRETRO_DOSBOX_CORE_DYNAREC)
endef

define LIBRETRO_DOSBOX_CORE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/dosbox_core_libretro.so \
		$(TARGET_DIR)/usr/bin/dosbox_core_libretro.so
endef

$(eval $(generic-package))
