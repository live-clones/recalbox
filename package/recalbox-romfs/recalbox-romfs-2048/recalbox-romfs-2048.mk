################################################################################
#
# recalbox-romfs-2048
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system 2048 --extension '.game' --fullname '2048' --platform 2048 --theme 2048 1:libretro:2048:BR2_PACKAGE_LIBRETRO_2048

# Name the 3 vars as the package requires
RECALBOX_ROMFS_2048_SOURCE = 
RECALBOX_ROMFS_2048_SITE = 
RECALBOX_ROMFS_2048_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_2048 = 2048
SYSTEM_XML_2048 = $(@D)/$(SYSTEM_NAME_2048).xml
# System rom path
SOURCE_ROMDIR_2048 = $(RECALBOX_ROMFS_2048_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_2048),)
define CONFIGURE_MAIN_2048_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_2048),2048,$(SYSTEM_NAME_2048),.game,2048,2048)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_2048),)
define CONFIGURE_2048_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_2048),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_2048),y)
define CONFIGURE_2048_LIBRETRO_2048_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_2048),2048,1)
endef
endif

define CONFIGURE_2048_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_2048))
endef
endif



define CONFIGURE_MAIN_2048_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_2048),$(SOURCE_ROMDIR_2048),$(@D),2048)
endef
endif

define RECALBOX_ROMFS_2048_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_2048_START)
	$(CONFIGURE_2048_LIBRETRO_START)
	$(CONFIGURE_2048_LIBRETRO_2048_DEF)
	$(CONFIGURE_2048_LIBRETRO_END)
	$(CONFIGURE_MAIN_2048_END)
endef

$(eval $(generic-package))
