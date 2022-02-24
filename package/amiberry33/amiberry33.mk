################################################################################
#
# amiberry
#
################################################################################

AMIBERRY33_VERSION = v3.3
AMIBERRY33_SITE = $(call gitlab,recalbox,packages/standalone/amiberry,$(AMIBERRY33_VERSION))
AMIBERRY33_DEPENDENCIES = sdl2 sdl2_image sdl2_ttf libcapsimage libmpeg2 mpg123 flac
AMIBERRY33_LICENSE = GPL-3.0
AMIBERRY33_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
AMIBERRY33_PLATFORM=rpi4-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2LEGACY)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2),y)
AMIBERRY33_PLATFORM=rpi3-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
AMIBERRY33_PLATFORM=rpi1-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
AMIBERRY33_PLATFORM=xu4
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
AMIBERRY33_PLATFORM=go-advance
endif

define AMIBERRY33_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	$(MAKE) CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		AS="$(TARGET_AS)" \
		STRIP="$(TARGET_STRIP)" \
		-C $(@D) \
		-f Makefile \
		PLATFORM="$(AMIBERRY33_PLATFORM)"
endef

define AMIBERRY33_INSTALL_TARGET_CMDS
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
