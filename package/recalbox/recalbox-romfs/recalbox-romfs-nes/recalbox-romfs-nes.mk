################################################################################
#
# recalbox-romfs-nes
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system nes --extension '.nes .NES .zip .ZIP' --fullname 'Nintendo Entertainment System' --platform nes --theme nes libretro:fceumm:BR2_PACKAGE_LIBRETRO_FCEUMM libretro:fceunext:BR2_PACKAGE_LIBRETRO_FCEUNEXT libretro:nestopia:BR2_PACKAGE_LIBRETRO_NESTOPIA libretro:quicknes:BR2_PACKAGE_LIBRETRO_QUICKNES

# Name the 3 vars as the package requires
RECALBOX_ROMFS_NES_SOURCE = 
RECALBOX_ROMFS_NES_SITE = 
RECALBOX_ROMFS_NES_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_NES = nes
SYSTEM_XML_NES = $(@D)/$(SYSTEM_NAME_NES).xml
# System rom path
SOURCE_ROMDIR_NES = $(RECALBOX_ROMFS_NES_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FCEUMM)$(BR2_PACKAGE_LIBRETRO_FCEUNEXT)$(BR2_PACKAGE_LIBRETRO_NESTOPIA)$(BR2_PACKAGE_LIBRETRO_QUICKNES),)
define CONFIGURE_MAIN_NES_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_NES),Nintendo Entertainment System,$(SYSTEM_NAME_NES),.nes .NES .zip .ZIP,nes,nes)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FCEUMM)$(BR2_PACKAGE_LIBRETRO_FCEUNEXT)$(BR2_PACKAGE_LIBRETRO_NESTOPIA)$(BR2_PACKAGE_LIBRETRO_QUICKNES),)
define CONFIGURE_NES_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_NES),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FCEUNEXT),y)
define CONFIGURE_NES_LIBRETRO_FCEUNEXT_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NES),fceunext)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_QUICKNES),y)
define CONFIGURE_NES_LIBRETRO_QUICKNES_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NES),quicknes)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_FCEUMM),y)
define CONFIGURE_NES_LIBRETRO_FCEUMM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NES),fceumm)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_NESTOPIA),y)
define CONFIGURE_NES_LIBRETRO_NESTOPIA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NES),nestopia)
endef
endif

define CONFIGURE_NES_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_NES))
endef
endif



define CONFIGURE_MAIN_NES_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_NES),$(SOURCE_ROMDIR_NES),$(@D))
endef
endif

define RECALBOX_ROMFS_NES_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_NES_START)
	$(CONFIGURE_NES_LIBRETRO_START)
	$(CONFIGURE_NES_LIBRETRO_FCEUNEXT_DEF)
	$(CONFIGURE_NES_LIBRETRO_QUICKNES_DEF)
	$(CONFIGURE_NES_LIBRETRO_FCEUMM_DEF)
	$(CONFIGURE_NES_LIBRETRO_NESTOPIA_DEF)
	$(CONFIGURE_NES_LIBRETRO_END)
	$(CONFIGURE_MAIN_NES_END)
endef

$(eval $(generic-package))
