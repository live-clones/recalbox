################################################################################
#
# recalbox-romfs-prboom
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system prboom --extension '.wad .WAD' --fullname 'PrBoom' --platform prboom --theme prboom libretro:prboom:BR2_PACKAGE_LIBRETRO_PRBOOM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PRBOOM_SOURCE = 
RECALBOX_ROMFS_PRBOOM_SITE = 
RECALBOX_ROMFS_PRBOOM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PRBOOM = prboom
SYSTEM_XML_PRBOOM = $(@D)/$(SYSTEM_NAME_PRBOOM).xml
# System rom path
SOURCE_ROMDIR_PRBOOM = $(RECALBOX_ROMFS_PRBOOM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),)
define CONFIGURE_MAIN_PRBOOM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PRBOOM),PrBoom,$(SYSTEM_NAME_PRBOOM),.wad .WAD,prboom,prboom)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),)
define CONFIGURE_PRBOOM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PRBOOM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PRBOOM),y)
define CONFIGURE_PRBOOM_LIBRETRO_PRBOOM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PRBOOM),prboom)
endef
endif

define CONFIGURE_PRBOOM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PRBOOM))
endef
endif



define CONFIGURE_MAIN_PRBOOM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PRBOOM),$(SOURCE_ROMDIR_PRBOOM),$(@D))
endef
endif

define RECALBOX_ROMFS_PRBOOM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PRBOOM_START)
	$(CONFIGURE_PRBOOM_LIBRETRO_START)
	$(CONFIGURE_PRBOOM_LIBRETRO_PRBOOM_DEF)
	$(CONFIGURE_PRBOOM_LIBRETRO_END)
	$(CONFIGURE_MAIN_PRBOOM_END)
endef

$(eval $(generic-package))
