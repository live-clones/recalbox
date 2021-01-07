################################################################################
#
# recalbox-romfs-thepowdertoy
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --port --readonly --system thepowdertoy --extension '.game .cps' --fullname 'The Powder Toy' --platform thepowdertoy --theme thepowdertoy 1:libretro:thepowdertoy:BR2_PACKAGE_LIBRETRO_THEPOWDERTOY

# Name the 3 vars as the package requires
RECALBOX_ROMFS_THEPOWDERTOY_SOURCE = 
RECALBOX_ROMFS_THEPOWDERTOY_SITE = 
RECALBOX_ROMFS_THEPOWDERTOY_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_THEPOWDERTOY = thepowdertoy
SYSTEM_XML_THEPOWDERTOY = $(@D)/$(SYSTEM_NAME_THEPOWDERTOY).xml
# System rom path
SOURCE_ROMDIR_THEPOWDERTOY = $(RECALBOX_ROMFS_THEPOWDERTOY_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_THEPOWDERTOY),)
define CONFIGURE_MAIN_THEPOWDERTOY_START
	$(call RECALBOX_ROMFS_CALL_ADD_PORT,$(SYSTEM_XML_THEPOWDERTOY),The Powder Toy,$(SYSTEM_NAME_THEPOWDERTOY),.game .cps,thepowdertoy,thepowdertoy,1)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_THEPOWDERTOY),)
define CONFIGURE_THEPOWDERTOY_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_THEPOWDERTOY),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_THEPOWDERTOY),y)
define CONFIGURE_THEPOWDERTOY_LIBRETRO_THEPOWDERTOY_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_THEPOWDERTOY),thepowdertoy,1)
endef
endif

define CONFIGURE_THEPOWDERTOY_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_THEPOWDERTOY))
endef
endif



define CONFIGURE_MAIN_THEPOWDERTOY_END
	$(call RECALBOX_ROMFS_CALL_END_PORT,$(SYSTEM_XML_THEPOWDERTOY),$(SOURCE_ROMDIR_THEPOWDERTOY),$(@D),thepowdertoy)
endef
endif

define RECALBOX_ROMFS_THEPOWDERTOY_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_THEPOWDERTOY_START)
	$(CONFIGURE_THEPOWDERTOY_LIBRETRO_START)
	$(CONFIGURE_THEPOWDERTOY_LIBRETRO_THEPOWDERTOY_DEF)
	$(CONFIGURE_THEPOWDERTOY_LIBRETRO_END)
	$(CONFIGURE_MAIN_THEPOWDERTOY_END)
endef

$(eval $(generic-package))

