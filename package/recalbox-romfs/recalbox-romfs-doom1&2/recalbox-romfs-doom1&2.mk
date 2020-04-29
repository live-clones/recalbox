################################################################################
#
# recalbox-romfs-doom1&2
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system 'doom1&2' --extension '.wad .iwad .pwad .lmp .zip .7z' --fullname 'Doom 1 & 2' --platform doom --theme doom 1:libretro:prboom:BR2_PACKAGE_LIBRETRO_PRBOOM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DOOM1&2_SOURCE = 
RECALBOX_ROMFS_DOOM1&2_SITE = 
RECALBOX_ROMFS_DOOM1&2_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DOOM1&2 = doom1&2
SYSTEM_XML_DOOM1&2 = $(@D)/$(SYSTEM_NAME_DOOM1&2).xml
# System rom path
SOURCE_ROMDIR_DOOM1&2 = $(RECALBOX_ROMFS_DOOM1&2_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),)
define CONFIGURE_MAIN_DOOM1&2_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_DOOM1&2),Doom 1 & 2,$(SYSTEM_NAME_DOOM1&2),.wad .iwad .pwad .lmp .zip .7z,doom,doom)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),)
define CONFIGURE_DOOM1&2_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DOOM1&2),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),y)
define CONFIGURE_DOOM1&2_LIBRETRO_PRBOOM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DOOM1&2),prboom,1)
endef
endif

define CONFIGURE_DOOM1&2_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DOOM1&2))
endef
endif



define CONFIGURE_MAIN_DOOM1&2_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_DOOM1&2),$(SOURCE_ROMDIR_DOOM1&2),$(@D),doom1&2)
endef
endif

define RECALBOX_ROMFS_DOOM1&2_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DOOM1&2_START)
	$(CONFIGURE_DOOM1&2_LIBRETRO_START)
	$(CONFIGURE_DOOM1&2_LIBRETRO_PRBOOM_DEF)
	$(CONFIGURE_DOOM1&2_LIBRETRO_END)
	$(CONFIGURE_MAIN_DOOM1&2_END)
endef

$(eval $(generic-package))
