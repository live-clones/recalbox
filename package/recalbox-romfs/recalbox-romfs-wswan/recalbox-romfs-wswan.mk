################################################################################
#
# recalbox-romfs-wswan
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system wswan --extension '.ws .WS .wsc .WSC .zip .ZIP' --fullname 'WonderSwan' --platform wonderswan --theme wonderswan libretro:mednafen_wswan:BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN

# Name the 3 vars as the package requires
RECALBOX_ROMFS_WSWAN_SOURCE = 
RECALBOX_ROMFS_WSWAN_SITE = 
RECALBOX_ROMFS_WSWAN_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_WSWAN = wswan
SYSTEM_XML_WSWAN = $(@D)/$(SYSTEM_NAME_WSWAN).xml
# System rom path
SOURCE_ROMDIR_WSWAN = $(RECALBOX_ROMFS_WSWAN_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
define CONFIGURE_MAIN_WSWAN_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_WSWAN),WonderSwan,$(SYSTEM_NAME_WSWAN),.ws .WS .wsc .WSC .zip .ZIP,wonderswan,wonderswan)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
define CONFIGURE_WSWAN_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_WSWAN),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),y)
define CONFIGURE_WSWAN_LIBRETRO_MEDNAFEN_WSWAN_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_WSWAN),mednafen_wswan)
endef
endif

define CONFIGURE_WSWAN_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_WSWAN))
endef
endif



define CONFIGURE_MAIN_WSWAN_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_WSWAN),$(SOURCE_ROMDIR_WSWAN),$(@D))
endef
endif

define RECALBOX_ROMFS_WSWAN_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_WSWAN_START)
	$(CONFIGURE_WSWAN_LIBRETRO_START)
	$(CONFIGURE_WSWAN_LIBRETRO_MEDNAFEN_WSWAN_DEF)
	$(CONFIGURE_WSWAN_LIBRETRO_END)
	$(CONFIGURE_MAIN_WSWAN_END)
endef

$(eval $(generic-package))
