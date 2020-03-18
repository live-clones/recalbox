################################################################################
#
# recalbox-romfs-3do
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system 3do --extension '.iso .ISO .cue .CUE .chd .CHD' --fullname 'Panasonic 3DO' --platform 3do --theme 3do 1:libretro:opera:BR2_PACKAGE_LIBRETRO_OPERA

# Name the 3 vars as the package requires
RECALBOX_ROMFS_3DO_SOURCE = 
RECALBOX_ROMFS_3DO_SITE = 
RECALBOX_ROMFS_3DO_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_3DO = 3do
SYSTEM_XML_3DO = $(@D)/$(SYSTEM_NAME_3DO).xml
# System rom path
SOURCE_ROMDIR_3DO = $(RECALBOX_ROMFS_3DO_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_OPERA),)
define CONFIGURE_MAIN_3DO_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_3DO),Panasonic 3DO,$(SYSTEM_NAME_3DO),.iso .ISO .cue .CUE .chd .CHD,3do,3do)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_OPERA),)
define CONFIGURE_3DO_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_3DO),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_OPERA),y)
define CONFIGURE_3DO_LIBRETRO_OPERA_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_3DO),opera,1)
endef
endif

define CONFIGURE_3DO_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_3DO))
endef
endif



define CONFIGURE_MAIN_3DO_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_3DO),$(SOURCE_ROMDIR_3DO),$(@D))
endef
endif

define RECALBOX_ROMFS_3DO_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_3DO_START)
	$(CONFIGURE_3DO_LIBRETRO_START)
	$(CONFIGURE_3DO_LIBRETRO_OPERA_DEF)
	$(CONFIGURE_3DO_LIBRETRO_END)
	$(CONFIGURE_MAIN_3DO_END)
endef

$(eval $(generic-package))
