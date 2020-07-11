################################################################################
#
# RETRODREAM
#
################################################################################

LIBRETRO_RETRODREAM_VERSION = bf4d812daf326c56e99208a4c30acae1475fd701
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
	# Kill residual X86 flags...
	$(SED) "s|CAPSTONE_HAS_X86|CAPSTONE_HAS_ARM|g" $(@D)/deps/libretro/Makefile
	# Kill shared memory (cause memory allocation failure)
	$(SED) "s|-DHAVE_FASTMEM||g" $(@D)/deps/libretro/Makefile.common
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D)/deps/libretro -f Makefile platform="unix-$(RETROARCH_LIBRETRO_PLATFORM)" $(LIBRETRO_RETRODREAM_OPTS)
endef

define LIBRETRO_RETRODREAM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/deps/libretro/retrodream_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/retrodream_libretro.so
endef

$(eval $(generic-package))
