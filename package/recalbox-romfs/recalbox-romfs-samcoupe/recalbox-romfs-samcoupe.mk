################################################################################
#
# recalbox-romfs-samcoupe
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system samcoupe --extension '.dsk .DSK .mgt .MGT .sbt .SBT .sad .SAD' --fullname 'MGT SAM Coupé' --platform samcoupe --theme samcoupe BR2_PACKAGE_SIMCOUPE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SAMCOUPE_SOURCE = 
RECALBOX_ROMFS_SAMCOUPE_SITE = 
RECALBOX_ROMFS_SAMCOUPE_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SAMCOUPE = samcoupe
SYSTEM_XML_SAMCOUPE = $(@D)/$(SYSTEM_NAME_SAMCOUPE).xml
# System rom path
SOURCE_ROMDIR_SAMCOUPE = $(RECALBOX_ROMFS_SAMCOUPE_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_SIMCOUPE),y)
define CONFIGURE_SAMCOUPE
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_SAMCOUPE),MGT SAM Coupé,$(SYSTEM_NAME_SAMCOUPE),.dsk .DSK .mgt .MGT .sbt .SBT .sad .SAD,samcoupe,samcoupe,$(SOURCE_ROMDIR_SAMCOUPE),$(@D))
endef
RECALBOX_ROMFS_SAMCOUPE_CONFIGURE_CMDS += $(CONFIGURE_SAMCOUPE)
endif

$(eval $(generic-package))
