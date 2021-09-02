################################################################################
#
# MGT SAM Coupe Emulation
#
################################################################################

SIMCOUPE_VERSION = v1.2.5
SIMCOUPE_SITE = https://gitlab.com/recalbox/packages/standalone/simcoupe.git
SIMCOUPE_LICENSE = GPL-2.0
SIMCOUPE_LICENSE_FILES = License.txt
SIMCOUPE_SITE_METHOD = git
SIMCOUPE_DEPENDENCIES = sdl2 fmt

SIMCOUPE_BIOS_AND_RESOURCES = /usr/share/simcoupe

define SIMCOUPE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/simcoupe $(TARGET_DIR)/usr/bin/simcoupe
	$(INSTALL) -D $(@D)/_deps/saasound-build/libSAASound* $(TARGET_DIR)/usr/lib/
	$(INSTALL) -D $(@D)/_deps/resid-build/libresid* $(TARGET_DIR)/usr/lib/
	#$(INSTALL) -D $(@D)/Extern/fmt/libfmt* $(TARGET_DIR)/usr/lib/
	mkdir -p $(TARGET_DIR)$(SIMCOUPE_BIOS_AND_RESOURCES)
	cp -R $(@D)/Resource/** $(TARGET_DIR)$(SIMCOUPE_BIOS_AND_RESOURCES)
endef

define SIMCOUPE_POST_EXTRACT_FIX_SDL2_PATH
	# Change emulator resource folder
	sed -i -E -e "s|set\(RESOURCE_DIR \\$$\{CMAKE_INSTALL_PREFIX\}/share/\\$$\{PROJECT_NAME\}\)|set(RESOURCE_DIR $(SIMCOUPE_BIOS_AND_RESOURCES))|g" $(@D)/CMakeLists.txt
	# DOS2UNIX Joystick.cpp - patch system does not support different line endings
	#sed -i -E -e "s|\r$$||g" $(@D)/Base/Joystick.cpp
endef

SIMCOUPE_POST_EXTRACT_HOOKS += SIMCOUPE_POST_EXTRACT_FIX_SDL2_PATH

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
SIMCOUPE_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
SIMCOUPE_DEPENDENCIES += libgles
endif

SIMCOUPE_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
SIMCOUPE_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
SIMCOUPE_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
SIMCOUPE_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
SIMCOUPE_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
SIMCOUPE_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
SIMCOUPE_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
SIMCOUPE_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
SIMCOUPE_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_SO)"
SIMCOUPE_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_SO)"
SIMCOUPE_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_SO)"

$(eval $(cmake-package))
