################################################################################
#
# recalbox-romfs-model3
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system model3 --extension '.zip .ZIP' --fullname 'Sega Model3' --platform model3 --theme model3 1:supermodel:supermodel:BR2_PACKAGE_SUPERMODEL

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MODEL3_SOURCE = 
RECALBOX_ROMFS_MODEL3_SITE = 
RECALBOX_ROMFS_MODEL3_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MODEL3 = model3
SYSTEM_XML_MODEL3 = $(@D)/$(SYSTEM_NAME_MODEL3).xml
# System rom path
SOURCE_ROMDIR_MODEL3 = $(RECALBOX_ROMFS_MODEL3_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_SUPERMODEL),)
define CONFIGURE_MAIN_MODEL3_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MODEL3),Sega Model3,$(SYSTEM_NAME_MODEL3),.zip .ZIP,model3,model3)
endef

ifneq ($(BR2_PACKAGE_SUPERMODEL),)
define CONFIGURE_MODEL3_SUPERMODEL_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MODEL3),supermodel)
endef
ifeq ($(BR2_PACKAGE_SUPERMODEL),y)
define CONFIGURE_MODEL3_SUPERMODEL_SUPERMODEL_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MODEL3),supermodel,1)
endef
endif

define CONFIGURE_MODEL3_SUPERMODEL_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MODEL3))
endef
endif



define CONFIGURE_MAIN_MODEL3_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MODEL3),$(SOURCE_ROMDIR_MODEL3),$(@D))
endef
endif

define RECALBOX_ROMFS_MODEL3_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MODEL3_START)
	$(CONFIGURE_MODEL3_SUPERMODEL_START)
	$(CONFIGURE_MODEL3_SUPERMODEL_SUPERMODEL_DEF)
	$(CONFIGURE_MODEL3_SUPERMODEL_END)
	$(CONFIGURE_MAIN_MODEL3_END)
endef

$(eval $(generic-package))
