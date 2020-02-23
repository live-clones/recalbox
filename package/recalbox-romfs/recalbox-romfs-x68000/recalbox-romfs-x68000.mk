################################################################################
#
# recalbox-romfs-x68000
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system x68000 --extension '.dim .DIM .img .IMG .d88 .D88 .88d .88D .hdm .HDM .dup .DUP .2hd .2HD .xdf .XDF .hdf .HDF .cmd .CMD .m3u .M3U .zip .ZIP .7z .7Z' --fullname 'Sharp X68000' --platform x68000 --theme x68000 1:libretro:px68k:BR2_PACKAGE_LIBRETRO_PX68K

# Name the 3 vars as the package requires
RECALBOX_ROMFS_X68000_SOURCE = 
RECALBOX_ROMFS_X68000_SITE = 
RECALBOX_ROMFS_X68000_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_X68000 = x68000
SYSTEM_XML_X68000 = $(@D)/$(SYSTEM_NAME_X68000).xml
# System rom path
SOURCE_ROMDIR_X68000 = $(RECALBOX_ROMFS_X68000_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PX68K),)
define CONFIGURE_MAIN_X68000_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_X68000),Sharp X68000,$(SYSTEM_NAME_X68000),.dim .DIM .img .IMG .d88 .D88 .88d .88D .hdm .HDM .dup .DUP .2hd .2HD .xdf .XDF .hdf .HDF .cmd .CMD .m3u .M3U .zip .ZIP .7z .7Z,x68000,x68000)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PX68K),)
define CONFIGURE_X68000_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_X68000),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PX68K),y)
define CONFIGURE_X68000_LIBRETRO_PX68K_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_X68000),px68k,1)
endef
endif

define CONFIGURE_X68000_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_X68000))
endef
endif



define CONFIGURE_MAIN_X68000_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_X68000),$(SOURCE_ROMDIR_X68000),$(@D))
endef
endif

define RECALBOX_ROMFS_X68000_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_X68000_START)
	$(CONFIGURE_X68000_LIBRETRO_START)
	$(CONFIGURE_X68000_LIBRETRO_PX68K_DEF)
	$(CONFIGURE_X68000_LIBRETRO_END)
	$(CONFIGURE_MAIN_X68000_END)
endef

$(eval $(generic-package))
