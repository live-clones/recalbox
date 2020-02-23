################################################################################
#
# recalbox-romfs-pc98
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system pc98 --extension '.d98 .D98 .98d .98D .fdi .FDI .fdd .FDD .2hd .2HD .tfd .TFD .d88 .D88 .88d .88D .hdm .HDM .xdf .XDF .dup .DUP .cmd .CMD .hdi .HDI .thd .THD .nhd .NHD .hdd .HDD .hdn .HDN .zip .ZIP .7z .7Z' --fullname 'NEC PC-98' --platform pc98 --theme pc98 1:libretro:np2kai:BR2_PACKAGE_LIBRETRO_NP2KAI

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PC98_SOURCE = 
RECALBOX_ROMFS_PC98_SITE = 
RECALBOX_ROMFS_PC98_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PC98 = pc98
SYSTEM_XML_PC98 = $(@D)/$(SYSTEM_NAME_PC98).xml
# System rom path
SOURCE_ROMDIR_PC98 = $(RECALBOX_ROMFS_PC98_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_NP2KAI),)
define CONFIGURE_MAIN_PC98_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PC98),NEC PC-98,$(SYSTEM_NAME_PC98),.d98 .D98 .98d .98D .fdi .FDI .fdd .FDD .2hd .2HD .tfd .TFD .d88 .D88 .88d .88D .hdm .HDM .xdf .XDF .dup .DUP .cmd .CMD .hdi .HDI .thd .THD .nhd .NHD .hdd .HDD .hdn .HDN .zip .ZIP .7z .7Z,pc98,pc98)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_NP2KAI),)
define CONFIGURE_PC98_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PC98),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_NP2KAI),y)
define CONFIGURE_PC98_LIBRETRO_NP2KAI_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PC98),np2kai,1)
endef
endif

define CONFIGURE_PC98_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PC98))
endef
endif



define CONFIGURE_MAIN_PC98_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PC98),$(SOURCE_ROMDIR_PC98),$(@D))
endef
endif

define RECALBOX_ROMFS_PC98_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PC98_START)
	$(CONFIGURE_PC98_LIBRETRO_START)
	$(CONFIGURE_PC98_LIBRETRO_NP2KAI_DEF)
	$(CONFIGURE_PC98_LIBRETRO_END)
	$(CONFIGURE_MAIN_PC98_END)
endef

$(eval $(generic-package))