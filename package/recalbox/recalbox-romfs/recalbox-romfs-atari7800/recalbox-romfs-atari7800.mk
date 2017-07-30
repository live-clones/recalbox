################################################################################
#
# recalbox-romfs-atari7800
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system atari7800 --extension '.a78 .A78 .bin .BIN .zip .ZIP' --fullname 'Atari 7800' --platform atari7800 --theme atari7800 libretro:prosystem:BR2_PACKAGE_LIBRETRO_PROSYSTEM

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ATARI7800_SOURCE = 
RECALBOX_ROMFS_ATARI7800_SITE = 
RECALBOX_ROMFS_ATARI7800_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ATARI7800 = atari7800
SYSTEM_XML_ATARI7800 = $(@D)/$(SYSTEM_NAME_ATARI7800).xml
# System rom path
SOURCE_ROMDIR_ATARI7800 = $(RECALBOX_ROMFS_ATARI7800_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PROSYSTEM),)
define CONFIGURE_MAIN_ATARI7800_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ATARI7800),Atari 7800,$(SYSTEM_NAME_ATARI7800),.a78 .A78 .bin .BIN .zip .ZIP,atari7800,atari7800)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PROSYSTEM),)
define CONFIGURE_ATARI7800_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ATARI7800),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_PROSYSTEM),y)
define CONFIGURE_ATARI7800_LIBRETRO_PROSYSTEM_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ATARI7800),prosystem)
endef
endif

define CONFIGURE_ATARI7800_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ATARI7800))
endef
endif



define CONFIGURE_MAIN_ATARI7800_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ATARI7800),$(SOURCE_ROMDIR_ATARI7800),$(@D))
endef
endif

define RECALBOX_ROMFS_ATARI7800_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ATARI7800_START)
	$(CONFIGURE_ATARI7800_LIBRETRO_START)
	$(CONFIGURE_ATARI7800_LIBRETRO_PROSYSTEM_DEF)
	$(CONFIGURE_ATARI7800_LIBRETRO_END)
	$(CONFIGURE_MAIN_ATARI7800_END)
endef

$(eval $(generic-package))
