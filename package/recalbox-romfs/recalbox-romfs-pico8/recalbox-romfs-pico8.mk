################################################################################
#
# recalbox-romfs-pico8
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system pico8 --extension '.p8 .png .zip .7z' --fullname 'PICO-8' --platform pico8 --theme pico8 1:libretro:pico8:BR2_PACKAGE_LIBRETRO_RETRO8

# Name the 3 vars as the package requires
RECALBOX_ROMFS_PICO8_SOURCE = 
RECALBOX_ROMFS_PICO8_SITE = 
RECALBOX_ROMFS_PICO8_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_PICO8 = pico8
SYSTEM_XML_PICO8 = $(@D)/$(SYSTEM_NAME_PICO8).xml
# System rom path
SOURCE_ROMDIR_PICO8 = $(RECALBOX_ROMFS_PICO8_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_RETRO8),)
define CONFIGURE_MAIN_PICO8_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_PICO8),PICO-8,$(SYSTEM_NAME_PICO8),.p8 .png .zip .7z,pico8,pico8)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_RETRO8),)
define CONFIGURE_PICO8_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_PICO8),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_RETRO8),y)
define CONFIGURE_PICO8_LIBRETRO_RETRO8_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_PICO8),pico8,1)
endef
endif

define CONFIGURE_PICO8_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_PICO8))
endef
endif



define CONFIGURE_MAIN_PICO8_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_PICO8),$(SOURCE_ROMDIR_PICO8),$(@D))
endef
endif

define RECALBOX_ROMFS_PICO8_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_PICO8_START)
	$(CONFIGURE_PICO8_LIBRETRO_START)
	$(CONFIGURE_PICO8_LIBRETRO_RETRO8_DEF)
	$(CONFIGURE_PICO8_LIBRETRO_END)
	$(CONFIGURE_MAIN_PICO8_END)
endef

$(eval $(generic-package))
