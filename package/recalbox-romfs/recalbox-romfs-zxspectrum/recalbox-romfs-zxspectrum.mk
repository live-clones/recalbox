################################################################################
#
# recalbox-romfs-zxspectrum
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system zxspectrum --extension '.tzx .TZX .tap .TAP .z80 .Z80 .rzx .RZX .scl .SCL .trd .TRD .zip .ZIP .7z .7Z' --fullname 'ZXSpectrum' --platform zxspectrum --theme zxspectrum 1:libretro:fuse:BR2_PACKAGE_LIBRETRO_FUSE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ZXSPECTRUM_SOURCE = 
RECALBOX_ROMFS_ZXSPECTRUM_SITE = 
RECALBOX_ROMFS_ZXSPECTRUM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ZXSPECTRUM = zxspectrum
SYSTEM_XML_ZXSPECTRUM = $(@D)/$(SYSTEM_NAME_ZXSPECTRUM).xml
# System rom path
SOURCE_ROMDIR_ZXSPECTRUM = $(RECALBOX_ROMFS_ZXSPECTRUM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FUSE),)
define CONFIGURE_MAIN_ZXSPECTRUM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ZXSPECTRUM),ZXSpectrum,$(SYSTEM_NAME_ZXSPECTRUM),.tzx .TZX .tap .TAP .z80 .Z80 .rzx .RZX .scl .SCL .trd .TRD .zip .ZIP .7z .7Z,zxspectrum,zxspectrum)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FUSE),)
define CONFIGURE_ZXSPECTRUM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ZXSPECTRUM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FUSE),y)
define CONFIGURE_ZXSPECTRUM_LIBRETRO_FUSE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ZXSPECTRUM),fuse,1)
endef
endif

define CONFIGURE_ZXSPECTRUM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ZXSPECTRUM))
endef
endif



define CONFIGURE_MAIN_ZXSPECTRUM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ZXSPECTRUM),$(SOURCE_ROMDIR_ZXSPECTRUM),$(@D))
endef
endif

define RECALBOX_ROMFS_ZXSPECTRUM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ZXSPECTRUM_START)
	$(CONFIGURE_ZXSPECTRUM_LIBRETRO_START)
	$(CONFIGURE_ZXSPECTRUM_LIBRETRO_FUSE_DEF)
	$(CONFIGURE_ZXSPECTRUM_LIBRETRO_END)
	$(CONFIGURE_MAIN_ZXSPECTRUM_END)
endef

$(eval $(generic-package))
