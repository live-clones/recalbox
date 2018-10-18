################################################################################
#
# recalbox-romfs-neogeocd
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system neogeocd --extension '.cue .CUE' --fullname 'Neo-Geo CD' --platform neogeocd --theme neogeocd libretro:fba:BR2_PACKAGE_LIBRETRO_FBA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_NEOGEOCD_SOURCE = 
RECALBOX_ROMFS_NEOGEOCD_SITE = 
RECALBOX_ROMFS_NEOGEOCD_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_NEOGEOCD = neogeocd
SYSTEM_XML_NEOGEOCD = $(@D)/$(SYSTEM_NAME_NEOGEOCD).xml
# System rom path
SOURCE_ROMDIR_NEOGEOCD = $(RECALBOX_ROMFS_NEOGEOCD_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FBA),)
    define CONFIGURE_MAIN_NEOGEOCD_START
        $(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_NEOGEOCD),Neo-Geo CD,$(SYSTEM_NAME_NEOGEOCD),.cue .CUE,neogeocd,neogeocd,--subsystem neogeocd)
    endef
    define CONFIGURE_NEOGEOCD_LIBRETRO_START
        $(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_NEOGEOCD),libretro)
    endef
    define CONFIGURE_NEOGEOCD_LIBRETRO_FBA_DEF
        $(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NEOGEOCD),fba)
    endef
    define CONFIGURE_NEOGEOCD_LIBRETRO_END
        $(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_NEOGEOCD))
    endef
    define CONFIGURE_MAIN_NEOGEOCD_END
        $(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_NEOGEOCD),$(SOURCE_ROMDIR_NEOGEOCD),$(@D))
    endef
endif

define RECALBOX_ROMFS_NEOGEOCD_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_NEOGEOCD_START)
	$(CONFIGURE_NEOGEOCD_LIBRETRO_START)
	$(CONFIGURE_NEOGEOCD_LIBRETRO_FBA_DEF)
	$(CONFIGURE_NEOGEOCD_LIBRETRO_END)
	$(CONFIGURE_MAIN_NEOGEOCD_END)
endef

$(eval $(generic-package))