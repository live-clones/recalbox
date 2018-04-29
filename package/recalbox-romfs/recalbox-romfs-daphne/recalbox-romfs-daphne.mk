################################################################################
#
# recalbox-romfs-daphne
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system daphne --extension '.daphne .DAPHNE' --fullname 'Daphne' --platform daphne --theme daphne BR2_PACKAGE_HYPSEUS

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DAPHNE_SOURCE = 
RECALBOX_ROMFS_DAPHNE_SITE = 
RECALBOX_ROMFS_DAPHNE_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DAPHNE = daphne
SYSTEM_XML_DAPHNE = $(@D)/$(SYSTEM_NAME_DAPHNE).xml
# System rom path
SOURCE_ROMDIR_DAPHNE = $(RECALBOX_ROMFS_DAPHNE_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_HYPSEUS),y)
define CONFIGURE_DAPHNE
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_DAPHNE),Daphne,$(SYSTEM_NAME_DAPHNE),.daphne .DAPHNE,daphne,daphne,$(SOURCE_ROMDIR_DAPHNE),$(@D))
endef
RECALBOX_ROMFS_DAPHNE_CONFIGURE_CMDS += $(CONFIGURE_DAPHNE)
endif

$(eval $(generic-package))
