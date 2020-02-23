################################################################################
#
# recalbox-romfs-moonlight
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system moonlight --extension '.moonlight .MOONLIGHT' --fullname 'Moonlight' --platform pc --theme moonlight 1:moonlight:moonlight:BR2_PACKAGE_MOONLIGHT_EMBEDDED

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MOONLIGHT_SOURCE = 
RECALBOX_ROMFS_MOONLIGHT_SITE = 
RECALBOX_ROMFS_MOONLIGHT_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MOONLIGHT = moonlight
SYSTEM_XML_MOONLIGHT = $(@D)/$(SYSTEM_NAME_MOONLIGHT).xml
# System rom path
SOURCE_ROMDIR_MOONLIGHT = $(RECALBOX_ROMFS_MOONLIGHT_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_MOONLIGHT_EMBEDDED),)
define CONFIGURE_MAIN_MOONLIGHT_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MOONLIGHT),Moonlight,$(SYSTEM_NAME_MOONLIGHT),.moonlight .MOONLIGHT,pc,moonlight)
endef

ifneq ($(BR2_PACKAGE_MOONLIGHT_EMBEDDED),)
define CONFIGURE_MOONLIGHT_MOONLIGHT_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MOONLIGHT),moonlight)
endef
ifeq ($(BR2_PACKAGE_MOONLIGHT_EMBEDDED),y)
define CONFIGURE_MOONLIGHT_MOONLIGHT_MOONLIGHT_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MOONLIGHT),moonlight,1)
endef
endif

define CONFIGURE_MOONLIGHT_MOONLIGHT_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MOONLIGHT))
endef
endif



define CONFIGURE_MAIN_MOONLIGHT_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MOONLIGHT),$(SOURCE_ROMDIR_MOONLIGHT),$(@D))
endef
endif

define RECALBOX_ROMFS_MOONLIGHT_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MOONLIGHT_START)
	$(CONFIGURE_MOONLIGHT_MOONLIGHT_START)
	$(CONFIGURE_MOONLIGHT_MOONLIGHT_MOONLIGHT_DEF)
	$(CONFIGURE_MOONLIGHT_MOONLIGHT_END)
	$(CONFIGURE_MAIN_MOONLIGHT_END)
endef

$(eval $(generic-package))
