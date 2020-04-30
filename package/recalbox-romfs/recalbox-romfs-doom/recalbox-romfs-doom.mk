################################################################################
#
# recalbox-romfs-doom
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system doom --extension '.wad .iwad .pwad .lmp .zip .7z' --fullname 'Doom' --platform doom --theme doom 1:libretro:prboom:BR2_PACKAGE_LIBRETRO_PRBOOM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DOOM_SOURCE = 
RECALBOX_ROMFS_DOOM_SITE = 
RECALBOX_ROMFS_DOOM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DOOM = doom
SYSTEM_XML_DOOM = $(@D)/$(SYSTEM_NAME_DOOM).xml
# System rom path
SOURCE_ROMDIR_DOOM = $(RECALBOX_ROMFS_DOOM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),)
define CONFIGURE_MAIN_DOOM_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_DOOM),Doom,$(SYSTEM_NAME_DOOM),.wad .iwad .pwad .lmp .zip .7z,doom,doom)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),)
define CONFIGURE_DOOM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DOOM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),y)
define CONFIGURE_DOOM_LIBRETRO_PRBOOM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DOOM),prboom,1)
endef
endif

define CONFIGURE_DOOM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DOOM))
endef
endif



define CONFIGURE_MAIN_DOOM_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_DOOM),$(SOURCE_ROMDIR_DOOM),$(@D),doom)
endef
endif

define RECALBOX_ROMFS_DOOM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DOOM_START)
	$(CONFIGURE_DOOM_LIBRETRO_START)
	$(CONFIGURE_DOOM_LIBRETRO_PRBOOM_DEF)
	$(CONFIGURE_DOOM_LIBRETRO_END)
	$(CONFIGURE_MAIN_DOOM_END)
endef

$(eval $(generic-package))
