################################################################################
#
# recalbox-romfs-daphne
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system daphne --extension '.daphne .DAPHNE' --fullname 'Daphne' --platform daphne --theme daphne 1:daphne:daphne:BR2_PACKAGE_HYPSEUS

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


ifneq ($(BR2_PACKAGE_HYPSEUS),)
define CONFIGURE_MAIN_DAPHNE_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_DAPHNE),Daphne,$(SYSTEM_NAME_DAPHNE),.daphne .DAPHNE,daphne,daphne)
endef

ifneq ($(BR2_PACKAGE_HYPSEUS),)
define CONFIGURE_DAPHNE_DAPHNE_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DAPHNE),daphne)
endef
ifeq ($(BR2_PACKAGE_HYPSEUS),y)
define CONFIGURE_DAPHNE_DAPHNE_DAPHNE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DAPHNE),daphne,1)
endef
endif

define CONFIGURE_DAPHNE_DAPHNE_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DAPHNE))
endef
endif



define CONFIGURE_MAIN_DAPHNE_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_DAPHNE),$(SOURCE_ROMDIR_DAPHNE),$(@D))
endef
endif

define RECALBOX_ROMFS_DAPHNE_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DAPHNE_START)
	$(CONFIGURE_DAPHNE_DAPHNE_START)
	$(CONFIGURE_DAPHNE_DAPHNE_DAPHNE_DEF)
	$(CONFIGURE_DAPHNE_DAPHNE_END)
	$(CONFIGURE_MAIN_DAPHNE_END)
endef

$(eval $(generic-package))
