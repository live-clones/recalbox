################################################################################
#
# recalbox-romfs-quake2
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system quake2 --extension '.pak .zip .7z' --fullname 'Quake 2' --platform quake2 --theme quake 1:libretro:vitaquake2:BR2_PACKAGE_LIBRETRO_VITAQUAKE2

# Name the 3 vars as the package requires
RECALBOX_ROMFS_QUAKE2_SOURCE = 
RECALBOX_ROMFS_QUAKE2_SITE = 
RECALBOX_ROMFS_QUAKE2_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_QUAKE2 = quake2
SYSTEM_XML_QUAKE2 = $(@D)/$(SYSTEM_NAME_QUAKE2).xml
# System rom path
SOURCE_ROMDIR_QUAKE2 = $(RECALBOX_ROMFS_QUAKE2_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VITAQUAKE2),)
define CONFIGURE_MAIN_QUAKE2_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_QUAKE2),Quake 2,$(SYSTEM_NAME_QUAKE2),.pak .zip .7z,quake2,quake)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VITAQUAKE2),)
define CONFIGURE_QUAKE2_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_QUAKE2),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VITAQUAKE2),y)
define CONFIGURE_QUAKE2_LIBRETRO_VITAQUAKE2_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_QUAKE2),vitaquake2,1)
endef
endif

define CONFIGURE_QUAKE2_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_QUAKE2))
endef
endif



define CONFIGURE_MAIN_QUAKE2_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_QUAKE2),$(SOURCE_ROMDIR_QUAKE2),$(@D),quake2)
endef
endif

define RECALBOX_ROMFS_QUAKE2_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_QUAKE2_START)
	$(CONFIGURE_QUAKE2_LIBRETRO_START)
	$(CONFIGURE_QUAKE2_LIBRETRO_VITAQUAKE2_DEF)
	$(CONFIGURE_QUAKE2_LIBRETRO_END)
	$(CONFIGURE_MAIN_QUAKE2_END)
endef

$(eval $(generic-package))
