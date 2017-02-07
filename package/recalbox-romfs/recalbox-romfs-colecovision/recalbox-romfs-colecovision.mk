################################################################################
#
# recalbox-romfs-colecovision
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system colecovision --extension '.col .COL .zip .ZIP' --fullname 'Colecovision' --platform colecovision --theme colecovision libretro:bluemsx:BR2_PACKAGE_LIBRETRO_BLUEMSX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_COLECOVISION_SOURCE = 
RECALBOX_ROMFS_COLECOVISION_SITE = 
RECALBOX_ROMFS_COLECOVISION_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_COLECOVISION = colecovision
SYSTEM_XML_COLECOVISION = $(@D)/$(SYSTEM_NAME_COLECOVISION).xml
# System rom path
SOURCE_ROMDIR_COLECOVISION = $(RECALBOX_ROMFS_COLECOVISION_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),)
define CONFIGURE_MAIN_COLECOVISION_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_COLECOVISION),Colecovision,$(SYSTEM_NAME_COLECOVISION),.col .COL .zip .ZIP,colecovision,colecovision)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),)
define CONFIGURE_COLECOVISION_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_COLECOVISION),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),y)
define CONFIGURE_COLECOVISION_LIBRETRO_BLUEMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_COLECOVISION),bluemsx)
endef
endif

define CONFIGURE_COLECOVISION_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_COLECOVISION))
endef
endif



define CONFIGURE_MAIN_COLECOVISION_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_COLECOVISION),$(SOURCE_ROMDIR_COLECOVISION),$(@D))
endef
endif

define RECALBOX_ROMFS_COLECOVISION_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_COLECOVISION_START)
	$(CONFIGURE_COLECOVISION_LIBRETRO_START)
	$(CONFIGURE_COLECOVISION_LIBRETRO_BLUEMSX_DEF)
	$(CONFIGURE_COLECOVISION_LIBRETRO_END)
	$(CONFIGURE_MAIN_COLECOVISION_END)
endef

$(eval $(generic-package))
