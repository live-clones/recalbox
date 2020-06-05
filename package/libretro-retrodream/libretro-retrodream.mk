################################################################################
#
# RETRODREAM
#
################################################################################

LIBRETRO_RETRODREAM_VERSION = c656735ffaa29ccca3fdb0384ceb3bddb9aa6c7a
LIBRETRO_RETRODREAM_SITE = $(call github,libretro,retrodream,$(LIBRETRO_RETRODREAM_VERSION))
LIBRETRO_RETRODREAM_LICENSE = GPLv3

ifeq ($(BR2_arm),y)
LIBRETRO_RETRODREAM_OPTS += FORCE_GLES=1 WITH_DYNAREC=arm HAVE_NEON=1
else ifeq ($(BR2_i386),y)
LIBRETRO_RETRODREAM_OPTS += WITH_DYNAREC=x86
else ifeq ($(BR2_x86_64),y)
LIBRETRO_RETRODREAM_OPTS += WITH_DYNAREC=x64
else
LIBRETRO_RETRODREAM_OPTS +=
endif

define LIBRETRO_RETRODREAM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/deps/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D)/deps/libretro -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)" $(LIBRETRO_RETRODREAM_OPTS)
endef

define LIBRETRO_RETRODREAM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/deps/libretro/retrodream_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/retrodream_libretro.so
endef

$(eval $(generic-package))
