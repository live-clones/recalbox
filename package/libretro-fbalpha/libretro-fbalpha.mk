################################################################################
#
# FBALPHA
#
################################################################################
LIBRETRO_FBALPHA_VERSION = 57cec6c760f83391712435abc5b4e31f9b90e1b8
LIBRETRO_FBALPHA_SITE = $(call github,libretro,fbalpha,$(LIBRETRO_FBALPHA_VERSION))

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
	LIBRETRO_FBALPHA_NEON += "HAVE_NEON=1"
else
	LIBRETRO_FBALPHA_NEON += "HAVE_NEON=0"
endif

define LIBRETRO_FBALPHA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f makefile.libretro platform="$(LIBRETRO_BOARD)" $(LIBRETRO_FBALPHA_NEON)
endef

define LIBRETRO_FBALPHA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/fbalpha_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/fbalpha_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/fba/samples
	cp "$(@D)/dats/FB Alpha (ClrMame Pro XML, Arcade only).dat" \
	    $(TARGET_DIR)/recalbox/share_init/bios/fba
	cp "$(@D)/dats/FB Alpha (ClrMame Pro XML, Neogeo only).dat" \
	    $(TARGET_DIR)/recalbox/share_init/bios/fba
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_init/bios/fba
endef

$(eval $(generic-package))