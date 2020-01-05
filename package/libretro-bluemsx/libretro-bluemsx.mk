################################################################################
#
# BLUEMSX
#
################################################################################

LIBRETRO_BLUEMSX_VERSION = 5aa8297e1a9308f41b9954fc9a593b28ca389cdd
LIBRETRO_BLUEMSX_SITE = $(call github,libretro,blueMSX-libretro,$(LIBRETRO_BLUEMSX_VERSION))

define LIBRETRO_BLUEMSX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_BLUEMSX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/bluemsx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/bluemsx_libretro.so
	# Create bios directory
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios
	# Copy Databases and Machines directories
	cp -R $(@D)/system/bluemsx/* $(TARGET_DIR)/recalbox/share_init/bios
endef

$(eval $(generic-package))
