################################################################################
#
# MUPEN64PLUS
#
################################################################################

LIBRETRO_PARALLEL64_VERSION = ab155da18068f638e5ace2e5e6f7387bddc3511b
LIBRETRO_PARALLEL64_SITE = $(call github,libretro,parallel-n64,$(LIBRETRO_PARALLEL64_VERSION))
LIBRETRO_PARALLEL64_DEPENDENCIES = rpi-userland

ifeq ($(BR2_cortex_a7),y)
LIBRETRO_PARALLEL64_PLATFORM=rpi2
else
LIBRETRO_PARALLEL64_PLATFORM=rpi3
endif

define LIBRETRO_PARALLEL64_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)"
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PARALLEL64_PLATFORM)" WITH_DYNAREC=arm
endef

define LIBRETRO_PARALLEL64_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/parallel_n64_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/parallel_n64_libretro.so
endef

$(eval $(generic-package))
