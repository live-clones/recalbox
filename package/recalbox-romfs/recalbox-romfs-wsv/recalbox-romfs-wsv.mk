################################################################################
#
# recalbox-romfs-wsv
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system wsv --extension '.sv .zip .7z' --fullname 'Watara Supervision' --platform wsv --theme wsv 1:libretro:potator:BR2_PACKAGE_LIBRETRO_POTATOR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_WSV_SOURCE = 
RECALBOX_ROMFS_WSV_SITE = 
RECALBOX_ROMFS_WSV_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_WSV = wsv
SYSTEM_XML_WSV = $(@D)/$(SYSTEM_NAME_WSV).xml
# System rom path
SOURCE_ROMDIR_WSV = $(RECALBOX_ROMFS_WSV_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_POTATOR),)
define CONFIGURE_MAIN_WSV_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_WSV),Watara Supervision,$(SYSTEM_NAME_WSV),.sv .zip .7z,wsv,wsv)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_POTATOR),)
define CONFIGURE_WSV_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_WSV),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_POTATOR),y)
define CONFIGURE_WSV_LIBRETRO_POTATOR_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_WSV),potator,1)
endef
endif

define CONFIGURE_WSV_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_WSV))
endef
endif



define CONFIGURE_MAIN_WSV_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_WSV),$(SOURCE_ROMDIR_WSV),$(@D))
endef
endif

define RECALBOX_ROMFS_WSV_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_WSV_START)
	$(CONFIGURE_WSV_LIBRETRO_START)
	$(CONFIGURE_WSV_LIBRETRO_POTATOR_DEF)
	$(CONFIGURE_WSV_LIBRETRO_END)
	$(CONFIGURE_MAIN_WSV_END)
endef

$(eval $(generic-package))
