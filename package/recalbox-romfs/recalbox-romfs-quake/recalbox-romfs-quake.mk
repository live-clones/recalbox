################################################################################
#
# recalbox-romfs-quake
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system quake --extension '.pak .zip .7z' --fullname 'Quake' --platform quake --theme quake 1:libretro:tyrquake:BR2_PACKAGE_LIBRETRO_TYRQUAKE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_QUAKE_SOURCE = 
RECALBOX_ROMFS_QUAKE_SITE = 
RECALBOX_ROMFS_QUAKE_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_QUAKE = quake
SYSTEM_XML_QUAKE = $(@D)/$(SYSTEM_NAME_QUAKE).xml
# System rom path
SOURCE_ROMDIR_QUAKE = $(RECALBOX_ROMFS_QUAKE_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_TYRQUAKE),)
define CONFIGURE_MAIN_QUAKE_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_QUAKE),Quake,$(SYSTEM_NAME_QUAKE),.pak .zip .7z,quake,quake)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_TYRQUAKE),)
define CONFIGURE_QUAKE_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_QUAKE),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_TYRQUAKE),y)
define CONFIGURE_QUAKE_LIBRETRO_TYRQUAKE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_QUAKE),tyrquake,1)
endef
endif

define CONFIGURE_QUAKE_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_QUAKE))
endef
endif



define CONFIGURE_MAIN_QUAKE_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_QUAKE),$(SOURCE_ROMDIR_QUAKE),$(@D),quake)
endef
endif

define RECALBOX_ROMFS_QUAKE_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_QUAKE_START)
	$(CONFIGURE_QUAKE_LIBRETRO_START)
	$(CONFIGURE_QUAKE_LIBRETRO_TYRQUAKE_DEF)
	$(CONFIGURE_QUAKE_LIBRETRO_END)
	$(CONFIGURE_MAIN_QUAKE_END)
endef

$(eval $(generic-package))
