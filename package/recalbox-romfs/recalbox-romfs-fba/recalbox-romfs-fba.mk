################################################################################
#
# recalbox-romfs-fba
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system fba --extension '.zip .ZIP .fba .FBA' --fullname 'Final Burn Alpha' --platform arcade --theme fba libretro:fba:BR2_PACKAGE_LIBRETRO_FBA fba2x:fba2x:BR2_PACKAGE_PIFBA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_FBA_SOURCE = 
RECALBOX_ROMFS_FBA_SITE = 
RECALBOX_ROMFS_FBA_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_FBA = fba
SYSTEM_XML_FBA = $(@D)/$(SYSTEM_NAME_FBA).xml
# System rom path
SOURCE_ROMDIR_FBA = $(RECALBOX_ROMFS_FBA_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FBA)$(BR2_PACKAGE_PIFBA),)
define CONFIGURE_MAIN_FBA_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_FBA),Final Burn Alpha,$(SYSTEM_NAME_FBA),.zip .ZIP .fba .FBA,arcade,fba)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FBA)$(BR2_PACKAGE_PIFBA),)
define CONFIGURE_FBA_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_FBA),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FBA),y)
define CONFIGURE_FBA_LIBRETRO_FBA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_FBA),fba)
endef
endif

define CONFIGURE_FBA_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_FBA))
endef
endif

ifneq ($(BR2_PACKAGE_LIBRETRO_FBA)$(BR2_PACKAGE_PIFBA),)
define CONFIGURE_FBA_FBA2X_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_FBA),fba2x)
endef
ifeq ($(BR2_PACKAGE_PIFBA),y)
define CONFIGURE_FBA_FBA2X_FBA2X_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_FBA),fba2x)
endef
endif

define CONFIGURE_FBA_FBA2X_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_FBA))
endef
endif



define CONFIGURE_MAIN_FBA_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_FBA),$(SOURCE_ROMDIR_FBA),$(@D))
endef
endif

define RECALBOX_ROMFS_FBA_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_FBA_START)
	$(CONFIGURE_FBA_LIBRETRO_START)
	$(CONFIGURE_FBA_LIBRETRO_FBA_DEF)
	$(CONFIGURE_FBA_LIBRETRO_END)
	$(CONFIGURE_FBA_FBA2X_START)
	$(CONFIGURE_FBA_FBA2X_FBA2X_DEF)
	$(CONFIGURE_FBA_FBA2X_END)
	$(CONFIGURE_MAIN_FBA_END)
endef

$(eval $(generic-package))
