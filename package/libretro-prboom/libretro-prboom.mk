################################################################################
#
# PRBOOM
#
################################################################################

LIBRETRO_PRBOOM_VERSION = 1e5faec9ba5d884747422bcac33c1677d0432c99
LIBRETRO_PRBOOM_SITE = $(call github,libretro,libretro-prboom,$(LIBRETRO_PRBOOM_VERSION))

define LIBRETRO_PRBOOM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|LDFLAGS :=|LDFLAGS := $(COMPILER_COMMONS_LDFLAGS_SO)|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_PRBOOM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/prboom_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/prboom_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/prboom
	$(INSTALL) -D $(@D)/prboom.wad $(TARGET_DIR)/recalbox/share_init/bios/prboom
endef

$(eval $(generic-package))
