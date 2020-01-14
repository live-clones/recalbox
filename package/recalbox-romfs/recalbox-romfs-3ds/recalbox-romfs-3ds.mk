################################################################################
#
# recalbox-romfs-3ds
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system 3ds --extension '.3ds .3DS' --fullname 'Nintendo 3DS' --platform 3ds --theme 3ds BR2_PACKAGE_CITRA_EMU

# Name the 3 vars as the package requires
RECALBOX_ROMFS_3DS_SOURCE = 
RECALBOX_ROMFS_3DS_SITE = 
RECALBOX_ROMFS_3DS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_3DS = 3ds
SYSTEM_XML_3DS = $(@D)/$(SYSTEM_NAME_3DS).xml
# System rom path
SOURCE_ROMDIR_3DS = $(RECALBOX_ROMFS_3DS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_CITRA_EMU),y)
define CONFIGURE_3DS
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_3DS),Nintendo 3DS,$(SYSTEM_NAME_3DS),.3ds .3DS,3ds,3ds,$(SOURCE_ROMDIR_3DS),$(@D))
endef
RECALBOX_ROMFS_3DS_CONFIGURE_CMDS += $(CONFIGURE_3DS)
endif

$(eval $(generic-package))
