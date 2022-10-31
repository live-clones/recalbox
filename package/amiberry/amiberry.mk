################################################################################
#
# amiberry
#
################################################################################

AMIBERRY_VERSION = v4.1.6
AMIBERRY_SITE = $(call gitlab,recalbox,packages/standalone/amiberry,$(AMIBERRY_VERSION))
AMIBERRY_DEPENDENCIES = sdl2 sdl2_image sdl2_ttf libcapsimage libmpeg2 mpg123 flac
AMIBERRY_LICENSE = GPL-3.0
AMIBERRY_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
AMIBERRY_PLATFORM=rpi4-64-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
AMIBERRY_PLATFORM=rpi4-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2LEGACY)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2),y)
AMIBERRY_PLATFORM=rpi3-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
AMIBERRY_PLATFORM=rpi1-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
AMIBERRY_PLATFORM=xu4
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
AMIBERRY_PLATFORM=go-advance
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RG353X),y)
AMIBERRY_PLATFORM=a64
endif

define AMIBERRY_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
	$(MAKE) CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_LD)" \
		AS="$(TARGET_AS)" \
		STRIP="$(TARGET_STRIP)" \
		-C $(@D) \
		-f Makefile \
		PLATFORM="$(AMIBERRY_PLATFORM)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config"
endef

define AMIBERRY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/amiberry $(TARGET_DIR)/usr/bin/amiberry
	mkdir -p $(TARGET_DIR)/usr/share/amiberry
	cp -R $(@D)/data $(TARGET_DIR)/usr/share/amiberry
	cp -R $(@D)/whdboot $(TARGET_DIR)/usr/share/amiberry
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Savegames/foo.txt
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Kickstarts/foo.txt
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Debugs/foo.txt
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Autoboots/foo.txt
	# Copy RTB files
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios && \
		cp $(@D)/whdboot/save-data/Kickstarts/*.RTB $(TARGET_DIR)/recalbox/share_upgrade/bios/
endef

$(eval $(generic-package))
