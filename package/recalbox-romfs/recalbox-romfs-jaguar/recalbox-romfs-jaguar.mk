################################################################################
#
# recalbox-romfs-jaguar
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system jaguar --extension '.j64 .J64 .jag .JAG .rom .ROM .abs .ABS .cof .COF .bin .BIN .prg .PRG .zip .ZIP .7z .7Z' --fullname 'Atari Jaguar' --platform atarijaguar --theme jaguar 1:libretro:virtualjaguar:BR2_PACKAGE_LIBRETRO_VIRTUALJAGUAR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_JAGUAR_SOURCE = 
RECALBOX_ROMFS_JAGUAR_SITE = 
RECALBOX_ROMFS_JAGUAR_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_JAGUAR = jaguar
SYSTEM_XML_JAGUAR = $(@D)/$(SYSTEM_NAME_JAGUAR).xml
# System rom path
SOURCE_ROMDIR_JAGUAR = $(RECALBOX_ROMFS_JAGUAR_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_VIRTUALJAGUAR),)
define CONFIGURE_MAIN_JAGUAR_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_JAGUAR),Atari Jaguar,$(SYSTEM_NAME_JAGUAR),.j64 .J64 .jag .JAG .rom .ROM .abs .ABS .cof .COF .bin .BIN .prg .PRG .zip .ZIP .7z .7Z,atarijaguar,jaguar)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_VIRTUALJAGUAR),)
define CONFIGURE_JAGUAR_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_JAGUAR),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_VIRTUALJAGUAR),y)
define CONFIGURE_JAGUAR_LIBRETRO_VIRTUALJAGUAR_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_JAGUAR),virtualjaguar,1)
endef
endif

define CONFIGURE_JAGUAR_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_JAGUAR))
endef
endif



define CONFIGURE_MAIN_JAGUAR_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_JAGUAR),$(SOURCE_ROMDIR_JAGUAR),$(@D))
endef
endif

define RECALBOX_ROMFS_JAGUAR_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_JAGUAR_START)
	$(CONFIGURE_JAGUAR_LIBRETRO_START)
	$(CONFIGURE_JAGUAR_LIBRETRO_VIRTUALJAGUAR_DEF)
	$(CONFIGURE_JAGUAR_LIBRETRO_END)
	$(CONFIGURE_MAIN_JAGUAR_END)
endef

$(eval $(generic-package))
