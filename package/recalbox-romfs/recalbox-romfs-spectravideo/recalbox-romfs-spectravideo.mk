################################################################################
#
# recalbox-romfs-spectravideo
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --force --system spectravideo --extension '.cas .CAS .bin .BIN .zip .ZIP' --fullname 'Spectravideo' --platform spectravideo --theme spectravideo 1:libretro:bluemsx:BR2_PACKAGE_LIBRETRO_BLUEMSX

# Name the 3 vars as the package requires
RECALBOX_ROMFS_SPECTRAVIDEO_SOURCE = 
RECALBOX_ROMFS_SPECTRAVIDEO_SITE = 
RECALBOX_ROMFS_SPECTRAVIDEO_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_SPECTRAVIDEO = spectravideo
SYSTEM_XML_SPECTRAVIDEO = $(@D)/$(SYSTEM_NAME_SPECTRAVIDEO).xml
# System rom path
SOURCE_ROMDIR_SPECTRAVIDEO = $(RECALBOX_ROMFS_SPECTRAVIDEO_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),)
define CONFIGURE_MAIN_SPECTRAVIDEO_START
	$(call RECALBOX_ROMFS_CALL_ADD_SYSTEM,$(SYSTEM_XML_SPECTRAVIDEO),Spectravideo,$(SYSTEM_NAME_SPECTRAVIDEO),.cas .CAS .bin .BIN .zip .ZIP,spectravideo,spectravideo)
endef

ifneq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),)
define CONFIGURE_SPECTRAVIDEO_LIBRETRO_START
	$(call RECALBOX_ROMFS_CALL_START_EMULATOR,$(SYSTEM_XML_SPECTRAVIDEO),libretro)
endef
ifeq ($(BR2_PACKAGE_LIBRETRO_BLUEMSX),y)
define CONFIGURE_SPECTRAVIDEO_LIBRETRO_BLUEMSX_DEF
	$(call RECALBOX_ROMFS_CALL_ADD_CORE,$(SYSTEM_XML_SPECTRAVIDEO),bluemsx,1)
endef
endif

define CONFIGURE_SPECTRAVIDEO_LIBRETRO_END
	$(call RECALBOX_ROMFS_CALL_END_EMULATOR,$(SYSTEM_XML_SPECTRAVIDEO))
endef
endif



define CONFIGURE_MAIN_SPECTRAVIDEO_END
	$(call RECALBOX_ROMFS_CALL_END_SYSTEM,$(SYSTEM_XML_SPECTRAVIDEO),$(SOURCE_ROMDIR_SPECTRAVIDEO),$(@D))
endef
endif

define RECALBOX_ROMFS_SPECTRAVIDEO_CONFIGURE_CMDS
	$(CONFIGURE_MAIN_SPECTRAVIDEO_START)
	$(CONFIGURE_SPECTRAVIDEO_LIBRETRO_START)
	$(CONFIGURE_SPECTRAVIDEO_LIBRETRO_BLUEMSX_DEF)
	$(CONFIGURE_SPECTRAVIDEO_LIBRETRO_END)
	$(CONFIGURE_MAIN_SPECTRAVIDEO_END)
endef

$(eval $(generic-package))
