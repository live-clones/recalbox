################################################################################
#
# recalbox-romfs-gba
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system gba --extension '.gba .GBA .zip .ZIP' --fullname 'Game Boy Advance' --platform gba --theme gba libretro:gpsp:BR2_PACKAGE_LIBRETRO_GPSP libretro:mgba:BR2_PACKAGE_LIBRETRO_MGBA libretro:meteor:BR2_PACKAGE_LIBRETRO_METEOR

# Name the 3 vars as the package requires
RECALBOX_ROMFS_GBA_SOURCE = 
RECALBOX_ROMFS_GBA_SITE = 
RECALBOX_ROMFS_GBA_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_GBA = gba
SYSTEM_XML_GBA = $(@D)/$(SYSTEM_NAME_GBA).xml
# System rom path
SOURCE_ROMDIR_GBA = $(RECALBOX_ROMFS_GBA_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_GPSP)$(BR2_PACKAGE_LIBRETRO_MGBA)$(BR2_PACKAGE_LIBRETRO_METEOR),)
define CONFIGURE_MAIN_GBA_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_GBA),Game Boy Advance,$(SYSTEM_NAME_GBA),.gba .GBA .zip .ZIP,gba,gba)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_GPSP)$(BR2_PACKAGE_LIBRETRO_MGBA)$(BR2_PACKAGE_LIBRETRO_METEOR),)
define CONFIGURE_GBA_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_GBA),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_GPSP),y)
define CONFIGURE_GBA_LIBRETRO_GPSP_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBA),gpsp)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_MGBA),y)
define CONFIGURE_GBA_LIBRETRO_MGBA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBA),mgba)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_METEOR),y)
define CONFIGURE_GBA_LIBRETRO_METEOR_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_GBA),meteor)
endef
endif

define CONFIGURE_GBA_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_GBA))
endef
endif



define CONFIGURE_MAIN_GBA_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_GBA),$(SOURCE_ROMDIR_GBA),$(@D))
endef
endif

define RECALBOX_ROMFS_GBA_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_GBA_START)
	$(CONFIGURE_GBA_LIBRETRO_START)
	$(CONFIGURE_GBA_LIBRETRO_GPSP_DEF)
	$(CONFIGURE_GBA_LIBRETRO_MGBA_DEF)
	$(CONFIGURE_GBA_LIBRETRO_METEOR_DEF)
	$(CONFIGURE_GBA_LIBRETRO_END)
	$(CONFIGURE_MAIN_GBA_END)
endef

$(eval $(generic-package))
