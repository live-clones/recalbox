################################################################################
#
# amiberry
#
################################################################################
ifeq ($(BR2_PACKAGE_RECALBOX_TARGETGROUP_ROCKCHIP),y)
AMIBERRY_VERSION = 9cffc2680394fa705a7732e36f2453d8ace8eaad
AMIBERRY_BINARY = amiberry
else
AMIBERRY_VERSION = v2.25
AMIBERRY_BINARY = amiberry-$(AMIBERRY_PLATFORM)
endif
AMIBERRY_SITE = $(call github,midwan,amiberry,$(AMIBERRY_VERSION))
AMIBERRY_DEPENDENCIES = sdl2 sdl2_image sdl2_mixer guichan mpg123 host-pkgconf sdl2_ttf libcapsimage libmpeg2

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
AMIBERRY_PLATFORM=rpi3-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
AMIBERRY_PLATFORM=rpi2-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
AMIBERRY_PLATFORM=rpi1-sdl2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
AMIBERRY_PLATFORM=xu4
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RK3328),y)
AMIBERRY_PLATFORM=RK3328
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RK3399),y)
AMIBERRY_PLATFORM=RK3399
endif

SEDPLUS = sed -i -E -e

define AMIBERRY_RIGHT_PI_PATH
	#
	# PATCH MakeFile
	#
	$(SED) "s+-I/opt/vc+-I$(STAGING_DIR)/usr+g" $(@D)/Makefile
	$(SED) "s+-Isrc/include+-Isrc/include -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL+g" $(@D)/Makefile
	$(SED) "s+-L/opt/vc/lib+-L$(STAGING_DIR)/usr/lib -lvchostif -lbcm_host `$(STAGING_DIR)/usr/bin/sdl-config --libs`+g" $(@D)/Makefile
	$(SED) "s+xml2-config+$(STAGING_DIR)/usr/bin/xml2-config+g" $(@D)/Makefile
	$(SED) "s+sdl-config+$(STAGING_DIR)/usr/bin/sdl-config+g" $(@D)/Makefile
	# For sdl2-dev branch
	$(SED) "s+sdl2-config+$(STAGING_DIR)/usr/bin/sdl2-config+g" $(@D)/Makefile
	$(SED) "s+sdl2-config+$(STAGING_DIR)/usr/bin/sdl2-config+g" $(@D)/guisan-dev/Makefile
	#
	# PATCH Path in source code
	#
	# -- data patch in GUI code
	$(SEDPLUS) "s|data/(.*?)\.png|/usr/share/amiberry/data/\1.png|g" $(@D)/src/osdep/gui/PanelHD.cpp
	$(SEDPLUS) "s|data/(.*?)\.png|/usr/share/amiberry/data/\1.png|g" $(@D)/src/osdep/gui/PanelAbout.cpp
	$(SEDPLUS) "s|data/(.*?)\.png|/usr/share/amiberry/data/\1.png|g" $(@D)/src/osdep/gui/main_window.cpp
	$(SEDPLUS) "s|data/(.*?)\.ico|/usr/share/amiberry/data/\1.ico|g" $(@D)/src/osdep/gui/main_window.cpp
	$(SEDPLUS) "s|data/(.*?)\.ttf|/usr/share/amiberry/data/\1.ttf|g" $(@D)/src/osdep/gui/main_window.cpp
	$(SEDPLUS) "s|data/(.*?)\.bmp|/usr/share/amiberry/data/\1.bmp|g" $(@D)/src/osdep/gui/main_window.cpp
	# -- libcapsimage (IPF) patch
	$(SEDPLUS) "s|\./capsimg\.so|/usr/lib/libcapsimage\.so\.4|g" $(@D)/src/dlopen.cpp
	# -- Reroot CWD to /tmp/amiga
	$(SEDPLUS) "s|getcwd\(start_path_data,\sMAX_DPATH\)|strncpy(start_path_data, \"/tmp/amiga\", MAX_DPATH)|g" $(@D)/src/osdep/amiberry.cpp
	# -- Savestate
	$(SEDPLUS) "s|\%s/savestates/default.ads|/recalbox/share/saves/default.ads|g" $(@D)/src/osdep/amiberry.cpp
	$(SEDPLUS) "s|strncat\(out\,\s\"/savestates/\"\,\ssize\)|strncpy(out, \"/recalbox/share/saves/\", size)|g" $(@D)/src/osdep/amiberry.cpp
	# -- Screenshots
	$(SEDPLUS) "s|strncat\(out\,\s\"/screenshots/\"\,\ssize\)|strncpy(out, \"/recalbox/share/screenshots/\", size)|g" $(@D)/src/osdep/amiberry.cpp
	# -- Logging to tmp
	$(SEDPLUS) "s|\%s/amiberry_log.txt|/tmp/amiga/amiberry_log.txt|g" $(@D)/src/osdep/amiberry.cpp
	# -- Global conf
	$(SEDPLUS) "s|\%s/conf|/tmp/amiga/conf|g" $(@D)/src/osdep/amiberry.cpp
	# -- Controllers
	$(SEDPLUS) "s|\%s/controllers|/tmp/amiga/conf|g" $(@D)/src/osdep/amiberry.cpp
	# -- Kickstarts
	$(SEDPLUS) "s|\%s/kickstarts|/recalbox/share/bios|g" $(@D)/src/osdep/amiberry.cpp
	# -- RP9/UAE Confg files
	$(SEDPLUS) "s|\%s/rp9|/tmp/amiga/conf|g" $(@D)/src/osdep/amiberry.cpp
endef
AMIBERRY_PRE_CONFIGURE_HOOKS += AMIBERRY_RIGHT_PI_PATH

define AMIBERRY_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_GCC_LINKER_OPTIMIZATION_EXECUTABLE)" \
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE)	CPP="$(TARGET_CPP)" \
		CXX="$(TARGET_CXX)" \
		CC="$(TARGET_CC)" \
		AS="$(TARGET_CC)" \
		STRIP="$(TARGET_STRIP)" \
		SDL_CONFIG=$(STAGING_DIR)/usr/bin/sdl2-config \
		-C $(@D) \
		-f Makefile \
		PLATFORM="$(AMIBERRY_PLATFORM)"
endef

define AMIBERRY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/$(AMIBERRY_BINARY) $(TARGET_DIR)/usr/bin/amiberry
	mkdir -p $(TARGET_DIR)/usr/share/amiberry/data
	cp -R $(@D)/data $(TARGET_DIR)/usr/share/amiberry
	cp -R $(@D)/whdboot $(TARGET_DIR)/usr/share/amiberry
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/hostprefs.conf
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Savegames/foo.txt
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Kickstarts/foo.txt
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Debugs/foo.txt
	rm $(TARGET_DIR)/usr/share/amiberry/whdboot/save-data/Autoboots/foo.txt
endef

$(eval $(generic-package))
