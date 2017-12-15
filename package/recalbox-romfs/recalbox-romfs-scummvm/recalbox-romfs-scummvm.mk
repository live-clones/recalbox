################################################################################
#
# recalbox-romfs-scummvm
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system scummvm --extension '.scummvm' --fullname 'ScummVM' --platform scummvm --theme scummvm BR2_PACKAGE_SCUMMVM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SCUMMVM_SOURCE = 
RECALBOX_ROMFS_SCUMMVM_SITE = 
RECALBOX_ROMFS_SCUMMVM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SCUMMVM = scummvm
SYSTEM_XML_SCUMMVM = $(@D)/$(SYSTEM_NAME_SCUMMVM).xml
# System rom path
SOURCE_ROMDIR_SCUMMVM = $(RECALBOX_ROMFS_SCUMMVM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_SCUMMVM)$(BR2_PACKAGE_RESIDUALVM),)
define CONFIGURE_MAIN_SCUMMVM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SCUMMVM),ScummVM,$(SYSTEM_NAME_SCUMMVM),.scummvm,scummvm,scummvm)
endef

ifeq ($(BR2_PACKAGE_SCUMMVM),y)
define CONFIGURE_SCUMMVM_SCUMMVM_DEF
        $(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SCUMMVM),scummvm)
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SCUMMVM),scummvm)
        $(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SCUMMVM))
endef
endif

ifeq ($(BR2_PACKAGE_RESIDUALVM),y)
define CONFIGURE_SCUMMVM_RESIDUALVM_DEF
        $(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SCUMMVM),residualvm)
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SCUMMVM),residualvm)
        $(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SCUMMVM))
endef
endif

define CONFIGURE_MAIN_SCUMMVM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SCUMMVM),$(SOURCE_ROMDIR_SCUMMVM),$(@D))
endef
endif


define RECALBOX_ROMFS_SCUMMVM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SCUMMVM_START)
	$(CONFIGURE_SCUMMVM_SCUMMVM_DEF)
	$(CONFIGURE_SCUMMVM_RESIDUALVM_DEF)
	$(CONFIGURE_MAIN_SCUMMVM_END)
endef

$(eval $(generic-package))
