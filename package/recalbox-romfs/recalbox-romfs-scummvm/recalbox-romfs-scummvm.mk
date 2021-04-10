################################################################################
#
# recalbox-romfs-scummvm
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system scummvm --extension '.scummvm .residualvm' --fullname 'ScummVM' --platform scummvm --theme scummvm 1:scummvm:scummvm:BR2_PACKAGE_SCUMMVM 2:libretro:scummvm:BR2_PACKAGE_LIBRETRO_SCUMMVM

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


ifneq ($(BR2_PACKAGE_SCUMMVM)$(BR2_PACKAGE_LIBRETRO_SCUMMVM),)
define CONFIGURE_MAIN_SCUMMVM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SCUMMVM),ScummVM,$(SYSTEM_NAME_SCUMMVM),.scummvm .residualvm,scummvm,scummvm)
endef

ifneq ($(BR2_PACKAGE_SCUMMVM)$(BR2_PACKAGE_LIBRETRO_SCUMMVM),)
define CONFIGURE_SCUMMVM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SCUMMVM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_SCUMMVM),y)
define CONFIGURE_SCUMMVM_LIBRETRO_SCUMMVM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SCUMMVM),scummvm,2)
endef
endif

define CONFIGURE_SCUMMVM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SCUMMVM))
endef
endif

ifneq ($(BR2_PACKAGE_SCUMMVM)$(BR2_PACKAGE_LIBRETRO_SCUMMVM),)
define CONFIGURE_SCUMMVM_SCUMMVM_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SCUMMVM),scummvm)
endef
ifeq ($(BR2_PACKAGE_SCUMMVM),y)
define CONFIGURE_SCUMMVM_SCUMMVM_SCUMMVM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SCUMMVM),scummvm,1)
endef
endif

define CONFIGURE_SCUMMVM_SCUMMVM_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SCUMMVM))
endef
endif



define CONFIGURE_MAIN_SCUMMVM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SCUMMVM),$(SOURCE_ROMDIR_SCUMMVM),$(@D))
endef
endif

define RECALBOX_ROMFS_SCUMMVM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SCUMMVM_START)
	$(CONFIGURE_SCUMMVM_LIBRETRO_START)
	$(CONFIGURE_SCUMMVM_LIBRETRO_SCUMMVM_DEF)
	$(CONFIGURE_SCUMMVM_LIBRETRO_END)
	$(CONFIGURE_SCUMMVM_SCUMMVM_START)
	$(CONFIGURE_SCUMMVM_SCUMMVM_SCUMMVM_DEF)
	$(CONFIGURE_SCUMMVM_SCUMMVM_END)
	$(CONFIGURE_MAIN_SCUMMVM_END)
endef

$(eval $(generic-package))
