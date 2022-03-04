################################################################################
#
# PICODRIVE
#
################################################################################

#Commit of 2022-02-11
LIBRETRO_PICODRIVE_VERSION = ee6b7139175a3270e9be314218b22baa592610f2
LIBRETRO_PICODRIVE_SITE = git://github.com/libretro/picodrive.git
LIBRETRO_PICODRIVE_LICENSE = COPYRIGHT
LIBRETRO_PICODRIVE_LICENSE_FILES = COPYING
LIBRETRO_PICODRIVE_NON_COMMERCIAL = y

LIBRETRO_PICODRIVE_DEPENDENCIES = libpng
LIBRETRO_PICODRIVE_GIT_SUBMODULES=y

# All arm 32bits
ifeq ($(BR2_arm),y)
PICOPLATFORM=$(RETROARCH_LIBRETRO_PLATFORM) armasm
# All arm 64bits
else ifeq ($(BR2_aarch64),y)
PICOPLATFORM=aarch64
else
PICOPLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
endif

define LIBRETRO_PICODRIVE_BUILD_CMDS
	$(MAKE) -C $(@D)/cpu/cyclone CONFIG_FILE=$(@D)/cpu/cyclone_config.h
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO) -I '$(@D)'" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -I '$(@D)'" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO) -I '$(@D)'" \
		$(MAKE) CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" -C  $(@D) -f Makefile.libretro platform="$(PICOPLATFORM)"
endef

define LIBRETRO_PICODRIVE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/picodrive_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/picodrive_libretro.so
endef

$(eval $(generic-package))
