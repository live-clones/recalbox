################################################################################
#
# DINOTHAWR
#
################################################################################

LIBRETRO_DINOTHAWR_VERSION = 18118f66f4a04897fd1cbbfa87cd8957f5bd22ad
LIBRETRO_DINOTHAWR_SITE = $(call github,libretro,Dinothawr,$(LIBRETRO_DINOTHAWR_VERSION))

define LIBRETRO_DINOTHAWR_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_DINOTHAWR_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/dinothawr_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/dinothawr_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/roms/ports/Dinothawr/dinothawr
	cp -R $(@D)/dinothawr/* $(TARGET_DIR)/recalbox/share_init/roms/ports/Dinothawr/dinothawr
endef

$(eval $(generic-package))
