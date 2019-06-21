################################################################################
#
# recalbox-romfs-openbor
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system openbor --extension '.pak .PAK' --fullname 'OpenBOR' --platform openbor --theme openbor BR2_PACKAGE_OPENBOR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_OPENBOR_SOURCE = 
RECALBOX_ROMFS_OPENBOR_SITE = 
RECALBOX_ROMFS_OPENBOR_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_OPENBOR = openbor
SYSTEM_XML_OPENBOR = $(@D)/$(SYSTEM_NAME_OPENBOR).xml
# System rom path
SOURCE_ROMDIR_OPENBOR = $(RECALBOX_ROMFS_OPENBOR_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_OPENBOR),y)
define CONFIGURE_OPENBOR
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_OPENBOR),OpenBOR,$(SYSTEM_NAME_OPENBOR),.pak .PAK,openbor,openbor,$(SOURCE_ROMDIR_OPENBOR),$(@D))
endef
RECALBOX_ROMFS_OPENBOR_CONFIGURE_CMDS += $(CONFIGURE_OPENBOR)
endif

$(eval $(generic-package))
