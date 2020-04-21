################################################################################
#
# recalbox-romfs-openbor
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system openbor --extension '.pak .PAK' --fullname 'OpenBOR' --platform openbor --theme openbor 1:openbor:openbor:BR2_PACKAGE_OPENBOR

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


ifneq ($(BR2_PACKAGE_OPENBOR),)
define CONFIGURE_MAIN_OPENBOR_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_OPENBOR),OpenBOR,$(SYSTEM_NAME_OPENBOR),.pak .PAK,openbor,openbor)
endef

ifneq ($(BR2_PACKAGE_OPENBOR),)
define CONFIGURE_OPENBOR_OPENBOR_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_OPENBOR),openbor)
endef
ifeq ($(BR2_PACKAGE_OPENBOR),y)
define CONFIGURE_OPENBOR_OPENBOR_OPENBOR_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_OPENBOR),openbor,1)
endef
endif

define CONFIGURE_OPENBOR_OPENBOR_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_OPENBOR))
endef
endif



define CONFIGURE_MAIN_OPENBOR_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_OPENBOR),$(SOURCE_ROMDIR_OPENBOR),$(@D))
endef
endif

define RECALBOX_ROMFS_OPENBOR_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_OPENBOR_START)
	$(CONFIGURE_OPENBOR_OPENBOR_START)
	$(CONFIGURE_OPENBOR_OPENBOR_OPENBOR_DEF)
	$(CONFIGURE_OPENBOR_OPENBOR_END)
	$(CONFIGURE_MAIN_OPENBOR_END)
endef

$(eval $(generic-package))
