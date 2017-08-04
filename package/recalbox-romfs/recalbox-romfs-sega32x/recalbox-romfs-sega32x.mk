################################################################################
#
# recalbox-romfs-sega32x
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system sega32x --extension '.32x .32X .smd .SMD .bin .zip .ZIP' --fullname 'Sega 32X' --platform sega32x --theme sega32x libretro:picodrive:BR2_PACKAGE_LIBRETRO_PICODRIVE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SEGA32X_SOURCE = 
RECALBOX_ROMFS_SEGA32X_SITE = 
RECALBOX_ROMFS_SEGA32X_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SEGA32X = sega32x
SYSTEM_XML_SEGA32X = $(@D)/$(SYSTEM_NAME_SEGA32X).xml
# System rom path
SOURCE_ROMDIR_SEGA32X = $(RECALBOX_ROMFS_SEGA32X_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PICODRIVE),)
define CONFIGURE_MAIN_SEGA32X_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SEGA32X),Sega 32X,$(SYSTEM_NAME_SEGA32X),.32x .32X .smd .SMD .bin .zip .ZIP,sega32x,sega32x)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PICODRIVE),)
define CONFIGURE_SEGA32X_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SEGA32X),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PICODRIVE),y)
define CONFIGURE_SEGA32X_LIBRETRO_PICODRIVE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SEGA32X),picodrive)
endef
endif

define CONFIGURE_SEGA32X_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SEGA32X))
endef
endif



define CONFIGURE_MAIN_SEGA32X_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SEGA32X),$(SOURCE_ROMDIR_SEGA32X),$(@D))
endef
endif

define RECALBOX_ROMFS_SEGA32X_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SEGA32X_START)
	$(CONFIGURE_SEGA32X_LIBRETRO_START)
	$(CONFIGURE_SEGA32X_LIBRETRO_PICODRIVE_DEF)
	$(CONFIGURE_SEGA32X_LIBRETRO_END)
	$(CONFIGURE_MAIN_SEGA32X_END)
endef

$(eval $(generic-package))
