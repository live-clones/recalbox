################################################################################
#
# recalbox-romfs-naomi
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system naomi --extension '.lst .LST .dat .DAT .chd .CHD .zip .ZIP .7z .7Z' --fullname 'Sega NAOMI' --platform naomi --theme naomi 1:libretro:flycast:BR2_PACKAGE_LIBRETRO_FLYCAST

# Name the 3 vars as the package requires
RECALBOX_ROMFS_NAOMI_SOURCE = 
RECALBOX_ROMFS_NAOMI_SITE = 
RECALBOX_ROMFS_NAOMI_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_NAOMI = naomi
SYSTEM_XML_NAOMI = $(@D)/$(SYSTEM_NAME_NAOMI).xml
# System rom path
SOURCE_ROMDIR_NAOMI = $(RECALBOX_ROMFS_NAOMI_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),)
define CONFIGURE_MAIN_NAOMI_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_NAOMI),Sega NAOMI,$(SYSTEM_NAME_NAOMI),.lst .LST .dat .DAT .chd .CHD .zip .ZIP .7z .7Z,naomi,naomi)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),)
define CONFIGURE_NAOMI_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_NAOMI),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),y)
define CONFIGURE_NAOMI_LIBRETRO_FLYCAST_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_NAOMI),flycast,1)
endef
endif

define CONFIGURE_NAOMI_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_NAOMI))
endef
endif



define CONFIGURE_MAIN_NAOMI_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_NAOMI),$(SOURCE_ROMDIR_NAOMI),$(@D))
endef
endif

define RECALBOX_ROMFS_NAOMI_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_NAOMI_START)
	$(CONFIGURE_NAOMI_LIBRETRO_START)
	$(CONFIGURE_NAOMI_LIBRETRO_FLYCAST_DEF)
	$(CONFIGURE_NAOMI_LIBRETRO_END)
	$(CONFIGURE_MAIN_NAOMI_END)
endef

$(eval $(generic-package))
