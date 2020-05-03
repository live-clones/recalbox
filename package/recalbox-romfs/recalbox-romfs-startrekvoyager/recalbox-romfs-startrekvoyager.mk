################################################################################
#
# recalbox-romfs-startrekvoyager
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system startrekvoyager --extension '.pak .zip .7z' --fullname 'Star Trek Voyager' --platform startrekvoyager --theme quake 1:libretro:vitavoyager:BR2_PACKAGE_LIBRETRO_VITAVOYAGER

# Name the 3 vars as the package requires
RECALBOX_ROMFS_STARTREKVOYAGER_SOURCE = 
RECALBOX_ROMFS_STARTREKVOYAGER_SITE = 
RECALBOX_ROMFS_STARTREKVOYAGER_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_STARTREKVOYAGER = startrekvoyager
SYSTEM_XML_STARTREKVOYAGER = $(@D)/$(SYSTEM_NAME_STARTREKVOYAGER).xml
# System rom path
SOURCE_ROMDIR_STARTREKVOYAGER = $(RECALBOX_ROMFS_STARTREKVOYAGER_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VITAVOYAGER),)
define CONFIGURE_MAIN_STARTREKVOYAGER_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_STARTREKVOYAGER),Star Trek Voyager,$(SYSTEM_NAME_STARTREKVOYAGER),.pak .zip .7z,startrekvoyager,quake)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VITAVOYAGER),)
define CONFIGURE_STARTREKVOYAGER_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_STARTREKVOYAGER),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VITAVOYAGER),y)
define CONFIGURE_STARTREKVOYAGER_LIBRETRO_VITAVOYAGER_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_STARTREKVOYAGER),vitavoyager,1)
endef
endif

define CONFIGURE_STARTREKVOYAGER_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_STARTREKVOYAGER))
endef
endif



define CONFIGURE_MAIN_STARTREKVOYAGER_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_STARTREKVOYAGER),$(SOURCE_ROMDIR_STARTREKVOYAGER),$(@D),startrekvoyager)
endef
endif

define RECALBOX_ROMFS_STARTREKVOYAGER_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_STARTREKVOYAGER_START)
	$(CONFIGURE_STARTREKVOYAGER_LIBRETRO_START)
	$(CONFIGURE_STARTREKVOYAGER_LIBRETRO_VITAVOYAGER_DEF)
	$(CONFIGURE_STARTREKVOYAGER_LIBRETRO_END)
	$(CONFIGURE_MAIN_STARTREKVOYAGER_END)
endef

$(eval $(generic-package))
