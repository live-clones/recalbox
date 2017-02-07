################################################################################
#
# recalbox-romfs-gw
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system gw --extension '.zip .ZIP .mgw .MGW' --fullname 'Game and Watch' --platform gw --theme gw libretro:vb:BR2_PACKAGE_LIBRETRO_GW

# Name the 3 vars as the package requires
RECALBOX_ROMFS_GW_SOURCE = 
RECALBOX_ROMFS_GW_SITE = 
RECALBOX_ROMFS_GW_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_GW = gw
SYSTEM_XML_GW = $(@D)/$(SYSTEM_NAME_GW).xml
# System rom path
SOURCE_ROMDIR_GW = $(RECALBOX_ROMFS_GW_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GW),)
define CONFIGURE_MAIN_GW_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GW),Game and Watch,$(SYSTEM_NAME_GW),.zip .ZIP .mgw .MGW,gw,gw)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GW),)
define CONFIGURE_GW_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GW),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_GW),y)
define CONFIGURE_GW_LIBRETRO_VB_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GW),vb)
endef
endif

define CONFIGURE_GW_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_GW))
endef
endif



define CONFIGURE_MAIN_GW_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_GW),$(SOURCE_ROMDIR_GW),$(@D))
endef
endif

define RECALBOX_ROMFS_GW_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_GW_START)
	$(CONFIGURE_GW_LIBRETRO_START)
	$(CONFIGURE_GW_LIBRETRO_VB_DEF)
	$(CONFIGURE_GW_LIBRETRO_END)
	$(CONFIGURE_MAIN_GW_END)
endef

$(eval $(generic-package))
