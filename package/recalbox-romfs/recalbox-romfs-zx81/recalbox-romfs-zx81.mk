################################################################################
#
# recalbox-romfs-zx81
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system zx81 --extension '.tzx .TZX .p .P .zip .ZIP' --fullname 'ZX81' --platform zx81 --theme zx81 libretro:81:BR2_PACKAGE_LIBRETRO_81

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ZX81_SOURCE = 
RECALBOX_ROMFS_ZX81_SITE = 
RECALBOX_ROMFS_ZX81_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ZX81 = zx81
SYSTEM_XML_ZX81 = $(@D)/$(SYSTEM_NAME_ZX81).xml
# System rom path
SOURCE_ROMDIR_ZX81 = $(RECALBOX_ROMFS_ZX81_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_81),)
define CONFIGURE_MAIN_ZX81_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ZX81),ZX81,$(SYSTEM_NAME_ZX81),.tzx .TZX .p .P .zip .ZIP,zx81,zx81)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_81),)
define CONFIGURE_ZX81_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ZX81),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_81),y)
define CONFIGURE_ZX81_LIBRETRO_81_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ZX81),81)
endef
endif

define CONFIGURE_ZX81_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ZX81))
endef
endif



define CONFIGURE_MAIN_ZX81_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ZX81),$(SOURCE_ROMDIR_ZX81),$(@D))
endef
endif

define RECALBOX_ROMFS_ZX81_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ZX81_START)
	$(CONFIGURE_ZX81_LIBRETRO_START)
	$(CONFIGURE_ZX81_LIBRETRO_81_DEF)
	$(CONFIGURE_ZX81_LIBRETRO_END)
	$(CONFIGURE_MAIN_ZX81_END)
endef

$(eval $(generic-package))
