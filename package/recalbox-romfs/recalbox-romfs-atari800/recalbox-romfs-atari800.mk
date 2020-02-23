################################################################################
#
# recalbox-romfs-atari800
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system atari800 --extension '.xfd .XFD .atr .ATR .atx .ATX .cdm .CDM .cas .CAS .bin .BIN .xex .XEX .zip .ZIP' --fullname 'Atari 800' --platform atari800 --theme atari800 1:libretro:atari800:BR2_PACKAGE_LIBRETRO_ATARI800

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ATARI800_SOURCE = 
RECALBOX_ROMFS_ATARI800_SITE = 
RECALBOX_ROMFS_ATARI800_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ATARI800 = atari800
SYSTEM_XML_ATARI800 = $(@D)/$(SYSTEM_NAME_ATARI800).xml
# System rom path
SOURCE_ROMDIR_ATARI800 = $(RECALBOX_ROMFS_ATARI800_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_ATARI800),)
define CONFIGURE_MAIN_ATARI800_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ATARI800),Atari 800,$(SYSTEM_NAME_ATARI800),.xfd .XFD .atr .ATR .atx .ATX .cdm .CDM .cas .CAS .bin .BIN .xex .XEX .zip .ZIP,atari800,atari800)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_ATARI800),)
define CONFIGURE_ATARI800_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ATARI800),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_ATARI800),y)
define CONFIGURE_ATARI800_LIBRETRO_ATARI800_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ATARI800),atari800,1)
endef
endif

define CONFIGURE_ATARI800_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ATARI800))
endef
endif



define CONFIGURE_MAIN_ATARI800_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ATARI800),$(SOURCE_ROMDIR_ATARI800),$(@D))
endef
endif

define RECALBOX_ROMFS_ATARI800_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ATARI800_START)
	$(CONFIGURE_ATARI800_LIBRETRO_START)
	$(CONFIGURE_ATARI800_LIBRETRO_ATARI800_DEF)
	$(CONFIGURE_ATARI800_LIBRETRO_END)
	$(CONFIGURE_MAIN_ATARI800_END)
endef

$(eval $(generic-package))
