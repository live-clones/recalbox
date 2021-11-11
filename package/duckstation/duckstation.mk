################################################################################
#
# DUCKSTATION
#
################################################################################

# Commit from 2021/09/01
DUCKSTATION_VERSION = 5b3c7072e760e70a458250f578b1cfaff23fa8bc
DUCKSTATION_SITE = https://github.com/stenzek/duckstation.git
DUCKSTATION_DEPENDENCIES = boost ffmpeg fmt libdrm sdl2 libevdev libcurl libegl
DUCKSTATION_SITE_METHOD = git
DUCKSTATION_GIT_SUBMODULES = YES
DUCKSTATION_LICENSE = GPLv2
DUCKSTATION_NON_COMMERCIAL = NO

# Don't build in source tree
DUCKSTATION_SUPPORTS_IN_SOURCE_BUILD = NO

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
DUCKSTATION_CONF_OPTS += -DEGL_INCLUDE_DIR="$(STAGING_DIR)/usr/include"
DUCKSTATION_SIDE_LIBS = -lGLESv2
DUCKSTATION_CONF_OPTS += -DUSE_FBDEV=ON
else
DUCKSTATION_SIDE_LIBS =
DUCKSTATION_CONF_OPTS += -DUSE_DRMKMS=ON
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
DUCKSTATION_CONF_OPTS += -DUSE_X11=ON
else
DUCKSTATION_CONF_OPTS += -DUSE_X11=OFF
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
DUCKSTATION_SIDE_LIBS += -lmali
endif

DUCKSTATION_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
DUCKSTATION_CONF_OPTS += -DBUILD_SHARED_LIBS=FALSE
DUCKSTATION_CONF_OPTS += -DANDROID=OFF -DENABLE_DISCORD_PRESENCE=OFF -DBUILD_LIBRETRO_CORE=OFF
DUCKSTATION_CONF_OPTS += -DBUILD_QT_FRONTEND=OFF -DBUILD_NOGUI_FRONTEND=ON
DUCKSTATION_CONF_OPTS += -DUSE_EVDEV=ON
DUCKSTATION_CONF_OPTS += -DUSE_WAYLAND=OFF
DUCKSTATION_CONF_OPTS += -DCMAKE_C_FLAGS="-DEGL_NO_X11"

DUCKSTATION_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
DUCKSTATION_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
DUCKSTATION_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
DUCKSTATION_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
DUCKSTATION_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
DUCKSTATION_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
DUCKSTATION_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
DUCKSTATION_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
DUCKSTATION_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_SO) -DEGL_NO_X11=1 $(DUCKSTATION_SIDE_LIBS)"
DUCKSTATION_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_SO) -DEGL_NO_X11=1 $(DUCKSTATION_SIDE_LIBS)"
DUCKSTATION_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_SO)"

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
DUCKSTATION_CONF_OPTS += -DUSE_EGL=ON
else
DUCKSTATION_CONF_OPTS += -DUSE_EGL=OFF
endif

DUCKSTATION_CONF_ENV += LDFLAGS=-lpthread

define DUCKSTATION_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/duckstation
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/psx

	$(INSTALL) -D $(@D)/buildroot-build/bin/duckstation-nogui $(TARGET_DIR)/usr/bin/duckstation
	cp -R $(@D)/buildroot-build/bin/database      $(TARGET_DIR)/usr/share/duckstation/
	cp -R $(@D)/buildroot-build/bin/inputprofiles $(TARGET_DIR)/usr/share/duckstation/
	cp -R $(@D)/buildroot-build/bin/resources     $(TARGET_DIR)/usr/share/duckstation/
	cp -R $(@D)/buildroot-build/bin/shaders       $(TARGET_DIR)/usr/share/duckstation/

	# Don't use provided controller db
	rm -f $(TARGET_DIR)/usr/share/duckstation/database/gamecontrollerdb.txt
endef

$(eval $(cmake-package))
