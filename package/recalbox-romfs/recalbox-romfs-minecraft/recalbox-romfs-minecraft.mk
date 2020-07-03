################################################################################
#
# recalbox-romfs-minecraft
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --system minecraft --extension '.game' --fullname 'Minecraft' --platform minecraft --theme minecraft 1:libretro:craft:BR2_PACKAGE_LIBRETRO_CRAFT

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MINECRAFT_SOURCE = 
RECALBOX_ROMFS_MINECRAFT_SITE = 
RECALBOX_ROMFS_MINECRAFT_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MINECRAFT = minecraft
SYSTEM_XML_MINECRAFT = $(@D)/$(SYSTEM_NAME_MINECRAFT).xml
# System rom path
SOURCE_ROMDIR_MINECRAFT = $(RECALBOX_ROMFS_MINECRAFT_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_CRAFT),)
define CONFIGURE_MAIN_MINECRAFT_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_MINECRAFT),Minecraft,$(SYSTEM_NAME_MINECRAFT),.game,minecraft,minecraft)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_CRAFT),)
define CONFIGURE_MINECRAFT_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MINECRAFT),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_CRAFT),y)
define CONFIGURE_MINECRAFT_LIBRETRO_CRAFT_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MINECRAFT),craft,1)
endef
endif

define CONFIGURE_MINECRAFT_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MINECRAFT))
endef
endif



define CONFIGURE_MAIN_MINECRAFT_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_MINECRAFT),$(SOURCE_ROMDIR_MINECRAFT),$(@D),minecraft)
endef
endif

define RECALBOX_ROMFS_MINECRAFT_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MINECRAFT_START)
	$(CONFIGURE_MINECRAFT_LIBRETRO_START)
	$(CONFIGURE_MINECRAFT_LIBRETRO_CRAFT_DEF)
	$(CONFIGURE_MINECRAFT_LIBRETRO_END)
	$(CONFIGURE_MAIN_MINECRAFT_END)
endef

$(eval $(generic-package))
