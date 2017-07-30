################################################################################
#
# recalbox-romfs-atari2600
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system atari2600 --extension '.a26 .A26 .bin .BIN .zip .ZIP' --fullname 'Atari 2600' --platform atari2600 --theme atari2600 libretro:stella:BR2_PACKAGE_LIBRETRO_STELLA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ATARI2600_SOURCE = 
RECALBOX_ROMFS_ATARI2600_SITE = 
RECALBOX_ROMFS_ATARI2600_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ATARI2600 = atari2600
SYSTEM_XML_ATARI2600 = $(@D)/$(SYSTEM_NAME_ATARI2600).xml
# System rom path
SOURCE_ROMDIR_ATARI2600 = $(RECALBOX_ROMFS_ATARI2600_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_STELLA),)
define CONFIGURE_MAIN_ATARI2600_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ATARI2600),Atari 2600,$(SYSTEM_NAME_ATARI2600),.a26 .A26 .bin .BIN .zip .ZIP,atari2600,atari2600)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_STELLA),)
define CONFIGURE_ATARI2600_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ATARI2600),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_STELLA),y)
define CONFIGURE_ATARI2600_LIBRETRO_STELLA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ATARI2600),stella)
endef
endif

define CONFIGURE_ATARI2600_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ATARI2600))
endef
endif



define CONFIGURE_MAIN_ATARI2600_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ATARI2600),$(SOURCE_ROMDIR_ATARI2600),$(@D))
endef
endif

define RECALBOX_ROMFS_ATARI2600_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ATARI2600_START)
	$(CONFIGURE_ATARI2600_LIBRETRO_START)
	$(CONFIGURE_ATARI2600_LIBRETRO_STELLA_DEF)
	$(CONFIGURE_ATARI2600_LIBRETRO_END)
	$(CONFIGURE_MAIN_ATARI2600_END)
endef

$(eval $(generic-package))
