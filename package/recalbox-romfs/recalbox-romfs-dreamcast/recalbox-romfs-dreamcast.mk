################################################################################
#
# recalbox-romfs-dreamcast
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system dreamcast --extension '.gdi .GDI .cdi .CDI .chd .CHD' --fullname 'Sega Dreamcast' --platform dreamcast --theme dreamcast BR2_PACKAGE_REICAST

# Name the 3 vars as the package requires
RECALBOX_ROMFS_DREAMCAST_SOURCE = 
RECALBOX_ROMFS_DREAMCAST_SITE = 
RECALBOX_ROMFS_DREAMCAST_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_DREAMCAST = dreamcast
SYSTEM_XML_DREAMCAST = $(@D)/$(SYSTEM_NAME_DREAMCAST).xml
# System rom path
SOURCE_ROMDIR_DREAMCAST = $(RECALBOX_ROMFS_DREAMCAST_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_REICAST),y)
define CONFIGURE_DREAMCAST
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_DREAMCAST),Sega Dreamcast,$(SYSTEM_NAME_DREAMCAST),.gdi .GDI .cdi .CDI .chd .CHD,dreamcast,dreamcast,$(SOURCE_ROMDIR_DREAMCAST),$(@D))
endef
RECALBOX_ROMFS_DREAMCAST_CONFIGURE_CMDS += $(CONFIGURE_DREAMCAST)
endif

$(eval $(generic-package))
