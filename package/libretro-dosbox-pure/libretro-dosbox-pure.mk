################################################################################
#
# DOSBOX_PURE
#
################################################################################

LIBRETRO_DOSBOX_PURE_VERSION = 669c1f11889a0b52203e63e24a5207c0474bf19b
LIBRETRO_DOSBOX_PURE_SITE = $(call gitlab,recalbox,packages/libretro/libretro-dosbox-pure,$(LIBRETRO_DOSBOX_PURE_VERSION))
LIBRETRO_DOSBOX_PURE_LICENSE = GPL-2.0
LIBRETRO_DOSBOX_PURE_LICENSE_FILES = LICENSE

#ifeq ($(BR2_x86_64)$(BR2_i386),y)
#LIBRETRO_DOSBOX_PURE_GLES = 0
#else
#LIBRETRO_DOSBOX_PURE_GLES = 1
#endif

define LIBRETRO_DOSBOX_PURE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_DOSBOX_PURE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/dosbox_pure_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/dosbox_pure_libretro.so
endef

$(eval $(generic-package))
