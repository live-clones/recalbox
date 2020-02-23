################################################################################
#
# recalbox-romfs-gx4000
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system gx4000 --extension '.dsk .DSK .m3u .M3U .zip .ZIP .7z .7Z .cpr .CPR .bin .BIN' --fullname 'Amstrad GX4000' --platform gx4000 --theme gx4000 1:libretro:cap32:BR2_PACKAGE_LIBRETRO_CAP32

# Name the 3 vars as the package requires
RECALBOX_ROMFS_GX4000_SOURCE = 
RECALBOX_ROMFS_GX4000_SITE = 
RECALBOX_ROMFS_GX4000_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_GX4000 = gx4000
SYSTEM_XML_GX4000 = $(@D)/$(SYSTEM_NAME_GX4000).xml
# System rom path
SOURCE_ROMDIR_GX4000 = $(RECALBOX_ROMFS_GX4000_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_CAP32),)
define CONFIGURE_MAIN_GX4000_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GX4000),Amstrad GX4000,$(SYSTEM_NAME_GX4000),.dsk .DSK .m3u .M3U .zip .ZIP .7z .7Z .cpr .CPR .bin .BIN,gx4000,gx4000)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_CAP32),)
define CONFIGURE_GX4000_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GX4000),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_CAP32),y)
define CONFIGURE_GX4000_LIBRETRO_CAP32_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GX4000),cap32,1)
endef
endif

define CONFIGURE_GX4000_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_GX4000))
endef
endif



define CONFIGURE_MAIN_GX4000_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_GX4000),$(SOURCE_ROMDIR_GX4000),$(@D))
endef
endif

define RECALBOX_ROMFS_GX4000_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_GX4000_START)
	$(CONFIGURE_GX4000_LIBRETRO_START)
	$(CONFIGURE_GX4000_LIBRETRO_CAP32_DEF)
	$(CONFIGURE_GX4000_LIBRETRO_END)
	$(CONFIGURE_MAIN_GX4000_END)
endef

$(eval $(generic-package))
