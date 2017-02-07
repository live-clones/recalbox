################################################################################
#
# recalbox-romfs-segacd
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system segacd --extension '.cue .CUE .iso .ISO' --fullname 'Sega CD' --platform segacd --theme segacd libretro:genesisplusgx:BR2_PACKAGE_LIBRETRO_GENESISPLUSGX libretro:picodrive:BR2_PACKAGE_LIBRETRO_PICODRIVE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SEGACD_SOURCE = 
RECALBOX_ROMFS_SEGACD_SITE = 
RECALBOX_ROMFS_SEGACD_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SEGACD = segacd
SYSTEM_XML_SEGACD = $(@D)/$(SYSTEM_NAME_SEGACD).xml
# System rom path
SOURCE_ROMDIR_SEGACD = $(RECALBOX_ROMFS_SEGACD_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GENESISPLUSGX)$(BR2_PACKAGE_LIBRETRO_PICODRIVE),)
define CONFIGURE_MAIN_SEGACD_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SEGACD),Sega CD,$(SYSTEM_NAME_SEGACD),.cue .CUE .iso .ISO,segacd,segacd)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GENESISPLUSGX)$(BR2_PACKAGE_LIBRETRO_PICODRIVE),)
define CONFIGURE_SEGACD_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SEGACD),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_GENESISPLUSGX),y)
define CONFIGURE_SEGACD_LIBRETRO_GENESISPLUSGX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SEGACD),genesisplusgx)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_PICODRIVE),y)
define CONFIGURE_SEGACD_LIBRETRO_PICODRIVE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SEGACD),picodrive)
endef
endif

define CONFIGURE_SEGACD_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SEGACD))
endef
endif



define CONFIGURE_MAIN_SEGACD_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SEGACD),$(SOURCE_ROMDIR_SEGACD),$(@D))
endef
endif

define RECALBOX_ROMFS_SEGACD_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SEGACD_START)
	$(CONFIGURE_SEGACD_LIBRETRO_START)
	$(CONFIGURE_SEGACD_LIBRETRO_GENESISPLUSGX_DEF)
	$(CONFIGURE_SEGACD_LIBRETRO_PICODRIVE_DEF)
	$(CONFIGURE_SEGACD_LIBRETRO_END)
	$(CONFIGURE_MAIN_SEGACD_END)
endef

$(eval $(generic-package))
