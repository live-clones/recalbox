################################################################################
#
# UAE
#
################################################################################

LIBRETRO_UAE_VERSION = e491fc481b6713e89951064e82a783b59208aba3
LIBRETRO_UAE_SITE = $(call github,libretro,libretro-uae,master)

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

define LIBRETRO_UAE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|^PLATFLAGS :=|PLATFLAGS := $(UAEPLATFLAGS)|g" $(@D)/Makefile
	$(SED) "s+defined(WIIU)+defined(WIIU) || defined(AARCH64)+g" $(@D)/sources/src/machdep/maccess.h
	$(SED) "s+defined(WIIU)+defined(WIIU) || defined(AARCH64)+g" $(@D)/sources/src/machdep/m68kops.h
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(UAEPLATFORM)"
endef

define LIBRETRO_UAE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/puae_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/puae_libretro.so
endef

$(eval $(generic-package))
