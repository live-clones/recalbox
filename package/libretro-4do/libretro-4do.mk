################################################################################
#
# 4DO
#
################################################################################

LIBRETRO_4DO_VERSION = ffb026896ad49a7a7654f103ef74a7ce5641eb06
LIBRETRO_4DO_SITE = $(call github,libretro,4do-libretro,$(LIBRETRO_4DO_VERSION))

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_4DO_PLATFORM=rpi3
LIBRETRO_4DO_PLATFORM_CPU=cortex-a53
LIBRETRO_4DO_PLATFORM_OPT=-mfpu=neon-vfpv4 -fdata-sections -ffunction-sections -Wl,--gc-sections -fno-unroll-loops -fmerge-all-constants
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
LIBRETRO_4DO_PLATFORM=odroidxu4
LIBRETRO_4DO_PLATFORM_CPU=cortex-a5
LIBRETRO_4DO_PLATFORM_OPT=-mfpu=neon-vfpv4 -fdata-sections -ffunction-sections -Wl,--gc-sections -fno-unroll-loops -fmerge-all-constants
endif

define LIBRETRO_4DO_MAKEFILE_PATCHING
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
endef
#-fno-stack-protector -fno-ident -fno-unwind-tables -fno-asynchronous-unwind-tables
LIBRETRO_4DO_PRE_CONFIGURE_HOOKS += LIBRETRO_4DO_MAKEFILE_PATCHING

define LIBRETRO_4DO_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO) -DARM" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -DARM" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="unix"
endef

define LIBRETRO_4DO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/4do_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/4do_libretro.so
endef

$(eval $(generic-package))
