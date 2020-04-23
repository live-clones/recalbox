################################################################################
#
# recalbox-romfs-solarus
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system solarus --extension '.zip .solarus' --fullname 'Solarus' --platform solarus --theme solarus 1:solarus:solarus:BR2_PACKAGE_RECALBOX_SOLARUS

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SOLARUS_SOURCE = 
RECALBOX_ROMFS_SOLARUS_SITE = 
RECALBOX_ROMFS_SOLARUS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SOLARUS = solarus
SYSTEM_XML_SOLARUS = $(@D)/$(SYSTEM_NAME_SOLARUS).xml
# System rom path
SOURCE_ROMDIR_SOLARUS = $(RECALBOX_ROMFS_SOLARUS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_RECALBOX_SOLARUS),)
define CONFIGURE_MAIN_SOLARUS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SOLARUS),Solarus,$(SYSTEM_NAME_SOLARUS),.zip .solarus,solarus,solarus)
endef

ifneq ($(BR2_PACKAGE_RECALBOX_SOLARUS),)
define CONFIGURE_SOLARUS_SOLARUS_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SOLARUS),solarus)
endef
ifeq ($(BR2_PACKAGE_RECALBOX_SOLARUS),y)
define CONFIGURE_SOLARUS_SOLARUS_SOLARUS_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SOLARUS),solarus,1)
endef
endif

define CONFIGURE_SOLARUS_SOLARUS_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SOLARUS))
endef
endif



define CONFIGURE_MAIN_SOLARUS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SOLARUS),$(SOURCE_ROMDIR_SOLARUS),$(@D))
endef
endif

define RECALBOX_ROMFS_SOLARUS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SOLARUS_START)
	$(CONFIGURE_SOLARUS_SOLARUS_START)
	$(CONFIGURE_SOLARUS_SOLARUS_SOLARUS_DEF)
	$(CONFIGURE_SOLARUS_SOLARUS_END)
	$(CONFIGURE_MAIN_SOLARUS_END)
endef

$(eval $(generic-package))
