################################################################################
#
# recalbox-romfs-msxturbor
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system msxturbor --extension '.rom .ROM .mx1 .MX1 .mx2 .MX2 .dsk .DSK .cas .CAS .m3u .M3U .zip .ZIP .7z .7Z' --fullname 'MSXturboR' --platform msxturbor --theme msxturbor 1:libretro:bluemsx:BR2_PACKAGE_LIBRETRO_BLUEMSX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MSXTURBOR_SOURCE = 
RECALBOX_ROMFS_MSXTURBOR_SITE = 
RECALBOX_ROMFS_MSXTURBOR_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MSXTURBOR = msxturbor
SYSTEM_XML_MSXTURBOR = $(@D)/$(SYSTEM_NAME_MSXTURBOR).xml
# System rom path
SOURCE_ROMDIR_MSXTURBOR = $(RECALBOX_ROMFS_MSXTURBOR_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),)
define CONFIGURE_MAIN_MSXTURBOR_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MSXTURBOR),MSXturboR,$(SYSTEM_NAME_MSXTURBOR),.rom .ROM .mx1 .MX1 .mx2 .MX2 .dsk .DSK .cas .CAS .m3u .M3U .zip .ZIP .7z .7Z,msxturbor,msxturbor)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),)
define CONFIGURE_MSXTURBOR_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MSXTURBOR),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),y)
define CONFIGURE_MSXTURBOR_LIBRETRO_BLUEMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MSXTURBOR),bluemsx,1)
endef
endif

define CONFIGURE_MSXTURBOR_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MSXTURBOR))
endef
endif



define CONFIGURE_MAIN_MSXTURBOR_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MSXTURBOR),$(SOURCE_ROMDIR_MSXTURBOR),$(@D))
endef
endif

define RECALBOX_ROMFS_MSXTURBOR_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MSXTURBOR_START)
	$(CONFIGURE_MSXTURBOR_LIBRETRO_START)
	$(CONFIGURE_MSXTURBOR_LIBRETRO_BLUEMSX_DEF)
	$(CONFIGURE_MSXTURBOR_LIBRETRO_END)
	$(CONFIGURE_MAIN_MSXTURBOR_END)
endef

$(eval $(generic-package))
