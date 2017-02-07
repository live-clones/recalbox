################################################################################
#
# recalbox-romfs-gb
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system gb --extension '.gb .GB .zip .ZIP' --fullname 'Game Boy' --platform gb --theme gb libretro:gambatte:BR2_PACKAGE_LIBRETRO_GAMBATTE libretro:tgbdual:BR2_PACKAGE_LIBRETRO_TGBDUAL

# Name the 3 vars as the package requires
RECALBOX_ROMFS_GB_SOURCE = 
RECALBOX_ROMFS_GB_SITE = 
RECALBOX_ROMFS_GB_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_GB = gb
SYSTEM_XML_GB = $(@D)/$(SYSTEM_NAME_GB).xml
# System rom path
SOURCE_ROMDIR_GB = $(RECALBOX_ROMFS_GB_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE)$(BR2_PACKAGE_LIBRETRO_TGBDUAL),)
define CONFIGURE_MAIN_GB_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GB),Game Boy,$(SYSTEM_NAME_GB),.gb .GB .zip .ZIP,gb,gb)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE)$(BR2_PACKAGE_LIBRETRO_TGBDUAL),)
define CONFIGURE_GB_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GB),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE),y)
define CONFIGURE_GB_LIBRETRO_GAMBATTE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GB),gambatte)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_TGBDUAL),y)
define CONFIGURE_GB_LIBRETRO_TGBDUAL_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GB),tgbdual)
endef
endif

define CONFIGURE_GB_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_GB))
endef
endif



define CONFIGURE_MAIN_GB_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_GB),$(SOURCE_ROMDIR_GB),$(@D))
endef
endif

define RECALBOX_ROMFS_GB_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_GB_START)
	$(CONFIGURE_GB_LIBRETRO_START)
	$(CONFIGURE_GB_LIBRETRO_GAMBATTE_DEF)
	$(CONFIGURE_GB_LIBRETRO_TGBDUAL_DEF)
	$(CONFIGURE_GB_LIBRETRO_END)
	$(CONFIGURE_MAIN_GB_END)
endef

$(eval $(generic-package))
