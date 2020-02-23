################################################################################
#
# recalbox-romfs-gb
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system gb --extension '.gb .GB .zip .ZIP .7z .7Z' --fullname 'Game Boy' --platform gb --theme gb 1:libretro:gambatte:BR2_PACKAGE_LIBRETRO_GAMBATTE 2:libretro:tgbdual:BR2_PACKAGE_LIBRETRO_TGBDUAL 3:libretro:mgba:BR2_PACKAGE_LIBRETRO_MGBA 4:libretro:sameboy:BR2_PACKAGE_LIBRETRO_SAMEBOY

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


ifneq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE)$(BR2_PACKAGE_LIBRETRO_TGBDUAL)$(BR2_PACKAGE_LIBRETRO_MGBA)$(BR2_PACKAGE_LIBRETRO_SAMEBOY),)
define CONFIGURE_MAIN_GB_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GB),Game Boy,$(SYSTEM_NAME_GB),.gb .GB .zip .ZIP .7z .7Z,gb,gb)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE)$(BR2_PACKAGE_LIBRETRO_TGBDUAL)$(BR2_PACKAGE_LIBRETRO_MGBA)$(BR2_PACKAGE_LIBRETRO_SAMEBOY),)
define CONFIGURE_GB_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GB),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_MGBA),y)
define CONFIGURE_GB_LIBRETRO_MGBA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GB),mgba,3)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_GAMBATTE),y)
define CONFIGURE_GB_LIBRETRO_GAMBATTE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GB),gambatte,1)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_TGBDUAL),y)
define CONFIGURE_GB_LIBRETRO_TGBDUAL_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GB),tgbdual,2)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_SAMEBOY),y)
define CONFIGURE_GB_LIBRETRO_SAMEBOY_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GB),sameboy,4)
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
	$(CONFIGURE_GB_LIBRETRO_MGBA_DEF)
	$(CONFIGURE_GB_LIBRETRO_GAMBATTE_DEF)
	$(CONFIGURE_GB_LIBRETRO_TGBDUAL_DEF)
	$(CONFIGURE_GB_LIBRETRO_SAMEBOY_DEF)
	$(CONFIGURE_GB_LIBRETRO_END)
	$(CONFIGURE_MAIN_GB_END)
endef

$(eval $(generic-package))
