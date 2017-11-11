################################################################################
#
# recalbox-romfs-psx
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system psx --extension '.img .IMG .pbp .PBP .cue .CUE .iso .ISO .ccd .CCD .cbn .CBN .mu3 .MU3 .chd .CHD' --fullname 'Sony Playstation 1' --platform psx --theme psx libretro:pcsx_rearmed:BR2_PACKAGE_LIBRETRO_PCSX libretro:mednafen_psx:BR2_PACKAGE_LIBRETRO_BEETLE_PSX libretro:mednafen_psx_hw:BR2_PACKAGE_LIBRETRO_BEETLE_PSX_HW

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PSX_SOURCE = 
RECALBOX_ROMFS_PSX_SITE = 
RECALBOX_ROMFS_PSX_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PSX = psx
SYSTEM_XML_PSX = $(@D)/$(SYSTEM_NAME_PSX).xml
# System rom path
SOURCE_ROMDIR_PSX = $(RECALBOX_ROMFS_PSX_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_PCSX)$(BR2_PACKAGE_LIBRETRO_BEETLE_PSX)$(BR2_PACKAGE_LIBRETRO_BEETLE_PSX_HW),)
define CONFIGURE_MAIN_PSX_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PSX),Sony Playstation 1,$(SYSTEM_NAME_PSX),.img .IMG .pbp .PBP .cue .CUE .iso .ISO .ccd .CCD .cbn .CBN .mu3 .MU3 .chd .CHD,psx,psx)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_PCSX)$(BR2_PACKAGE_LIBRETRO_BEETLE_PSX)$(BR2_PACKAGE_LIBRETRO_BEETLE_PSX_HW),)
define CONFIGURE_PSX_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PSX),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_PSX),y)
define CONFIGURE_PSX_LIBRETRO_MEDNAFEN_PSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PSX),mednafen_psx)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_PSX_HW),y)
define CONFIGURE_PSX_LIBRETRO_MEDNAFEN_PSX_HW_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PSX),mednafen_psx_hw)
endef
endif

ifeq ($(BR2_PACKAGE_LIBRETRO_PCSX),y)
define CONFIGURE_PSX_LIBRETRO_PCSX_REARMED_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PSX),pcsx_rearmed)
endef
endif

define CONFIGURE_PSX_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PSX))
endef
endif



define CONFIGURE_MAIN_PSX_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PSX),$(SOURCE_ROMDIR_PSX),$(@D))
endef
endif

define RECALBOX_ROMFS_PSX_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PSX_START)
	$(CONFIGURE_PSX_LIBRETRO_START)
	$(CONFIGURE_PSX_LIBRETRO_MEDNAFEN_PSX_DEF)
	$(CONFIGURE_PSX_LIBRETRO_MEDNAFEN_PSX_HW_DEF)
	$(CONFIGURE_PSX_LIBRETRO_PCSX_REARMED_DEF)
	$(CONFIGURE_PSX_LIBRETRO_END)
	$(CONFIGURE_MAIN_PSX_END)
endef

$(eval $(generic-package))
