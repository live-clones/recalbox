################################################################################
#
# CROCODS
#
################################################################################
LIBRETRO_CROCODS_VERSION = a7d1098000376a39d70db171863c8f46da7de07d
LIBRETRO_CROCODS_SITE = $(call github,libretro,libretro-crocods,$(LIBRETRO_CROCODS_VERSION))

define LIBRETRO_CROCODS_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="$(LIBRETRO_PLATFORM)"
endef

define LIBRETRO_CROCODS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/crocods_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/crocods_libretro.so
endef

$(eval $(generic-package))
