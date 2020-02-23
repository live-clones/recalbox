################################################################################
#
# recalbox-romfs-sufami
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system sufami --extension '.st .ST .smc .SMC .sfc .SFC .fig .FIG .zip .ZIP .7z .7Z' --fullname 'SuFami Turbo' --platform sufami --theme sufami 1:libretro:snes9x:BR2_PACKAGE_LIBRETRO_SNES9X

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SUFAMI_SOURCE = 
RECALBOX_ROMFS_SUFAMI_SITE = 
RECALBOX_ROMFS_SUFAMI_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SUFAMI = sufami
SYSTEM_XML_SUFAMI = $(@D)/$(SYSTEM_NAME_SUFAMI).xml
# System rom path
SOURCE_ROMDIR_SUFAMI = $(RECALBOX_ROMFS_SUFAMI_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_SNES9X),)
define CONFIGURE_MAIN_SUFAMI_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SUFAMI),SuFami Turbo,$(SYSTEM_NAME_SUFAMI),.st .ST .smc .SMC .sfc .SFC .fig .FIG .zip .ZIP .7z .7Z,sufami,sufami)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_SNES9X),)
define CONFIGURE_SUFAMI_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SUFAMI),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_SNES9X),y)
define CONFIGURE_SUFAMI_LIBRETRO_SNES9X_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SUFAMI),snes9x,1)
endef
endif

define CONFIGURE_SUFAMI_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SUFAMI))
endef
endif



define CONFIGURE_MAIN_SUFAMI_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SUFAMI),$(SOURCE_ROMDIR_SUFAMI),$(@D))
endef
endif

define RECALBOX_ROMFS_SUFAMI_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SUFAMI_START)
	$(CONFIGURE_SUFAMI_LIBRETRO_START)
	$(CONFIGURE_SUFAMI_LIBRETRO_SNES9X_DEF)
	$(CONFIGURE_SUFAMI_LIBRETRO_END)
	$(CONFIGURE_MAIN_SUFAMI_END)
endef

$(eval $(generic-package))
