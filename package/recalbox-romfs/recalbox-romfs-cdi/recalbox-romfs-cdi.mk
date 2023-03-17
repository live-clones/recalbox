################################################################################
#
# recalbox-romfs-cdi
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system cdi --extension '.zip .ZIP' --fullname 'Phillips CD I' --platform cdi --theme cdi 1:libretro:mess:BR2_PACKAGE_LIBRETRO_MESS

# Name the 3 vars as the package requires
RECALBOX_ROMFS_CDI_SOURCE = 
RECALBOX_ROMFS_CDI_SITE = 
RECALBOX_ROMFS_CDI_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_CDI = cdi
SYSTEM_XML_CDI = $(@D)/$(SYSTEM_NAME_CDI).xml
# System rom path
SOURCE_ROMDIR_CDI = $(RECALBOX_ROMFS_CDI_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_MESS),)
define CONFIGURE_MAIN_CDI_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_CDI),Phillips CD I,$(SYSTEM_NAME_CDI),.zip .ZIP,cdi,cdi)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_MESS),)
define CONFIGURE_CDI_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_CDI),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MESS),y)
define CONFIGURE_CDI_LIBRETRO_MESS_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_CDI),mess,1)
endef
endif

define CONFIGURE_CDI_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_CDI))
endef
endif



define CONFIGURE_MAIN_CDI_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_CDI),$(SOURCE_ROMDIR_CDI),$(@D))
endef
endif

define RECALBOX_ROMFS_CDI_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_CDI_START)
	$(CONFIGURE_CDI_LIBRETRO_START)
	$(CONFIGURE_CDI_LIBRETRO_MESS_DEF)
	$(CONFIGURE_CDI_LIBRETRO_END)
	$(CONFIGURE_MAIN_CDI_END)
endef

$(eval $(generic-package))
