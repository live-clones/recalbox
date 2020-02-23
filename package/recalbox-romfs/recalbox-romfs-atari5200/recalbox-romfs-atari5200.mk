################################################################################
#
# recalbox-romfs-atari5200
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system atari5200 --extension '.a52 .A52 .atr .ATR .bas .BAS .bin .BIN .car .CAR .dcm .DCM .xex .XEX .xfd .XFD .atr.gz .ATR.GZ .xfd.gz .XFD.GZ' --fullname 'Atari 5200' --platform atari5200 --theme atari5200 1:libretro:atari800:BR2_PACKAGE_LIBRETRO_ATARI800

# Name the 3 vars as the package requires
RECALBOX_ROMFS_ATARI5200_SOURCE = 
RECALBOX_ROMFS_ATARI5200_SITE = 
RECALBOX_ROMFS_ATARI5200_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_ATARI5200 = atari5200
SYSTEM_XML_ATARI5200 = $(@D)/$(SYSTEM_NAME_ATARI5200).xml
# System rom path
SOURCE_ROMDIR_ATARI5200 = $(RECALBOX_ROMFS_ATARI5200_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_ATARI800),)
define CONFIGURE_MAIN_ATARI5200_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_ATARI5200),Atari 5200,$(SYSTEM_NAME_ATARI5200),.a52 .A52 .atr .ATR .bas .BAS .bin .BIN .car .CAR .dcm .DCM .xex .XEX .xfd .XFD .atr.gz .ATR.GZ .xfd.gz .XFD.GZ,atari5200,atari5200)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_ATARI800),)
define CONFIGURE_ATARI5200_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_ATARI5200),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_ATARI800),y)
define CONFIGURE_ATARI5200_LIBRETRO_ATARI800_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_ATARI5200),atari800,1)
endef
endif

define CONFIGURE_ATARI5200_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_ATARI5200))
endef
endif



define CONFIGURE_MAIN_ATARI5200_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_ATARI5200),$(SOURCE_ROMDIR_ATARI5200),$(@D))
endef
endif

define RECALBOX_ROMFS_ATARI5200_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_ATARI5200_START)
	$(CONFIGURE_ATARI5200_LIBRETRO_START)
	$(CONFIGURE_ATARI5200_LIBRETRO_ATARI800_DEF)
	$(CONFIGURE_ATARI5200_LIBRETRO_END)
	$(CONFIGURE_MAIN_ATARI5200_END)
endef

$(eval $(generic-package))
