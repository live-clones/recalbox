################################################################################
#
# recalbox-romfs-mastersystem
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system mastersystem --extension '.sms .SMS .zip .ZIP' --fullname 'Sega Master System / Mark III' --platform mastersystem --theme mastersystem libretro:genesisplusgx:BR2_PACKAGE_LIBRETRO_GENESISPLUSGX libretro:picodrive:BR2_PACKAGE_LIBRETRO_PICODRIVE

# Name the 3 vars as the package requires
RECALBOX_ROMFS_MASTERSYSTEM_SOURCE = 
RECALBOX_ROMFS_MASTERSYSTEM_SITE = 
RECALBOX_ROMFS_MASTERSYSTEM_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_MASTERSYSTEM = mastersystem
SYSTEM_XML_MASTERSYSTEM = $(@D)/$(SYSTEM_NAME_MASTERSYSTEM).xml
# System rom path
SOURCE_ROMDIR_MASTERSYSTEM = $(RECALBOX_ROMFS_MASTERSYSTEM_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GENESISPLUSGX)$(BR2_PACKAGE_LIBRETRO_PICODRIVE),)
define CONFIGURE_MAIN_MASTERSYSTEM_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_MASTERSYSTEM),Sega Master System / Mark III,$(SYSTEM_NAME_MASTERSYSTEM),.sms .SMS .zip .ZIP,mastersystem,mastersystem)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GENESISPLUSGX)$(BR2_PACKAGE_LIBRETRO_PICODRIVE),)
define CONFIGURE_MASTERSYSTEM_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_MASTERSYSTEM),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_GENESISPLUSGX),y)
define CONFIGURE_MASTERSYSTEM_LIBRETRO_GENESISPLUSGX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MASTERSYSTEM),genesisplusgx)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_PICODRIVE),y)
define CONFIGURE_MASTERSYSTEM_LIBRETRO_PICODRIVE_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_MASTERSYSTEM),picodrive)
endef
endif

define CONFIGURE_MASTERSYSTEM_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_MASTERSYSTEM))
endef
endif



define CONFIGURE_MAIN_MASTERSYSTEM_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_MASTERSYSTEM),$(SOURCE_ROMDIR_MASTERSYSTEM),$(@D))
endef
endif

define RECALBOX_ROMFS_MASTERSYSTEM_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_MASTERSYSTEM_START)
	$(CONFIGURE_MASTERSYSTEM_LIBRETRO_START)
	$(CONFIGURE_MASTERSYSTEM_LIBRETRO_GENESISPLUSGX_DEF)
	$(CONFIGURE_MASTERSYSTEM_LIBRETRO_PICODRIVE_DEF)
	$(CONFIGURE_MASTERSYSTEM_LIBRETRO_END)
	$(CONFIGURE_MAIN_MASTERSYSTEM_END)
endef

$(eval $(generic-package))
