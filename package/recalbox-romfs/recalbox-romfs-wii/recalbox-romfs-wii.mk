################################################################################
#
# recalbox-romfs-wii
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system wii --extension '.iso .ISO .wbfs .WBFS .wad .WAD' --fullname 'Wii' --platform wii --theme wii 1:dolphin:dolphin:BR2_PACKAGE_DOLPHIN_EMU

# Name the 3 vars as the package requires
RECALBOX_ROMFS_WII_SOURCE = 
RECALBOX_ROMFS_WII_SITE = 
RECALBOX_ROMFS_WII_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_WII = wii
SYSTEM_XML_WII = $(@D)/$(SYSTEM_NAME_WII).xml
# System rom path
SOURCE_ROMDIR_WII = $(RECALBOX_ROMFS_WII_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_DOLPHIN_EMU),)
define CONFIGURE_MAIN_WII_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_WII),Wii,$(SYSTEM_NAME_WII),.iso .ISO .wbfs .WBFS .wad .WAD,wii,wii)
endef

ifneq ($(BR2_PACKAGE_DOLPHIN_EMU),)
define CONFIGURE_WII_DOLPHIN_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_WII),dolphin)
endef
ifeq ($(BR2_PACKAGE_DOLPHIN_EMU),y)
define CONFIGURE_WII_DOLPHIN_DOLPHIN_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_WII),dolphin,1)
endef
endif

define CONFIGURE_WII_DOLPHIN_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_WII))
endef
endif



define CONFIGURE_MAIN_WII_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_WII),$(SOURCE_ROMDIR_WII),$(@D))
endef
endif

define RECALBOX_ROMFS_WII_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_WII_START)
	$(CONFIGURE_WII_DOLPHIN_START)
	$(CONFIGURE_WII_DOLPHIN_DOLPHIN_DEF)
	$(CONFIGURE_WII_DOLPHIN_END)
	$(CONFIGURE_MAIN_WII_END)
endef

$(eval $(generic-package))
