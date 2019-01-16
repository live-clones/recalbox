################################################################################
#
# recalbox-romfs-3ds
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system 3ds --extension '.3ds .3DS .3dsx .3DSX .elf .ELF .axf .AXF .cci .CCI .cxi .CXI .app .APP .zip .ZIP .7z .7Z' --fullname 'Nintendo 3DS' --platform 3ds --theme odyssey2 libretro:citra:BR2_PACKAGE_LIBRETRO_CITRA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_3DS_SOURCE = 
RECALBOX_ROMFS_3DS_SITE = 
RECALBOX_ROMFS_3DS_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_3DS = 3ds
SYSTEM_XML_3DS = $(@D)/$(SYSTEM_NAME_3DS).xml
# System rom path
SOURCE_ROMDIR_3DS = $(RECALBOX_ROMFS_3DS_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_CITRA),)
define CONFIGURE_MAIN_3DS_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_3DS),Nintendo 3DS,$(SYSTEM_NAME_3DS),.3ds .3DS .3dsx .3DSX .elf .ELF .axf .AXF .cci .CCI .cxi .CXI .app .APP .zip .ZIP .7z .7Z,3ds,odyssey2)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_CITRA),)
define CONFIGURE_3DS_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_3DS),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_CITRA),y)
define CONFIGURE_3DS_LIBRETRO_CITRA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_3DS),citra)
endef
endif

define CONFIGURE_3DS_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_3DS))
endef
endif



define CONFIGURE_MAIN_3DS_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_3DS),$(SOURCE_ROMDIR_3DS),$(@D))
endef
endif

define RECALBOX_ROMFS_3DS_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_3DS_START)
	$(CONFIGURE_3DS_LIBRETRO_START)
	$(CONFIGURE_3DS_LIBRETRO_CITRA_DEF)
	$(CONFIGURE_3DS_LIBRETRO_END)
	$(CONFIGURE_MAIN_3DS_END)
endef

$(eval $(generic-package))