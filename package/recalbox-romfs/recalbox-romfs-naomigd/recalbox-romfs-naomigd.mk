################################################################################
#
# recalbox-romfs-naomigd
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system naomigd --extension '.lst .LST .dat .DAT .zip .ZIP .7z .7Z' --fullname 'Sega NAOMI GD-ROM System' --platform naomigd --theme naomigd 1:libretro:flycast:BR2_PACKAGE_LIBRETRO_FLYCAST

# Name the 3 vars as the package requires
RECALBOX_ROMFS_NAOMIGD_SOURCE = 
RECALBOX_ROMFS_NAOMIGD_SITE = 
RECALBOX_ROMFS_NAOMIGD_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_NAOMIGD = naomigd
SYSTEM_XML_NAOMIGD = $(@D)/$(SYSTEM_NAME_NAOMIGD).xml
# System rom path
SOURCE_ROMDIR_NAOMIGD = $(RECALBOX_ROMFS_NAOMIGD_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),)
define CONFIGURE_MAIN_NAOMIGD_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_NAOMIGD),Sega NAOMI GD-ROM System,$(SYSTEM_NAME_NAOMIGD),.lst .LST .dat .DAT .zip .ZIP .7z .7Z,naomigd,naomigd)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),)
define CONFIGURE_NAOMIGD_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_NAOMIGD),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),y)
define CONFIGURE_NAOMIGD_LIBRETRO_FLYCAST_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NAOMIGD),flycast,1)
endef
endif

define CONFIGURE_NAOMIGD_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_NAOMIGD))
endef
endif



define CONFIGURE_MAIN_NAOMIGD_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_NAOMIGD),$(SOURCE_ROMDIR_NAOMIGD),$(@D))
endef
endif

define RECALBOX_ROMFS_NAOMIGD_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_NAOMIGD_START)
	$(CONFIGURE_NAOMIGD_LIBRETRO_START)
	$(CONFIGURE_NAOMIGD_LIBRETRO_FLYCAST_DEF)
	$(CONFIGURE_NAOMIGD_LIBRETRO_END)
	$(CONFIGURE_MAIN_NAOMIGD_END)
endef

$(eval $(generic-package))
