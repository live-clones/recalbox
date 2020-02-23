################################################################################
#
# recalbox-romfs-pcfx
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system pcfx --extension '.cue .CUE .ccd .CCD .toc .TOC .chd .CHD' --fullname 'NEC PC-FX' --platform pcfx --theme pcfx 1:libretro:mednafen_pcfx:BR2_PACKAGE_LIBRETRO_BEETLE_PCFX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PCFX_SOURCE = 
RECALBOX_ROMFS_PCFX_SITE = 
RECALBOX_ROMFS_PCFX_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PCFX = pcfx
SYSTEM_XML_PCFX = $(@D)/$(SYSTEM_NAME_PCFX).xml
# System rom path
SOURCE_ROMDIR_PCFX = $(RECALBOX_ROMFS_PCFX_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_PCFX),)
define CONFIGURE_MAIN_PCFX_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PCFX),NEC PC-FX,$(SYSTEM_NAME_PCFX),.cue .CUE .ccd .CCD .toc .TOC .chd .CHD,pcfx,pcfx)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BEETLE_PCFX),)
define CONFIGURE_PCFX_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PCFX),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BEETLE_PCFX),y)
define CONFIGURE_PCFX_LIBRETRO_MEDNAFEN_PCFX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PCFX),mednafen_pcfx,1)
endef
endif

define CONFIGURE_PCFX_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PCFX))
endef
endif



define CONFIGURE_MAIN_PCFX_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PCFX),$(SOURCE_ROMDIR_PCFX),$(@D))
endef
endif

define RECALBOX_ROMFS_PCFX_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PCFX_START)
	$(CONFIGURE_PCFX_LIBRETRO_START)
	$(CONFIGURE_PCFX_LIBRETRO_MEDNAFEN_PCFX_DEF)
	$(CONFIGURE_PCFX_LIBRETRO_END)
	$(CONFIGURE_MAIN_PCFX_END)
endef

$(eval $(generic-package))
