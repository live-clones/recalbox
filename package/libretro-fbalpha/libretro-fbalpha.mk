################################################################################
#
# FBALPHA
#
################################################################################

LIBRETRO_FBALPHA_VERSION = 4eda6f1cf7e2dfe24d30e60a26cbc933cb8f710e
LIBRETRO_FBALPHA_SITE = $(call github,libretro,fbalpha,$(LIBRETRO_FBALPHA_VERSION))

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_FBALPHA_NEON += "HAVE_NEON=1"
else
LIBRETRO_FBALPHA_NEON += "HAVE_NEON=0"
endif

ifeq ($(BR2_arm),y)
LIBRETRO_FBALPHA_USE_CYCLONE = USE_CYCLONE=1
endif

define LIBRETRO_FBALPHA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f makefile.libretro platform="$(RETROARCH_LIBRETRO_BOARD)" $(LIBRETRO_FBALPHA_NEON) $(LIBRETRO_FBALPHA_USE_CYCLONE)
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
