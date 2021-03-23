################################################################################
#
# recalbox-romfs-supervision
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system supervision --extension '.sv .zip .7z' --fullname 'Watara Supervision' --platform wsv --theme supervision 1:libretro:potator:BR2_PACKAGE_LIBRETRO_POTATOR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SUPERVISION_SOURCE = 
RECALBOX_ROMFS_SUPERVISION_SITE = 
RECALBOX_ROMFS_SUPERVISION_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SUPERVISION = supervision
SYSTEM_XML_SUPERVISION = $(@D)/$(SYSTEM_NAME_SUPERVISION).xml
# System rom path
SOURCE_ROMDIR_SUPERVISION = $(RECALBOX_ROMFS_SUPERVISION_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_POTATOR),)
define CONFIGURE_MAIN_SUPERVISION_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SUPERVISION),Watara Supervision,$(SYSTEM_NAME_SUPERVISION),.sv .zip .7z,wsv,supervision)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_POTATOR),)
define CONFIGURE_SUPERVISION_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SUPERVISION),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_POTATOR),y)
define CONFIGURE_SUPERVISION_LIBRETRO_POTATOR_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SUPERVISION),potator,1)
endef
endif

define CONFIGURE_SUPERVISION_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SUPERVISION))
endef
endif



define CONFIGURE_MAIN_SUPERVISION_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SUPERVISION),$(SOURCE_ROMDIR_SUPERVISION),$(@D))
endef
endif

define RECALBOX_ROMFS_SUPERVISION_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SUPERVISION_START)
	$(CONFIGURE_SUPERVISION_LIBRETRO_START)
	$(CONFIGURE_SUPERVISION_LIBRETRO_POTATOR_DEF)
	$(CONFIGURE_SUPERVISION_LIBRETRO_END)
	$(CONFIGURE_MAIN_SUPERVISION_END)
endef

$(eval $(generic-package))
