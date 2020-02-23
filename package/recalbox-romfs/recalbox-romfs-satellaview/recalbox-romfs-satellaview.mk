################################################################################
#
# recalbox-romfs-satellaview
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system satellaview --extension '.bs .BS .smc .SMC .sfc .SFC .zip .ZIP .7z .7Z' --fullname 'Satellaview' --platform satellaview --theme satellaview 1:libretro:snes9x:BR2_PACKAGE_LIBRETRO_SNES9X

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SATELLAVIEW_SOURCE = 
RECALBOX_ROMFS_SATELLAVIEW_SITE = 
RECALBOX_ROMFS_SATELLAVIEW_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SATELLAVIEW = satellaview
SYSTEM_XML_SATELLAVIEW = $(@D)/$(SYSTEM_NAME_SATELLAVIEW).xml
# System rom path
SOURCE_ROMDIR_SATELLAVIEW = $(RECALBOX_ROMFS_SATELLAVIEW_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_SNES9X),)
define CONFIGURE_MAIN_SATELLAVIEW_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SATELLAVIEW),Satellaview,$(SYSTEM_NAME_SATELLAVIEW),.bs .BS .smc .SMC .sfc .SFC .zip .ZIP .7z .7Z,satellaview,satellaview)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_SNES9X),)
define CONFIGURE_SATELLAVIEW_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SATELLAVIEW),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_SNES9X),y)
define CONFIGURE_SATELLAVIEW_LIBRETRO_SNES9X_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SATELLAVIEW),snes9x,1)
endef
endif

define CONFIGURE_SATELLAVIEW_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SATELLAVIEW))
endef
endif



define CONFIGURE_MAIN_SATELLAVIEW_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SATELLAVIEW),$(SOURCE_ROMDIR_SATELLAVIEW),$(@D))
endef
endif

define RECALBOX_ROMFS_SATELLAVIEW_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SATELLAVIEW_START)
	$(CONFIGURE_SATELLAVIEW_LIBRETRO_START)
	$(CONFIGURE_SATELLAVIEW_LIBRETRO_SNES9X_DEF)
	$(CONFIGURE_SATELLAVIEW_LIBRETRO_END)
	$(CONFIGURE_MAIN_SATELLAVIEW_END)
endef

$(eval $(generic-package))
