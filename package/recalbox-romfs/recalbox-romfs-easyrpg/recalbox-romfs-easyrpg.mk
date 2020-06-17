################################################################################
#
# recalbox-romfs-easyrpg
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system easyrpg --extension '.ini' --fullname 'EasyRPG' --platform easyrpg --theme easyrpg 1:libretro:easyrpg:BR2_PACKAGE_LIBRETRO_EASYRPG

# Name the 3 vars as the package requires
RECALBOX_ROMFS_EASYRPG_SOURCE = 
RECALBOX_ROMFS_EASYRPG_SITE = 
RECALBOX_ROMFS_EASYRPG_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_EASYRPG = easyrpg
SYSTEM_XML_EASYRPG = $(@D)/$(SYSTEM_NAME_EASYRPG).xml
# System rom path
SOURCE_ROMDIR_EASYRPG = $(RECALBOX_ROMFS_EASYRPG_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_EASYRPG),)
define CONFIGURE_MAIN_EASYRPG_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_EASYRPG),TIC-80,$(SYSTEM_NAME_EASYRPG),.ini,easyrpg,easyrpg)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_EASYRPG),)
define CONFIGURE_EASYRPG_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_EASYRPG),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_EASYRPG),y)
define CONFIGURE_EASYRPG_LIBRETRO_EASYRPG_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_EASYRPG),easyrpg,1)
endef
endif

define CONFIGURE_EASYRPG_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_EASYRPG))
endef
endif



define CONFIGURE_MAIN_EASYRPG_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_EASYRPG),$(SOURCE_ROMDIR_EASYRPG),$(@D))
endef
endif

define RECALBOX_ROMFS_EASYRPG_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_EASYRPG_START)
	$(CONFIGURE_EASYRPG_LIBRETRO_START)
	$(CONFIGURE_EASYRPG_LIBRETRO_EASYRPG_DEF)
	$(CONFIGURE_EASYRPG_LIBRETRO_END)
	$(CONFIGURE_MAIN_EASYRPG_END)
endef

$(eval $(generic-package))
