################################################################################
#
# recalbox-romfs-lynx
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system lynx --extension '.zip .ZIP .lnx .LNX' --fullname 'Lynx' --platform atarilynx --theme lynx libretro:mednafen_lynx:BR2_PACKAGE_LIBRETRO_BEETLE_LYNX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_LYNX_SOURCE = 
RECALBOX_ROMFS_LYNX_SITE = 
RECALBOX_ROMFS_LYNX_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_LYNX = lynx
SYSTEM_XML_LYNX = $(@D)/$(SYSTEM_NAME_LYNX).xml
# System rom path
SOURCE_ROMDIR_LYNX = $(RECALBOX_ROMFS_LYNX_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_LYNX),)
define CONFIGURE_MAIN_LYNX_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_LYNX),Lynx,$(SYSTEM_NAME_LYNX),.zip .ZIP .lnx .LNX,atarilynx,lynx)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_LYNX),)
define CONFIGURE_LYNX_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_LYNX),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_LYNX),y)
define CONFIGURE_LYNX_LIBRETRO_ATARILYNX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_LYNX),mednafen_lynx)
endef
endif

define CONFIGURE_LYNX_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_LYNX))
endef
endif



define CONFIGURE_MAIN_LYNX_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_LYNX),$(SOURCE_ROMDIR_LYNX),$(@D))
endef
endif

define RECALBOX_ROMFS_LYNX_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_LYNX_START)
	$(CONFIGURE_LYNX_LIBRETRO_START)
	$(CONFIGURE_LYNX_LIBRETRO_ATARILYNX_DEF)
	$(CONFIGURE_LYNX_LIBRETRO_END)
	$(CONFIGURE_MAIN_LYNX_END)
endef

$(eval $(generic-package))
