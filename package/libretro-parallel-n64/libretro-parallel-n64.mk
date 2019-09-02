################################################################################
#
# PARALLEL-N64
#
################################################################################

LIBRETRO_PARALLEL_N64_VERSION = 017d864aa5a727f3aa9b3c21534f07de21d0a0c9
LIBRETRO_PARALLEL_N64_SITE = $(call github,libretro,parallel-n64,$(LIBRETRO_PARALLEL_N64_VERSION))
LIBRETRO_PARALLEL_N64_DEPENDENCIES = rpi-userland

ifeq ($(BR2_cortex_a7),y)
LIBRETRO_PARALLEL_N64_PLATFORM=rpi2
else
LIBRETRO_PARALLEL_N64_PLATFORM=rpi3
endif

define LIBRETRO_PARALLEL_N64_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)"
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PARALLEL_N64_PLATFORM)" WITH_DYNAREC=arm
endef

define LIBRETRO_PARALLEL_N64_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/parallel_n64_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/parallel_n64_libretro.so
endef

$(eval $(generic-package))
