################################################################################
#
# UAE
#
################################################################################

LIBRETRO_UAE_VERSION = 753f042dcb47120f0b2f9744db5f9a69ae49bb86
LIBRETRO_UAE_SITE = $(call github,libretro,libretro-uae,$(LIBRETRO_UAE_VERSION))

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
UAEPLATFORM=rpi
else
UAEPLATFORM=unix
ifeq ($(BR2_ARM_CPU_HAS_ARM),y)
UAEPLATFLAGS=-DARM -marm
endif
ifeq ($(BR2_aarch64),y)
UAEPLATFLAGS=-DAARCH64
endif
endif

ifeq ($(BR2_x86_64),y)
# x64 does not support LTO
LIBRETRO_UAE_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_UAE_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_UAE_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)
else
LIBRETRO_UAE_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_UAE_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_UAE_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)
endif

define LIBRETRO_UAE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|^PLATFLAGS :=|PLATFLAGS := $(UAEPLATFLAGS)|g" $(@D)/Makefile
	$(SED) "s+defined(WIIU)+defined(WIIU) || defined(AARCH64)+g" $(@D)/sources/src/machdep/maccess.h
	$(SED) "s+defined(WIIU)+defined(WIIU) || defined(AARCH64)+g" $(@D)/sources/src/machdep/m68kops.h
	CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_UAE_COMPILER_COMMONS_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(LIBRETRO_UAE_COMPILER_COMMONS_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(LIBRETRO_UAE_COMPILER_COMMONS_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(UAEPLATFORM)"
endef

define LIBRETRO_UAE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/puae_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/puae_libretro.so
endef

$(eval $(generic-package))
