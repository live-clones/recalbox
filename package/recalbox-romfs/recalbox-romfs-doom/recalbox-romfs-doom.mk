################################################################################
#
# recalbox-romfs-doom
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system doom --extension '.wad .WAD .iwad .IWAD .pwad .PWAD .lmp .LMP' --fullname 'Doom' --platform doom --theme doom libretro:prboom:BR2_PACKAGE_LIBRETRO_PRBOOM libretro:dhewm3:BR2_PACKAGE_LIBRETRO_DHEWM3

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


ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM)$(BR2_PACKAGE_LIBRETRO_DHEWM3),)
define CONFIGURE_MAIN_DOOM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_DOOM),Doom,$(SYSTEM_NAME_DOOM),.wad .WAD .iwad .IWAD .pwad .PWAD .lmp .LMP,doom,doom)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM)$(BR2_PACKAGE_LIBRETRO_DHEWM3),)
define CONFIGURE_DOOM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DOOM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),y)
define CONFIGURE_DOOM_LIBRETRO_PRBOOM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DOOM),prboom)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_DHEWM3),y)
define CONFIGURE_DOOM_LIBRETRO_DHEWM3_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DOOM),dhewm3)
endef
endif

define CONFIGURE_DOOM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DOOM))
endef
endif



define CONFIGURE_MAIN_DOOM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_DOOM),$(SOURCE_ROMDIR_DOOM),$(@D))
endef
endif

define RECALBOX_ROMFS_DOOM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DOOM_START)
	$(CONFIGURE_DOOM_LIBRETRO_START)
	$(CONFIGURE_DOOM_LIBRETRO_PRBOOM_DEF)
	$(CONFIGURE_DOOM_LIBRETRO_DHEWM3_DEF)
	$(CONFIGURE_DOOM_LIBRETRO_END)
	$(CONFIGURE_MAIN_DOOM_END)
endef

$(eval $(generic-package))
