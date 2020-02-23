################################################################################
#
# recalbox-romfs-tic80
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system tic80 --extension '.tic .TIC .zip .ZIP .7z .7Z' --fullname 'TIC-80' --platform tic80 --theme tic80 1:libretro:tic80:BR2_PACKAGE_LIBRETRO_TIC80

# Name the 3 vars as the package requires
RECALBOX_ROMFS_TIC80_SOURCE = 
RECALBOX_ROMFS_TIC80_SITE = 
RECALBOX_ROMFS_TIC80_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_TIC80 = tic80
SYSTEM_XML_TIC80 = $(@D)/$(SYSTEM_NAME_TIC80).xml
# System rom path
SOURCE_ROMDIR_TIC80 = $(RECALBOX_ROMFS_TIC80_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_TIC80),)
define CONFIGURE_MAIN_TIC80_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_TIC80),TIC-80,$(SYSTEM_NAME_TIC80),.tic .TIC .zip .ZIP .7z .7Z,tic80,tic80)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_TIC80),)
define CONFIGURE_TIC80_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_TIC80),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_TIC80),y)
define CONFIGURE_TIC80_LIBRETRO_TIC80_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_TIC80),tic80,1)
endef
endif

define CONFIGURE_TIC80_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_TIC80))
endef
endif



define CONFIGURE_MAIN_TIC80_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_TIC80),$(SOURCE_ROMDIR_TIC80),$(@D))
endef
endif

define RECALBOX_ROMFS_TIC80_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_TIC80_START)
	$(CONFIGURE_TIC80_LIBRETRO_START)
	$(CONFIGURE_TIC80_LIBRETRO_TIC80_DEF)
	$(CONFIGURE_TIC80_LIBRETRO_END)
	$(CONFIGURE_MAIN_TIC80_END)
endef

$(eval $(generic-package))
