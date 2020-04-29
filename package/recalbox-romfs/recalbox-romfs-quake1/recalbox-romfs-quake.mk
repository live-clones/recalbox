################################################################################
#
# recalbox-romfs-quake1
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system quake1 --extension '.pak .zip .7z' --fullname 'Quake 1' --platform quake --theme quake 1:libretro:tyrquake:BR2_PACKAGE_LIBRETRO_TYRQUAKE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_QUAKE1_SOURCE = 
RECALBOX_ROMFS_QUAKE1_SITE = 
RECALBOX_ROMFS_QUAKE1_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_QUAKE1 = quake1
SYSTEM_XML_QUAKE1 = $(@D)/$(SYSTEM_NAME_QUAKE1).xml
# System rom path
SOURCE_ROMDIR_QUAKE1 = $(RECALBOX_ROMFS_QUAKE1_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_TYRQUAKE),)
define CONFIGURE_MAIN_QUAKE1_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_QUAKE1),Quake 1,$(SYSTEM_NAME_QUAKE1),.pak .zip .7z,quake,quake)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_TYRQUAKE),)
define CONFIGURE_QUAKE1_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_QUAKE1),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_TYRQUAKE),y)
define CONFIGURE_QUAKE1_LIBRETRO_TYRQUAKE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_QUAKE1),tyrquake,1)
endef
endif

define CONFIGURE_QUAKE1_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_QUAKE1))
endef
endif



define CONFIGURE_MAIN_QUAKE1_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_QUAKE1),$(SOURCE_ROMDIR_QUAKE1),$(@D))
endef
endif

define RECALBOX_ROMFS_QUAKE1_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_QUAKE1_START)
	$(CONFIGURE_QUAKE1_LIBRETRO_START)
	$(CONFIGURE_QUAKE1_LIBRETRO_TYRQUAKE_DEF)
	$(CONFIGURE_QUAKE1_LIBRETRO_END)
	$(CONFIGURE_MAIN_QUAKE1_END)
endef

$(eval $(generic-package))
