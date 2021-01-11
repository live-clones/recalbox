################################################################################
#
# GSPLUS - Apple II GS
#
################################################################################

GSPLUS_VERSION = 480572054518112647c8fae5d7ea7046a6d6ecfb
GSPLUS_SITE = $(call github,digarok,gsplus,$(GSPLUS_VERSION))
GSPLUS_DEPENDENCIES = sdl2 sdl2_image freetype libpcap
GSPLUS_LICENSE = GPL-2.0
GSPLUS_LICENSE_FILES = COPYING

define GSPLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/bin/GSplus \
		$(TARGET_DIR)/usr/bin/
	cp $(@D)/bin/libx_readline.so $(TARGET_DIR)/usr/lib/
	cp $(@D)/lib/letgothl.ttf $(TARGET_DIR)/usr/share
endef

define GSPLUS_POST_EXTRACT_FIX_PATH
	$(SED) "s|lib/letgothl.ttf|/usr/share/letgothl.ttf|g" $(@D)/src/sim65816.c
	$(SED) 's|g_cfg_rom_path = "ROM";|g_cfg_rom_path = "/recalbox/share/bios/apple2gs.rom";|g' $(@D)/src/config.c
endef

GSPLUS_POST_EXTRACT_HOOKS += GSPLUS_POST_EXTRACT_FIX_PATH

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
GSPLUS_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
GSPLUS_DEPENDENCIES += libgles
endif

GSPLUS_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
GSPLUS_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
GSPLUS_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
GSPLUS_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
GSPLUS_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
GSPLUS_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
GSPLUS_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
GSPLUS_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
GSPLUS_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_NOLTO)"
GSPLUS_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_NOLTO)"
GSPLUS_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_NOLTO)"

$(eval $(cmake-package))
