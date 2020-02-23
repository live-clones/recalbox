################################################################################
#
# recalbox-romfs-nds
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system nds --extension '.nds .NDS .zip .ZIP' --fullname 'Nintendo DS' --platform nds --theme nds 1:libretro:desmume:BR2_PACKAGE_LIBRETRO_DESMUME 2:libretro:melonds:BR2_PACKAGE_LIBRETRO_MELONDS

# Name the 3 vars as the package requires
RECALBOX_ROMFS_NDS_SOURCE = 
RECALBOX_ROMFS_NDS_SITE = 
RECALBOX_ROMFS_NDS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_NDS = nds
SYSTEM_XML_NDS = $(@D)/$(SYSTEM_NAME_NDS).xml
# System rom path
SOURCE_ROMDIR_NDS = $(RECALBOX_ROMFS_NDS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_DESMUME)$(BR2_PACKAGE_LIBRETRO_MELONDS),)
define CONFIGURE_MAIN_NDS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_NDS),Nintendo DS,$(SYSTEM_NAME_NDS),.nds .NDS .zip .ZIP,nds,nds)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_DESMUME)$(BR2_PACKAGE_LIBRETRO_MELONDS),)
define CONFIGURE_NDS_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_NDS),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_DESMUME),y)
define CONFIGURE_NDS_LIBRETRO_DESMUME_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NDS),desmume,1)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_MELONDS),y)
define CONFIGURE_NDS_LIBRETRO_MELONDS_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NDS),melonds,2)
endef
endif

define CONFIGURE_NDS_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_NDS))
endef
endif



define CONFIGURE_MAIN_NDS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_NDS),$(SOURCE_ROMDIR_NDS),$(@D))
endef
endif

define RECALBOX_ROMFS_NDS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_NDS_START)
	$(CONFIGURE_NDS_LIBRETRO_START)
	$(CONFIGURE_NDS_LIBRETRO_DESMUME_DEF)
	$(CONFIGURE_NDS_LIBRETRO_MELONDS_DEF)
	$(CONFIGURE_NDS_LIBRETRO_END)
	$(CONFIGURE_MAIN_NDS_END)
endef

$(eval $(generic-package))
