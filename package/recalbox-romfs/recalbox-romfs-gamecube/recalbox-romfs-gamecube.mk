################################################################################
#
# recalbox-romfs-gamecube
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system gamecube --extension '.iso .ISO .gc .GC .gcz .GCZ .gcm .GCM' --fullname 'GameCube' --platform gc --theme gc 1:dolphin:dolphin:BR2_PACKAGE_DOLPHIN_EMU

# Name the 3 vars as the package requires
RECALBOX_ROMFS_GAMECUBE_SOURCE = 
RECALBOX_ROMFS_GAMECUBE_SITE = 
RECALBOX_ROMFS_GAMECUBE_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_GAMECUBE = gamecube
SYSTEM_XML_GAMECUBE = $(@D)/$(SYSTEM_NAME_GAMECUBE).xml
# System rom path
SOURCE_ROMDIR_GAMECUBE = $(RECALBOX_ROMFS_GAMECUBE_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_DOLPHIN_EMU),)
define CONFIGURE_MAIN_GAMECUBE_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GAMECUBE),GameCube,$(SYSTEM_NAME_GAMECUBE),.iso .ISO .gc .GC .gcz .GCZ .gcm .GCM,gc,gc)
endef

ifneq ($(BR2_PACKAGE_DOLPHIN_EMU),)
define CONFIGURE_GAMECUBE_DOLPHIN_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GAMECUBE),dolphin)
endef
ifeq ($(BR2_PACKAGE_DOLPHIN_EMU),y)
define CONFIGURE_GAMECUBE_DOLPHIN_DOLPHIN_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GAMECUBE),dolphin,1)
endef
endif

define CONFIGURE_GAMECUBE_DOLPHIN_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_GAMECUBE))
endef
endif



define CONFIGURE_MAIN_GAMECUBE_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_GAMECUBE),$(SOURCE_ROMDIR_GAMECUBE),$(@D))
endef
endif

define RECALBOX_ROMFS_GAMECUBE_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_GAMECUBE_START)
	$(CONFIGURE_GAMECUBE_DOLPHIN_START)
	$(CONFIGURE_GAMECUBE_DOLPHIN_DOLPHIN_DEF)
	$(CONFIGURE_GAMECUBE_DOLPHIN_END)
	$(CONFIGURE_MAIN_GAMECUBE_END)
endef

$(eval $(generic-package))
