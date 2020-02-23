################################################################################
#
# recalbox-romfs-atomiswave
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system atomiswave --extension '.lst .LST .dat .DAT .zip .ZIP .7z .7Z' --fullname 'Sammy Atomiswave' --platform atomiswave --theme atomiswave 1:libretro:flycast:BR2_PACKAGE_LIBRETRO_FLYCAST

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ATOMISWAVE_SOURCE = 
RECALBOX_ROMFS_ATOMISWAVE_SITE = 
RECALBOX_ROMFS_ATOMISWAVE_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ATOMISWAVE = atomiswave
SYSTEM_XML_ATOMISWAVE = $(@D)/$(SYSTEM_NAME_ATOMISWAVE).xml
# System rom path
SOURCE_ROMDIR_ATOMISWAVE = $(RECALBOX_ROMFS_ATOMISWAVE_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),)
define CONFIGURE_MAIN_ATOMISWAVE_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ATOMISWAVE),Sammy Atomiswave,$(SYSTEM_NAME_ATOMISWAVE),.lst .LST .dat .DAT .zip .ZIP .7z .7Z,atomiswave,atomiswave)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),)
define CONFIGURE_ATOMISWAVE_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ATOMISWAVE),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_FLYCAST),y)
define CONFIGURE_ATOMISWAVE_LIBRETRO_FLYCAST_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ATOMISWAVE),flycast,1)
endef
endif

define CONFIGURE_ATOMISWAVE_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ATOMISWAVE))
endef
endif



define CONFIGURE_MAIN_ATOMISWAVE_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ATOMISWAVE),$(SOURCE_ROMDIR_ATOMISWAVE),$(@D))
endef
endif

define RECALBOX_ROMFS_ATOMISWAVE_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ATOMISWAVE_START)
	$(CONFIGURE_ATOMISWAVE_LIBRETRO_START)
	$(CONFIGURE_ATOMISWAVE_LIBRETRO_FLYCAST_DEF)
	$(CONFIGURE_ATOMISWAVE_LIBRETRO_END)
	$(CONFIGURE_MAIN_ATOMISWAVE_END)
endef

$(eval $(generic-package))
