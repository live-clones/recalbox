################################################################################
#
# recalbox-romfs-doom3
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system doom3 --extension '.pk4 .zip .7z' --fullname 'Doom 3' --platform doom3 --theme doom 1:libretro:boom3:BR2_PACKAGE_LIBRETRO_BOOM3

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DOOM3_SOURCE = 
RECALBOX_ROMFS_DOOM3_SITE = 
RECALBOX_ROMFS_DOOM3_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DOOM3 = doom3
SYSTEM_XML_DOOM3 = $(@D)/$(SYSTEM_NAME_DOOM3).xml
# System rom path
SOURCE_ROMDIR_DOOM3 = $(RECALBOX_ROMFS_DOOM3_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BOOM3),)
define CONFIGURE_MAIN_DOOM3_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_DOOM3),Doom 3,$(SYSTEM_NAME_DOOM3),.pk4 .zip .7z,doom3,doom)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BOOM3),)
define CONFIGURE_DOOM3_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_DOOM3),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BOOM3),y)
define CONFIGURE_DOOM3_LIBRETRO_BOOM3_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_DOOM3),boom3,1)
endef
endif

define CONFIGURE_DOOM3_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_DOOM3))
endef
endif



define CONFIGURE_MAIN_DOOM3_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_DOOM3),$(SOURCE_ROMDIR_DOOM3),$(@D),doom3)
endef
endif

define RECALBOX_ROMFS_DOOM3_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_DOOM3_START)
	$(CONFIGURE_DOOM3_LIBRETRO_START)
	$(CONFIGURE_DOOM3_LIBRETRO_BOOM3_DEF)
	$(CONFIGURE_DOOM3_LIBRETRO_END)
	$(CONFIGURE_MAIN_DOOM3_END)
endef

$(eval $(generic-package))
