################################################################################
#
# 4DO
#
################################################################################
LIBRETRO_4DO_VERSION = e2ba1ebcbf6f210fcb8807ea43f57fe03c5a10ba
LIBRETRO_4DO_SITE = $(call github,libretro,4do-libretro,$(LIBRETRO_4DO_VERSION))

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
	4DO_PLATFORM=rpi3
	4DO_PLATFORM_CPU=cortex-a53
	4DA_PLATFORM_OPT=-mfpu=neon-vfpv4 -fdata-sections -ffunction-sections -Wl,--gc-sections -fno-unroll-loops -fmerge-all-constants
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
	4DO_PLATFORM=odroidxu4
	4DO_PLATFORM_CPU=cortex-a5
	4DA_PLATFORM_OPT=-mfpu=neon-vfpv4 -fdata-sections -ffunction-sections -Wl,--gc-sections -fno-unroll-loops -fmerge-all-constants
endif

define LIBRETRO_4DO_MAKEFILE_PATCHING
 	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
 	$(SED) "s|CFLAGS = -mcpu=$(4DO_PLATFORM_CPU)|CFLAGS = -mcpu=$(4DO_PLATFORM_CPU) $(4DA_PLATFORM_OPT)|g" $(@D)/Makefile
endef
#-fno-stack-protector -fno-ident -fno-unwind-tables -fno-asynchronous-unwind-tables 
LIBRETRO_4DO_PRE_CONFIGURE_HOOKS += LIBRETRO_4DO_MAKEFILE_PATCHING

define LIBRETRO_4DO_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="unix $(4DO_PLATFORM)"
endef

define LIBRETRO_4DO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/4do_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/4do_libretro.so
endef

$(eval $(generic-package))
