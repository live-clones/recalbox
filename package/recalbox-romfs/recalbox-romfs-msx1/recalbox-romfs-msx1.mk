################################################################################
#
# recalbox-romfs-msx1
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system msx1 --extension '.rom .ROM .mx1 .MX1 .mx2 .MX2 .dsk .DSK .cas .CAS .m3u .M3U .zip .ZIP .7z .7Z' --fullname 'MSX1' --platform msx1 --theme msx1 1:libretro:bluemsx:BR2_PACKAGE_LIBRETRO_BLUEMSX 2:libretro:fmsx:BR2_PACKAGE_LIBRETRO_FMSX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MSX1_SOURCE = 
RECALBOX_ROMFS_MSX1_SITE = 
RECALBOX_ROMFS_MSX1_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MSX1 = msx1
SYSTEM_XML_MSX1 = $(@D)/$(SYSTEM_NAME_MSX1).xml
# System rom path
SOURCE_ROMDIR_MSX1 = $(RECALBOX_ROMFS_MSX1_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX)$(BR2_PACKAGE_LIBRETRO_FMSX),)
define CONFIGURE_MAIN_MSX1_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MSX1),MSX1,$(SYSTEM_NAME_MSX1),.rom .ROM .mx1 .MX1 .mx2 .MX2 .dsk .DSK .cas .CAS .m3u .M3U .zip .ZIP .7z .7Z,msx1,msx1)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX)$(BR2_PACKAGE_LIBRETRO_FMSX),)
define CONFIGURE_MSX1_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MSX1),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FMSX),y)
define CONFIGURE_MSX1_LIBRETRO_FMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MSX1),fmsx,2)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),y)
define CONFIGURE_MSX1_LIBRETRO_BLUEMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MSX1),bluemsx,1)
endef
endif

define CONFIGURE_MSX1_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MSX1))
endef
endif



define CONFIGURE_MAIN_MSX1_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MSX1),$(SOURCE_ROMDIR_MSX1),$(@D))
endef
endif

define RECALBOX_ROMFS_MSX1_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MSX1_START)
	$(CONFIGURE_MSX1_LIBRETRO_START)
	$(CONFIGURE_MSX1_LIBRETRO_FMSX_DEF)
	$(CONFIGURE_MSX1_LIBRETRO_BLUEMSX_DEF)
	$(CONFIGURE_MSX1_LIBRETRO_END)
	$(CONFIGURE_MAIN_MSX1_END)
endef

$(eval $(generic-package))
