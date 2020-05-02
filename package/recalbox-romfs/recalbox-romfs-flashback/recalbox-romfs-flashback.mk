################################################################################
#
# recalbox-romfs-flashback
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system flashback --extension '.map .aba .seq .zip .7z' --fullname 'Flashback' --platform flashback --theme flashback 1:libretro:reminiscence:BR2_PACKAGE_LIBRETRO_REMINISCENCE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_FLASHBACK_SOURCE = 
RECALBOX_ROMFS_FLASHBACK_SITE = 
RECALBOX_ROMFS_FLASHBACK_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_FLASHBACK = flashback
SYSTEM_XML_FLASHBACK = $(@D)/$(SYSTEM_NAME_FLASHBACK).xml
# System rom path
SOURCE_ROMDIR_FLASHBACK = $(RECALBOX_ROMFS_FLASHBACK_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_REMINISCENCE),)
define CONFIGURE_MAIN_FLASHBACK_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_FLASHBACK),Flashback,$(SYSTEM_NAME_FLASHBACK),.map .aba .seq .zip .7z,flashback,flashback)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_REMINISCENCE),)
define CONFIGURE_FLASHBACK_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_FLASHBACK),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_REMINISCENCE),y)
define CONFIGURE_FLASHBACK_LIBRETRO_REMINISCENCE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_FLASHBACK),reminiscence,1)
endef
endif

define CONFIGURE_FLASHBACK_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_FLASHBACK))
endef
endif



define CONFIGURE_MAIN_FLASHBACK_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_FLASHBACK),$(SOURCE_ROMDIR_FLASHBACK),$(@D),flashback)
endef
endif

define RECALBOX_ROMFS_FLASHBACK_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_FLASHBACK_START)
	$(CONFIGURE_FLASHBACK_LIBRETRO_START)
	$(CONFIGURE_FLASHBACK_LIBRETRO_REMINISCENCE_DEF)
	$(CONFIGURE_FLASHBACK_LIBRETRO_END)
	$(CONFIGURE_MAIN_FLASHBACK_END)
endef

$(eval $(generic-package))
