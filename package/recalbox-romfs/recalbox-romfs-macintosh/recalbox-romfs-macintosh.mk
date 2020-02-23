################################################################################
#
# recalbox-romfs-macintosh
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system macintosh --extension '.dsk .DSK .zip .ZIP .7z .7Z' --fullname 'Apple Macintosh' --platform macintosh --theme macintosh 1:libretro:minivmac:BR2_PACKAGE_LIBRETRO_MINIVMAC

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MACINTOSH_SOURCE = 
RECALBOX_ROMFS_MACINTOSH_SITE = 
RECALBOX_ROMFS_MACINTOSH_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MACINTOSH = macintosh
SYSTEM_XML_MACINTOSH = $(@D)/$(SYSTEM_NAME_MACINTOSH).xml
# System rom path
SOURCE_ROMDIR_MACINTOSH = $(RECALBOX_ROMFS_MACINTOSH_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_MINIVMAC),)
define CONFIGURE_MAIN_MACINTOSH_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MACINTOSH),Apple Macintosh,$(SYSTEM_NAME_MACINTOSH),.dsk .DSK .zip .ZIP .7z .7Z,macintosh,macintosh)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_MINIVMAC),)
define CONFIGURE_MACINTOSH_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MACINTOSH),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MINIVMAC),y)
define CONFIGURE_MACINTOSH_LIBRETRO_MINIVMAC_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MACINTOSH),minivmac,1)
endef
endif

define CONFIGURE_MACINTOSH_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MACINTOSH))
endef
endif



define CONFIGURE_MAIN_MACINTOSH_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MACINTOSH),$(SOURCE_ROMDIR_MACINTOSH),$(@D))
endef
endif

define RECALBOX_ROMFS_MACINTOSH_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MACINTOSH_START)
	$(CONFIGURE_MACINTOSH_LIBRETRO_START)
	$(CONFIGURE_MACINTOSH_LIBRETRO_MINIVMAC_DEF)
	$(CONFIGURE_MACINTOSH_LIBRETRO_END)
	$(CONFIGURE_MAIN_MACINTOSH_END)
endef

$(eval $(generic-package))
