################################################################################
#
# recalbox-romfs-c64
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system c64 --extension '.d64 .D64 .t64 .T64 .x64 .X64' --fullname 'Commodore 64' --platform c64 --theme c64 vice:x64:BR2_PACKAGE_VICE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_C64_SOURCE = 
RECALBOX_ROMFS_C64_SITE = 
RECALBOX_ROMFS_C64_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_C64 = c64
SYSTEM_XML_C64 = $(@D)/$(SYSTEM_NAME_C64).xml
# System rom path
SOURCE_ROMDIR_C64 = $(RECALBOX_ROMFS_C64_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_VICE),)
define CONFIGURE_MAIN_C64_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_C64),Commodore 64,$(SYSTEM_NAME_C64),.d64 .D64 .t64 .T64 .x64 .X64,c64,c64)
endef

ifneq ($(BR2_PACKAGE_VICE),)
define CONFIGURE_C64_VICE_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_C64),vice)
endef
ifeq ($(BR2_PACKAGE_VICE),y)
define CONFIGURE_C64_VICE_X64_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_C64),x64)
endef
endif

define CONFIGURE_C64_VICE_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_C64))
endef
endif



define CONFIGURE_MAIN_C64_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_C64),$(SOURCE_ROMDIR_C64),$(@D))
endef
endif

define RECALBOX_ROMFS_C64_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_C64_START)
	$(CONFIGURE_C64_VICE_START)
	$(CONFIGURE_C64_VICE_X64_DEF)
	$(CONFIGURE_C64_VICE_END)
	$(CONFIGURE_MAIN_C64_END)
endef

$(eval $(generic-package))
