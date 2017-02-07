################################################################################
#
# recalbox-romfs-wswanc
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system wswanc --extension '.ws .WS .wsc .WSC .zip .ZIP' --fullname 'WonderSwan Color' --platform wonderswancolor --theme wonderswancolor libretro:mednafen_wswan:BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN

# Name the 3 vars as the package requires
RECALBOX_ROMFS_WSWANC_SOURCE = 
RECALBOX_ROMFS_WSWANC_SITE = 
RECALBOX_ROMFS_WSWANC_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_WSWANC = wswanc
SYSTEM_XML_WSWANC = $(@D)/$(SYSTEM_NAME_WSWANC).xml
# System rom path
SOURCE_ROMDIR_WSWANC = $(RECALBOX_ROMFS_WSWANC_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
define CONFIGURE_MAIN_WSWANC_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_WSWANC),WonderSwan Color,$(SYSTEM_NAME_WSWANC),.ws .WS .wsc .WSC .zip .ZIP,wonderswancolor,wonderswancolor)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),)
define CONFIGURE_WSWANC_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_WSWANC),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_WSWAN),y)
define CONFIGURE_WSWANC_LIBRETRO_MEDNAFEN_WSWAN_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_WSWANC),mednafen_wswan)
endef
endif

define CONFIGURE_WSWANC_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_WSWANC))
endef
endif



define CONFIGURE_MAIN_WSWANC_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_WSWANC),$(SOURCE_ROMDIR_WSWANC),$(@D))
endef
endif

define RECALBOX_ROMFS_WSWANC_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_WSWANC_START)
	$(CONFIGURE_WSWANC_LIBRETRO_START)
	$(CONFIGURE_WSWANC_LIBRETRO_MEDNAFEN_WSWAN_DEF)
	$(CONFIGURE_WSWANC_LIBRETRO_END)
	$(CONFIGURE_MAIN_WSWANC_END)
endef

$(eval $(generic-package))
