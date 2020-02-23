################################################################################
#
# recalbox-romfs-msx2
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system msx2 --extension '.rom .ROM .mx1 .MX1 .mx2 .MX2 .dsk .DSK .cas .CAS .m3u .M3U .zip .ZIP .7z .7Z' --fullname 'MSX2' --platform msx2 --theme msx2 1:libretro:bluemsx:BR2_PACKAGE_LIBRETRO_BLUEMSX 2:libretro:fmsx:BR2_PACKAGE_LIBRETRO_FMSX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MSX2_SOURCE = 
RECALBOX_ROMFS_MSX2_SITE = 
RECALBOX_ROMFS_MSX2_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MSX2 = msx2
SYSTEM_XML_MSX2 = $(@D)/$(SYSTEM_NAME_MSX2).xml
# System rom path
SOURCE_ROMDIR_MSX2 = $(RECALBOX_ROMFS_MSX2_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX)$(BR2_PACKAGE_LIBRETRO_FMSX),)
define CONFIGURE_MAIN_MSX2_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MSX2),MSX2,$(SYSTEM_NAME_MSX2),.rom .ROM .mx1 .MX1 .mx2 .MX2 .dsk .DSK .cas .CAS .m3u .M3U .zip .ZIP .7z .7Z,msx2,msx2)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX)$(BR2_PACKAGE_LIBRETRO_FMSX),)
define CONFIGURE_MSX2_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MSX2),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FMSX),y)
define CONFIGURE_MSX2_LIBRETRO_FMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MSX2),fmsx,2)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),y)
define CONFIGURE_MSX2_LIBRETRO_BLUEMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MSX2),bluemsx,1)
endef
endif

define CONFIGURE_MSX2_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MSX2))
endef
endif



define CONFIGURE_MAIN_MSX2_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MSX2),$(SOURCE_ROMDIR_MSX2),$(@D))
endef
endif

define RECALBOX_ROMFS_MSX2_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MSX2_START)
	$(CONFIGURE_MSX2_LIBRETRO_START)
	$(CONFIGURE_MSX2_LIBRETRO_FMSX_DEF)
	$(CONFIGURE_MSX2_LIBRETRO_BLUEMSX_DEF)
	$(CONFIGURE_MSX2_LIBRETRO_END)
	$(CONFIGURE_MAIN_MSX2_END)
endef

$(eval $(generic-package))
