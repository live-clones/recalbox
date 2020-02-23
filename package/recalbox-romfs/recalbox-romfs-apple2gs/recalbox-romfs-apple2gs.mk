################################################################################
#
# recalbox-romfs-apple2gs
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system apple2gs --extension '.2mg .2MG' --fullname 'Apple IIGS' --platform apple2gs --theme apple2gs 1:gsplus:gsplus:BR2_PACKAGE_GSPLUS

# Name the 3 vars as the package requires
RECALBOX_ROMFS_APPLE2GS_SOURCE = 
RECALBOX_ROMFS_APPLE2GS_SITE = 
RECALBOX_ROMFS_APPLE2GS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_APPLE2GS = apple2gs
SYSTEM_XML_APPLE2GS = $(@D)/$(SYSTEM_NAME_APPLE2GS).xml
# System rom path
SOURCE_ROMDIR_APPLE2GS = $(RECALBOX_ROMFS_APPLE2GS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_GSPLUS),)
define CONFIGURE_MAIN_APPLE2GS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_APPLE2GS),Apple IIGS,$(SYSTEM_NAME_APPLE2GS),.2mg .2MG,apple2gs,apple2gs)
endef

ifneq ($(BR2_PACKAGE_GSPLUS),)
define CONFIGURE_APPLE2GS_GSPLUS_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_APPLE2GS),gsplus)
endef
ifeq ($(BR2_PACKAGE_GSPLUS),y)
define CONFIGURE_APPLE2GS_GSPLUS_GSPLUS_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_APPLE2GS),gsplus,1)
endef
endif

define CONFIGURE_APPLE2GS_GSPLUS_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_APPLE2GS))
endef
endif



define CONFIGURE_MAIN_APPLE2GS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_APPLE2GS),$(SOURCE_ROMDIR_APPLE2GS),$(@D))
endef
endif

define RECALBOX_ROMFS_APPLE2GS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_APPLE2GS_START)
	$(CONFIGURE_APPLE2GS_GSPLUS_START)
	$(CONFIGURE_APPLE2GS_GSPLUS_GSPLUS_DEF)
	$(CONFIGURE_APPLE2GS_GSPLUS_END)
	$(CONFIGURE_MAIN_APPLE2GS_END)
endef

$(eval $(generic-package))
