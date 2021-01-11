################################################################################
#
# amiberry
#
################################################################################

AMIBERRY_VERSION = v3.3
AMIBERRY_SITE = $(call gitlab,recalbox,packages/standalone/amiberry,$(AMIBERRY_VERSION))
AMIBERRY_DEPENDENCIES = sdl2 sdl2_image sdl2_ttf libcapsimage libmpeg2 mpg123 flac
AMIBERRY_LICENSE = GPL-3.0
AMIBERRY_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
AMIBERRY_PLATFORM=rpi4-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
AMIBERRY_PLATFORM=rpi3-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
AMIBERRY_PLATFORM=rpi2-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
AMIBERRY_PLATFORM=rpi1-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
AMIBERRY_PLATFORM=xu4
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
AMIBERRY_PLATFORM=go-advance
endif

define AMIBERRY_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	$(MAKE) CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		AS="$(TARGET_AS)" \
		STRIP="$(TARGET_STRIP)" \
		-C $(@D) \
		-f Makefile \
		PLATFORM="$(AMIBERRY_PLATFORM)"
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
endef

$(eval $(generic-package))
