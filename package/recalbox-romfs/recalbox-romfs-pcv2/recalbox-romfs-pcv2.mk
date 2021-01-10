################################################################################
#
# recalbox-romfs-pcv2
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system pcv2 --extension '.ws .wsc .pc2 .zip .7z' --fullname 'Pocket Challenge v2' --platform pcv2 --theme pcv2 1:libretro:mednafen_wswan:BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PCV2_SOURCE = 
RECALBOX_ROMFS_PCV2_SITE = 
RECALBOX_ROMFS_PCV2_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PCV2 = pcv2
SYSTEM_XML_PCV2 = $(@D)/$(SYSTEM_NAME_PCV2).xml
# System rom path
SOURCE_ROMDIR_PCV2 = $(RECALBOX_ROMFS_PCV2_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
define CONFIGURE_MAIN_PCV2_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PCV2),Pocket Challenge v2,$(SYSTEM_NAME_PCV2),.ws .wsc .pc2 .zip .7z,pcv2,pcv2)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
define CONFIGURE_PCV2_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PCV2),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),y)
define CONFIGURE_PCV2_LIBRETRO_MEDNAFEN_WSWAN_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PCV2),mednafen_wswan,1)
endef
endif

define CONFIGURE_PCV2_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PCV2))
endef
endif



define CONFIGURE_MAIN_PCV2_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PCV2),$(SOURCE_ROMDIR_PCV2),$(@D))
endef
endif

define RECALBOX_ROMFS_PCV2_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PCV2_START)
	$(CONFIGURE_PCV2_LIBRETRO_START)
	$(CONFIGURE_PCV2_LIBRETRO_MEDNAFEN_WSWAN_DEF)
	$(CONFIGURE_PCV2_LIBRETRO_END)
	$(CONFIGURE_MAIN_PCV2_END)
endef

$(eval $(generic-package))
