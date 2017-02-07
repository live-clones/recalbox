################################################################################
#
# recalbox-romfs-virtualboy
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system virtualboy --extension '.vb .VB .zip .ZIP' --fullname 'Virtual Boy' --platform virtualboy --theme virtualboy libretro:mednafen_vb:BR2_PACKAGE_LIBRETRO_BEETLE_VB

# Name the 3 vars as the package requires
RECALBOX_ROMFS_VIRTUALBOY_SOURCE = 
RECALBOX_ROMFS_VIRTUALBOY_SITE = 
RECALBOX_ROMFS_VIRTUALBOY_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_VIRTUALBOY = virtualboy
SYSTEM_XML_VIRTUALBOY = $(@D)/$(SYSTEM_NAME_VIRTUALBOY).xml
# System rom path
SOURCE_ROMDIR_VIRTUALBOY = $(RECALBOX_ROMFS_VIRTUALBOY_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_VB),)
define CONFIGURE_MAIN_VIRTUALBOY_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_VIRTUALBOY),Virtual Boy,$(SYSTEM_NAME_VIRTUALBOY),.vb .VB .zip .ZIP,virtualboy,virtualboy)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_VB),)
define CONFIGURE_VIRTUALBOY_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_VIRTUALBOY),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_VB),y)
define CONFIGURE_VIRTUALBOY_LIBRETRO_MEDNAFEN_VB_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_VIRTUALBOY),mednafen_vb)
endef
endif

define CONFIGURE_VIRTUALBOY_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_VIRTUALBOY))
endef
endif



define CONFIGURE_MAIN_VIRTUALBOY_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_VIRTUALBOY),$(SOURCE_ROMDIR_VIRTUALBOY),$(@D))
endef
endif

define RECALBOX_ROMFS_VIRTUALBOY_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_VIRTUALBOY_START)
	$(CONFIGURE_VIRTUALBOY_LIBRETRO_START)
	$(CONFIGURE_VIRTUALBOY_LIBRETRO_MEDNAFEN_VB_DEF)
	$(CONFIGURE_VIRTUALBOY_LIBRETRO_END)
	$(CONFIGURE_MAIN_VIRTUALBOY_END)
endef

$(eval $(generic-package))
