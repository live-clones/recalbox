################################################################################
#
# recalbox-romfs-samcoupe
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system samcoupe --extension '.dsk .DSK .mgt .MGT .sbt .SBT .sad .SAD' --fullname 'MGT SAM Coupé' --platform samcoupe --theme samcoupe 1:simcoupe:simcoupe:BR2_PACKAGE_SIMCOUPE

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


ifneq ($(BR2_PACKAGE_SIMCOUPE),)
define CONFIGURE_MAIN_SAMCOUPE_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SAMCOUPE),MGT SAM Coupé,$(SYSTEM_NAME_SAMCOUPE),.dsk .DSK .mgt .MGT .sbt .SBT .sad .SAD,samcoupe,samcoupe)
endef

ifneq ($(BR2_PACKAGE_SIMCOUPE),)
define CONFIGURE_SAMCOUPE_SIMCOUPE_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SAMCOUPE),simcoupe)
endef
ifeq ($(BR2_PACKAGE_SIMCOUPE),y)
define CONFIGURE_SAMCOUPE_SIMCOUPE_SIMCOUPE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SAMCOUPE),simcoupe,1)
endef
endif

define CONFIGURE_SAMCOUPE_SIMCOUPE_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SAMCOUPE))
endef
endif



define CONFIGURE_MAIN_SAMCOUPE_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SAMCOUPE),$(SOURCE_ROMDIR_SAMCOUPE),$(@D))
endef
endif

define RECALBOX_ROMFS_SAMCOUPE_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SAMCOUPE_START)
	$(CONFIGURE_SAMCOUPE_SIMCOUPE_START)
	$(CONFIGURE_SAMCOUPE_SIMCOUPE_SIMCOUPE_DEF)
	$(CONFIGURE_SAMCOUPE_SIMCOUPE_END)
	$(CONFIGURE_MAIN_SAMCOUPE_END)
endef

$(eval $(generic-package))
