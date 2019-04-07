################################################################################
#
# MGT SAM Coupé Emulation
#
################################################################################
SIMCOUPE_VERSION = 3a2ecdd2ae0d7d55f7682bd777c47afbcd866e96
SIMCOUPE_SITE = $(call github,simonowen,simcoupe,$(SIMCOUPE_VERSION))
SIMCOUPE_DEPENDENCIES = sdl2

SIMCOUPE_BIOS_AND_RESOURCES = /usr/share/simcoupe

define SIMCOUPE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/simcoupe \
		$(TARGET_DIR)/usr/bin/simcoupe
	mkdir -p $(TARGET_DIR)$(SIMCOUPE_BIOS_AND_RESOURCES)
	cp -R $(@D)/Resource/** $(TARGET_DIR)$(SIMCOUPE_BIOS_AND_RESOURCES)
endef

define SIMCOUPE_POST_EXTRACT_FIX_SDL2_PATH
    # Change emulator resource folder
    /bin/sed -i -E -e "s|set\(RESOURCE_DIR \\$$\{CMAKE_INSTALL_PREFIX\}/share/\\$$\{PROJECT_NAME\}\)|set(RESOURCE_DIR $(SIMCOUPE_BIOS_AND_RESOURCES))|g" $(@D)/CMakeLists.txt
    # DOS2UNIX Joystick.cpp - patch system does not support different line endings
    /bin/sed -i -E -e "s|\r$$||g" $(@D)/Base/Joystick.cpp
endef
 
SIMCOUPE_POST_EXTRACT_HOOKS += SIMCOUPE_POST_EXTRACT_FIX_SDL2_PATH

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
SIMCOUPE_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
SIMCOUPE_DEPENDENCIES += libgles
endif

$(eval $(cmake-package))


