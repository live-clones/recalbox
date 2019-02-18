################################################################################
#
# recalbox-romfs-amigacd32
#
################################################################################

# Package generated with :
# ./scripts/linux/empack.py --system amigacd32 --extension '.uae .cue .CUE .ccd .CCD .iso .ISO .zip .ZIP' --fullname 'Amiga CD32' --platform amiga --theme amigacd32 BR2_PACKAGE_AMIBERRY

# Name the 3 vars as the package requires
RECALBOX_ROMFS_AMIGACD32_SOURCE = 
RECALBOX_ROMFS_AMIGACD32_SITE = 
RECALBOX_ROMFS_AMIGACD32_INSTALL_STAGING = NO
# Set the system name
SYSTEM_NAME_AMIGACD32 = amigacd32
SYSTEM_XML_AMIGACD32 = $(@D)/$(SYSTEM_NAME_AMIGACD32).xml
# System rom path
SOURCE_ROMDIR_AMIGACD32 = $(RECALBOX_ROMFS_AMIGACD32_PKGDIR)/roms

# CONFIGGEN_STD_CMD is defined in recalbox-romfs, so take good care that
# variables are global across buildroot


ifeq ($(BR2_PACKAGE_AMIBERRY),y)
define CONFIGURE_AMIGACD32
	$(call RECALBOX_ROMFS_CALL_ADD_STANDALONE_SYSTEM,$(SYSTEM_XML_AMIGACD32),Amiga CD32,$(SYSTEM_NAME_AMIGACD32),.uae .cue .CUE .ccd .CCD .iso .ISO .zip .ZIP,amiga,amigacd32,$(SOURCE_ROMDIR_AMIGACD32),$(@D))
endef
RECALBOX_ROMFS_AMIGACD32_CONFIGURE_CMDS += $(CONFIGURE_AMIGACD32)
endif

$(eval $(generic-package))
